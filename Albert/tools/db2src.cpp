/*
 *  db2src.cpp
 *  Albert
 *
 *  Created by Matthias Melcher on 08.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

//#define VERBOSE

#if VERBOSE
#define VERB1
#define VERB2
#define VERB3
#define VERB4
#define VERB5 if (0)
#else
#define VERB1
#define VERB2
#define VERB3 if (0) 
#define VERB4 if (0) 
#define VERB5 if (0) 
#endif

#if 1
#define ABORT_SCAN return
#define ABORT_SCAN_0 return 0
#else
#define ABORT_SCAN throw "ERROR: aborting scan"
#define ABORT_SCAN_0 throw "ERROR: aborting scan"
#endif


#include "db2src.h"

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <map>

#define BDISP(x) ((((x) & 0xffffff) ^ 0x800000) - 0x800000) /* 26 bit */

bool gCodeCoverageChecked = false;

extern int disarm(char *dst, unsigned int addr, unsigned int cmd);
extern const char *getSymbol(unsigned int i);
extern const char *getSafeSymbol(unsigned int i);

const char *src_file_path = __FILE__;
char base_path[2048] = { 0 };
char filename_buffer[2048] = { 0 };

//const char *db_path = "./";
const char *db_path = ""; // base_path
const char *src_path = "src/";
const char *c_path = "src/"; 
const char *cpp_path = "src/"; 
const char *os_path = "NewtonOS/"; 

const unsigned int flags_type_mask          = 0x000000ff;

const unsigned int flags_type_unknown       = 0x00000000;
const unsigned int flags_type_arm_code      = 0x00000001;
const unsigned int flags_type_arm_byte      = 0x00000002;
const unsigned int flags_type_arm_word      = 0x00000003;
const unsigned int flags_type_arm_text      = 0x00000004;
const unsigned int flags_type_patch_table   = 0x00000005;
const unsigned int flags_type_jump_table    = 0x00000006;
const unsigned int flags_type_unused        = 0x00000007;
const unsigned int flags_type_rex           = 0x00000008;
const unsigned int flags_type_ns            = 0x00000009;
const unsigned int flags_type_ns_obj        = 0x0000000a;
const unsigned int flags_type_ns_ref        = 0x0000000b;
const unsigned int flags_type_dict          = 0x0000000c;
const unsigned int flags_type_classinfo     = 0x0000000d;
const unsigned int flags_type_arm_wtext     = 0x0000000e;
const unsigned int flags_type_data          = 0x0000000f;

const unsigned int flags_is_function        = 0x30000000;
const unsigned int flags_is_target          = 0x10000000;
const unsigned int flags_walked             = 0x40000000;
const unsigned int flags_include            = 0x80000000;


const char *type_lut[] = {
  "flags_type_unknown",
  "flags_type_arm_code",
  "flags_type_arm_byte",
  "flags_type_arm_word",
  "flags_type_arm_text",
  "flags_type_patch_table",
  "flags_type_jump_table",
  "flags_type_unused",
  "flags_type_rex",
  "flags_type_ns",
  "flags_type_ns_obj",
  "flags_type_ns_ref",
  "flags_type_dict",
  "flags_type_classinfo",
  "flags_type_arm_wtext",
  "flags_type_data",
  "flags_type_include",
};

unsigned char ROM[0x00800000];
unsigned int ROM_flags[0x00200000];

typedef std::multimap < unsigned int, const char* > AlROMComment;
typedef std::multimap < unsigned int, const char* >::iterator AlROMCommentIterator;
typedef std::pair<AlROMCommentIterator,AlROMCommentIterator> AlROMCommentIteratorPair;
AlROMComment ROMComment;

unsigned char printable(unsigned char c) { return (c>32&&c<127)?c:'.'; }

const char *p_ascii(unsigned char c) 
{ 
  static char buf[8];
  switch (c) {
    case    8: return "\\b";
    case    9: return "\\t";
    case   10: return "\\n";
    case   12: return "\\bf";
    case   13: return "\\r";
    case  '"': return "\\\"";
    case  '@': return "\\x40";
    case '\\': return "\\\\";
  }
  // 48:16 56:8 60:4 62:2
  if (c>=' ' && c<=126) {
    buf[0] = c; buf[1] = 0; return buf;
  }
  sprintf(buf, "\\%03o", c);
  return buf;
}

const char *four_cc(unsigned int x)
{
  static char buf[5];
  unsigned char c = x>>24;
  buf[0] = (c>31 && c<127) ? c : '.';
  c = x>>16;
  buf[1] = (c>31 && c<127) ? c : '.';
  c = x>>8;
  buf[2] = (c>31 && c<127) ? c : '.';
  c = x;
  buf[3] = (c>31 && c<127) ? c : '.';
  buf[4] = 0;
  return buf;
}

void AsmFlush(FILE *f, const char *buf)
{
  char dbuf[4096];
  // manipulate the buffer so that all comments are either starting at column 0
  // or at column 48 if possible
  const char *s = buf;
  char *d = dbuf;
  int col=0, t=0, x=0; 
  // walk to the comment character
  for (;;) {
    char c = *s;
    switch (c) {
      case 0: *d=0; x=1; break;
      case '@': x=1; break;
      case '\t': *d++=c; col = (col+8) & 0xfff8; s++; break;
      case ' ': *d++=c; col++; s++; break;
      default: *d++=c; col++; t=1; s++; break;
    }
    if (x) break;
  }
  // handle a comment if there is one
  if (*s=='@') {
    if (t) {
      // we have text, set the comment at col 48
      while (col<48) {
        *d++ = '\t';
        col = (col+8) & 0xfff8;
      }
      strcpy(d, s); // append the comment to the line
    } else {
      // no text yet: move the comment to the start
      strcpy(dbuf, s); // simply move the text to the start of the line
    }
  }
  fputs(dbuf, f);
}

void AsmPrintf(FILE *f, const char *pat, ...) 
{
  static char buf[4096];
  static char *dst = buf;
  // concatenate segments until we can flush an entire line
  va_list va;
  va_start(va, pat);
  vsnprintf(dst, 4096, pat, va);
  va_end(va);
  dst = buf + strlen(buf);
  if (strchr(buf, '\n')) {
    AsmFlush(f, buf);
    dst = buf; buf[0] = 0;
  }
}


unsigned long rotate_right(unsigned long n, unsigned long i)
{
  return (n >> i) | (n << (32 - i));
}

void rom_add_comment(unsigned int addr, const char *pat, ...)
{
  static char buf[4096];
  va_list va;
  va_start(va, pat);
  vsnprintf(buf, 4096, pat, va);
  va_end(va);
  ROMComment.insert(std::make_pair(addr, strdup(buf)));
}

/**
 * Return the 4-byte word in rom
 */
unsigned int rom_w(unsigned int addr)
{
  if (addr>=0x00800000)
    return 0;
  else
    return (ROM[addr]<<24)|(ROM[addr+1]<<16)|(ROM[addr+2]<<8)|ROM[addr+3];
}

/**
 * Return the 2-byte short in rom
 */
unsigned int rom_s(unsigned int addr)
{
  if (addr>=0x00800000)
    return 0;
  else
    return (ROM[addr]<<8)|(ROM[addr+1]);
}

/**
 * Return a 8-byte real (double prcision floating point) in rom
 */
double rom_real(unsigned int addr)
{
  union { unsigned char c[8]; double v; };
  if (addr>=0x00800000)
    return 0.0;
  else {
    int i;
    for (i=0; i<8; i++)
      c[7-i] = ROM[addr+i];
    return v;
  }
}

/**
 * Return a 8-byte double form a fixed decimal point 4 byte variable
 */
double rom_fixed(unsigned int addr)
{
  int si = (signed int)rom_w(addr);
  double d = si;
  d = d/65536.0;
  return d;
}

/**
 * Return the 4-byte flag for a ROM word
 */
unsigned int rom_flags(unsigned int addr)
{
  if (addr>=0x00800000)
    return 0;
  else
    return ROM_flags[addr/4];
}

void rom_flags_set(unsigned int addr, unsigned int f)
{
  if (addr<0x00800000)
    ROM_flags[addr/4] |= f;
}

void rom_flags_set(unsigned int addr, unsigned int end, unsigned int f)
{
  for ( ; addr<end; addr++)
    if (addr<0x00800000)
      ROM_flags[addr/4] |= f;
}

void rom_flags_clear(unsigned int addr, unsigned int f)
{
  if (addr<0x00800000)
    ROM_flags[addr/4] &= ~f;
}

void rom_flags_clear_all(unsigned int f)
{
  unsigned int i;
  f = ~f;
  for (i = 0; i<0x00200000; i++)
    ROM_flags[i] &= f;
}

char rom_flags_is_set(unsigned int addr, unsigned int f)
{
  if (addr>=0x00800000) return 0;
  return ((ROM_flags[addr/4]&f) == f);
}

void rom_flags_type(unsigned int addr, unsigned int t)
{
  if (addr<0x00800000)
    ROM_flags[addr/4] = (ROM_flags[addr/4]&~flags_type_mask) | t;
}

void rom_flags_type(unsigned int addr, unsigned int end, unsigned int t)
{
  rom_flags_set(addr, flags_is_target);
  for ( ; addr<end; addr++)
    rom_flags_type(addr, t);
}

unsigned int rom_flags_type(unsigned int addr)
{
  if (addr<0x00800000)
    return (ROM_flags[addr/4]&flags_type_mask);
  else
    return 0;
}


class AlClass;
class AlArg;
class AlMemberFunction;

//typedef std::map < unsigned int, const char* > OldSymbolList;
//OldSymbolList symbolList, plainSymbolList;

char *decodeCppSymbol(const char*);

const char *get_symbol_at(unsigned int addr)
{
#if 0
  // FIXME: this should return the CPP decoded function call
  AlData *data = gMemoryMap.find(addr);
  if (data) {
    AlCPPMethod *m = dynamic_cast<AlCPPMethod*>(data);
    if (m) {
      return m->prototype();
    } else {
      return 0L;
    }
  } else {
    return 0;
  }
#else
  const char *sym = get_plain_symbol_at(addr);
  if (!sym) {
    return 0;
  }
  const char *cps = decodeCppSymbol(sym);
  if (cps) {
    return cps;
  } else {
    return 0;
  }
#endif
}

const char *get_plain_symbol_at(unsigned int addr)
{
  AlData *data = gMemoryMap.find(addr);
  if (data)
    return data->label();
  else
    return 0;
}

typedef struct { const char *key; unsigned int addr; } MagicSym;
MagicSym magicSym[] = {
#include "magics1.h"
#include "magics2.h"
};

MagicSym unlabeledSym[] = {
#include "unlabeled.h"
  { 0, 0 }
};

void readSymbols(const char *cpp_filename, const char *plain_filename)
{
  int i;
  // unlabeled functions that are called from within the code
  for (i=0; unlabeledSym[i].key; i++) {
    gMemoryMap.at(unlabeledSym[i].addr)->label(unlabeledSym[i].key);
  }
  // magic pointers
  for (i=0; magicSym[i].key; i++) {
    unsigned int maddr = rom_w(0x003AF004+4*magicSym[i].addr) & 0xfffffffc;
    gMemoryMap.at(maddr)->label(magicSym[i].key);
  }
  // we can find many NewtonScript labels by checking Rbuiltinfunctions
  // do this first so that the values may be overridden later
  {
    unsigned int frame = 0x00639580;
    unsigned int symlist = rom_w(frame+8)&~3;
    unsigned int i, n = (rom_w(frame)>>10)-3;
    for (i=0; i<n; i++) {
      unsigned int addr = rom_w(frame+12+4*i);
      unsigned int sym = rom_w(symlist+16+4*i);
      if ((addr&3)==1 && (sym&3)==1) {
        const char *str = getSafeSymbol(symlist+16+4*i);
        if (str) {
          char buf[4096];
          sprintf(buf, "NSFn_%s", str);
          gMemoryMap.at(addr&~3)->label(buf);
        }
      }
    }
  }
  // more from files
  FILE *f = fopen(cpp_filename, "rb");
  if (!f) {
    puts("ERROR opening cpp symbol file");
    return;
  }
  for (;;) {
    char buf[1024], sym[1024];
    unsigned int addr;
    char *s = fgets(buf, 1024, f);
    if (!s) break;
    int n = sscanf(s, "0x%08X %[^\n]\n", &addr, sym);
    if (n==2 && AlCPPMethod::isCPPLabel(sym)) {
      AlCPPMethod *m = new AlCPPMethod();
      gMemoryMap.set(addr, m);
      m->decodeDecoratedLabel(sym);
    }
  }
  f = fopen(plain_filename, "rb");
  if (!f) {
    puts("ERROR opening plain symbol file");
    return;
  }
  for (;;) {
    char buf[1024], sym[1024];
    unsigned int addr;
    char *s = fgets(buf, 1024, f);
    if (!s) break;
    int n = sscanf(s, "0x%08X %[^\n]\n", &addr, sym);
    if (n==2) {
      gMemoryMap.at(addr)->label(sym);
    }
  }
}

void writeSymbolStatistics(const char *filename)
{
  FILE *st = fopen(filename, "wb");
  unsigned int i, pi = 0;
  const char *pSym = get_plain_symbol_at(0);
  for (i=4; i<0x00800000; i+=4) {
    const char *sym = get_symbol_at(i);
    if (!sym)
      sym = get_plain_symbol_at(i);
    if (sym) {
      fprintf(st, "%10d - %s\n", i-pi, pSym);
      pSym = sym;
      pi = i;
    }
  }
  fclose(st);
}

char *gComment = 0;
int gnComment = 0, gNComment = 0;

void addComment(const char *s)
{
  int n = strlen(s) + gnComment + 2;
  if (n>gNComment) {
    gNComment = n+512;
    gComment = (char*)realloc(gComment, gNComment);
  }
  if (gComment[0]) {
    gComment[gnComment]='\n';
    strcpy(gComment+gnComment+1, s);
  } else {
    strcpy(gComment, s);
  }    
  gnComment = strlen(gComment);
}

void clearComment()
{
  gComment[0] = 0;
  gnComment = 0;
}

char hasComment() 
{
  return (gComment && *gComment);
}

const char *lastComment()
{
  return gComment;
}

char *dupComment()
{
  if (hasComment()) {
    char *ret = strdup(gComment);
    clearComment();
    return ret;
  } else {
    return 0;
  }
}

void printComment(FILE *f, const char *s, const char *start, const char *line=0)
{
  if (!s || !*s) return;
  if (!line) line = start;
  fprintf(f, "%s", start);
  for(;;) {
    char c = *s++;
    if (c==0) break;
    if (c=='\n') {
      fprintf(f, "\n%s", line);
      continue;
    }
    fputc(c, f);
  }
  fprintf(f, "\n");
  clearComment();
}

/**
 * Skip over space characters
 */
char *skipSpace(char *s) {
  while (isspace(*s)) s++;
  return s;
}

char const *skipSpace(char const *s) {
  while (isspace(*s)) s++;
  return s;
}



class AlCString
{
public:
  static AlCString **pCString;
  static int pn, pN;
  unsigned int pAt, pNext;
  const char *pSym;
  const char *pString;
  const char *pComment;
public:
  AlCString(FILE *f) {
    pAt = pNext = 0xffffffff;
    pSym = 0;
    pString = 0;
    for (;;) {
      char buf[4096], cmd[32], arg[4096];
      char *s = fgets(buf, 256, f);
      if (*s==0) break;
      s = skipSpace(s);
      if (sscanf(s, "%s", cmd)==0) continue;
      if (strcmp(cmd, "end")==0) {
        break;
      } else if (strcmp(cmd, "//")==0) {
        int sn = strlen(s);
        if (s[sn-1]=='\n') s[sn-1]=0;
        addComment(s+2);
      } else if (strcmp(cmd, "at")==0) {
        sscanf(s, "%s 0x%08X", cmd, &pAt);
      } else if (strcmp(cmd, "next")==0) {
        sscanf(s, "%s 0x%08X", cmd, &pNext);
      } else if (strcmp(cmd, "sym")==0) {
        sscanf(s, "%s %s", cmd, arg);
        pSym = strdup(arg);
        pComment = dupComment();
      } else {
        printf("WARNING: unsupported CString attribute '%s'\n", cmd);
      }
    }
    add(this);
  }
  void write_h(FILE *f) {
    // write the doxygen commentary
    if (pComment) {    
      fprintf(f, "\n/**\n");
      printComment(f, pComment, " * ");
      fprintf(f, " *\n");
    }
    // write cstring header
    fprintf(f, "extern const char *%s;\n", pSym);
  }
  void write_c(FILE *f) {
    int i, last, x = 0;
    // write the C++ function
    fprintf(f, "const char *%s = \n                \"", pSym);
    for (last=pNext-1; last>pAt; last--) {
      if (ROM[last]) break;
    }
    for (i=pAt; i<=last; i++,x++) {
      if (x==63) { fprintf(f, "\"\n                \""); x=0; }
      char c = ROM[i];
      if (c==0) fprintf(f, "\\0");
      else if (c=='\n') fprintf(f, "\\n");
      else if (c=='\r') fprintf(f, "\\r");
      else if (c=='"') fprintf(f, "\\\"");
      else if (c=='\\') fprintf(f, "\\\\");
      else if (c<32 || c>126) fprintf(f, "\\%03o", (unsigned char)c);
      else fputc(c, f);
    }
    fprintf(f, "\";\n");
  }
  static AlCString *add(AlCString *str) {
    if (pn==pN) {
      pN += 100;
      pCString = (AlCString**)realloc(pCString, sizeof(AlCString**)*pN);
    }
    pCString[pn++] = str;
    return str;
  }
  static void write_all_h(FILE *f) {
    int i;
    for (i=0; i<pn; i++) 
      pCString[i]->write_h(f);
  }
  static void write_all_c(FILE *f) {
    int i;
    for (i=0; i<pn; i++) 
      pCString[i]->write_c(f);
  }
};

AlCString **AlCString::pCString;
int AlCString::pn, AlCString::pN;



class AlClass
{
public:
  static AlClass **pClass;
  static int pn, pN;
  const char *pSym;
  const char *pBaseClass;
  AlMemberFunction *pFn[200];
  int pnf;
public:
  static AlClass *find(char const *sym) {
    int i;
    for (i=0; i<pn; i++) {
      if (strcmp(sym, pClass[i]->pSym)==0)
        return pClass[i];
    }
    if (pn==pN) {
      pN += 100;
      pClass = (AlClass**)realloc(pClass, sizeof(AlClass**)*pN);
    }
    AlClass *c = new AlClass(sym);
    pClass[pn++] = c;
    return c;
  }
  static void load(FILE *f) {
    AlClass *ac = 0L;
    for (;;) {
      char buf[4096], cmd[32], arg[4096];
      char *s = fgets(buf, 256, f);
      if (*s==0) break;
      s = skipSpace(s);
      if (sscanf(s, "%s", cmd)==0) continue;
      if (strcmp(cmd, "end")==0) {
        break;
      } else if (strcmp(cmd, "sym")==0) {
        sscanf(s, "%s %s", cmd, arg);
        ac = find(arg);
      } else if (strcmp(cmd, "base")==0) {
        sscanf(s, "%s %s", cmd, arg);
        if (strcmp(arg, "FIXME")!=0) {
          if (ac) ac->base_class(arg);
        }        
      } else if (strcmp(cmd, "size")==0) {
      } else if (strcmp(cmd, "datac")==0) {
      } else if (strcmp(cmd, "data")==0) {
      } else if (strcmp(cmd, "file")==0) {
      } else if (strcmp(cmd, "//")==0) {
      } else if (strcmp(cmd, "#")==0) {
      } else {
        printf("WARNING: unsupported member function attribute '%s'\n", cmd);
      }
    }
  }
  static void write_all(char const *path) {
    int i;
    for (i=0; i<pn; i++) {
      pClass[i]->write(path);
    }
  }
  void init() {
    pSym = 0;
    pBaseClass = 0;
    pnf = 0;
  }
  AlClass(char const *sym) {
    init();
    pSym = strdup(sym);
  }
  void add(AlMemberFunction *fn) {
    if (pnf>=200) puts("Out of bounds!");
    pFn[pnf++] = fn;
  }
  void base_class(const char *name) {
    pBaseClass = strdup(name);
  }
  const char *base_class() {
    return pBaseClass;
  }
  void write_h(char const *path);
  void write_cpp(char const *path);
  void write(char const *path) {
    write_h(path);
    write_cpp(path);
  }
};

AlClass **AlClass::pClass;
int AlClass::pn, AlClass::pN;


class AlArg 
{
public:
  const char *pType;
  const char *pSym;
  const char *pComment;
public:
  AlArg(char *cmd) {
    pType = 0;
    pSym = 0;
    pComment = 0;
    char *type = cmd, *sym = 0, *cmt = 0;
    cmt = strstr(type, "//");
    if (cmt) { *cmt = 0; cmt+=2; }
    sym = strchr(type, '/');
    if (sym && sym[1]!='/') { *sym = 0; sym++; }
    if (type) pType = strdup(type);
    if (sym) pSym = strdup(sym);
    if (cmt) 
      pComment = strdup(cmt);
    else
      pComment = dupComment();
  }
  void write_h(FILE *f, int i) {
    if (pSym) {
      fprintf(f, "%s %s", pType, pSym);
    } else {
      fprintf(f, "%s arg%d", pType, i);
    }
  }
  void write_c(FILE *f, int i) {
    if (pSym) {
      fprintf(f, "%s %s", pType, pSym);
    } else {
      fprintf(f, "%s arg%d", pType, i);
    }
  }
  void write_doxy(FILE *f, int i) {
    if (pSym) {
      fprintf(f, " * \\param %s", pSym);
    } else {
      fprintf(f, " * \\param arg%d", i);
    }
    if (pComment)
      printComment(f, pComment, " ", " *   ");
    else
      fprintf(f, "\n");
  }
};


class AlMemberFunction
{
public:
  unsigned int pAt, pNext;
  const char *pSym;
  const char *pReturns;
  const char *pComment;
  AlClass *pClass;
  AlArg **pArg;
  int pnArg;
  char pIsStatic, pIsConst, pIsCtor, pIsDtor;
public:
  AlMemberFunction(FILE *f) {
    pAt = pNext = 0xffffffff;
    pSym = 0;
    pReturns = 0;
    pComment = 0;
    pClass = 0;
    pArg = 0;
    pnArg = 0;
    pIsStatic = pIsConst = pIsCtor = pIsDtor = 0;
    for (;;) {
      char buf[4096], cmd[32], arg[4096];
      char *s = fgets(buf, 256, f);
      if (*s==0) break;
      s = skipSpace(s);
      if (sscanf(s, "%s", cmd)==0) continue;
      if (strcmp(cmd, "end")==0) {
        break;
      } else if (strcmp(cmd, "//")==0) {
        int sn = strlen(s);
        if (s[sn-1]=='\n') s[sn-1]=0;
        addComment(s+2);
      } else if (strcmp(cmd, "at")==0) {
        sscanf(s, "%s 0x%08X", cmd, &pAt);
      } else if (strcmp(cmd, "next")==0) {
        sscanf(s, "%s 0x%08X", cmd, &pNext);
      } else if (strcmp(cmd, "static")==0) {
        pIsStatic = 1;
      } else if (strcmp(cmd, "const")==0) {
        pIsConst = 1;
      } else if (strcmp(cmd, "sym")==0) {
        sscanf(s, "%s %s", cmd, arg);
        pSym = strdup(arg);
        pComment = dupComment();
        if (strcmp(pSym, pClass->pSym)==0)
          pIsCtor = 1;
        if (pSym[0]=='~' && strcmp(pSym+1, pClass->pSym)==0)
          pIsDtor = 1;
      } else if (strcmp(cmd, "returns")==0) {
        sscanf(s, "%s %[^\n]", cmd, arg);
        if (strcmp(arg, "FIXME")==0) {
          if (pIsCtor || pIsDtor)
            pReturns = 0;
          else
            pReturns = strdup("t_unknown");
        } else if (strcmp(arg, "NONE")==0) {
          pReturns = 0;
        } else {
          pReturns = strdup(arg);
        }
        //pReturnComment = dupComment();
      } else if (strcmp(cmd, "argc")==0) {
        sscanf(s, "%s %d", cmd, &pnArg);
        pArg = (AlArg**)calloc(sizeof(AlArg**), pnArg);
      } else if (strcmp(cmd, "arg")==0) {
        int l = strlen(s); if (s[l-1]=='\n') s[l-1] = 0;
        int ix;
        sscanf(s, "%s %d", cmd, &ix);
        s = strchr(s, ' '); if (!s) continue;
        s = strchr(s+1, ' '); if (!s) continue;
        pArg[ix] = new AlArg(s+1);
      } else if (strcmp(cmd, "class")==0) {
        sscanf(s, "%s %s", cmd, arg);
        AlClass *c = AlClass::find(arg);
        pClass = c; c->add(this);
      } else {
        printf("WARNING: unsupported member function attribute '%s'\n", cmd);
      }
    }
  }
  void write_h(FILE *f) {
    int i;
    // write function header
    fprintf(f, "  ");
    if (pIsStatic)
      fprintf(f, "static ");
    if (pReturns)
      fprintf(f, "%s ", pReturns);
    fprintf(f, "%s(", pSym);
    for (i=0; i<pnArg; i++) {
      if (i>0) fprintf(f, ", ");
      pArg[i]->write_h(f, i);
    }
    fprintf(f, ")");
    if (pIsConst)
      fprintf(f, " const");
    fprintf(f, ";\n");
  }
  void write_cpp(FILE *f) {
    int i;
    // write the doxygen commentary
    fprintf(f, "/**\n");
    if (pComment)
      printComment(f, pComment, " * ");
    else
      fprintf(f, " * Member function %s of class %s.\n", pSym, pClass->pSym);
    fprintf(f, " *\n");
    for (i=0; i<pnArg;i++) {
      pArg[i]->write_doxy(f, i);
    }
    if (pReturns) {
      fprintf(f, " * \\returns %s\n", pReturns);
    }
    fprintf(f, " */\n");
    // write the C++ function
    if (pReturns)
      fprintf(f, "%s ", pReturns);
    fprintf(f, "%s::%s(", pClass->pSym, pSym);
    for (i=0; i<pnArg; i++) {
      if (i>0) fprintf(f, ", ");
      pArg[i]->write_c(f, i);
    }
    fprintf(f, ")");
    if (pIsConst)
      fprintf(f, " const");
    fprintf(f, "\n{\n");
    // find the program workflow (goto-stacking)
    // a typical switch-case statement starts with "add pc, pc, r#, lsl #2" = E08FF101
    //   any register r can be used, other logical shifts have been used
    //   "add" is followed by a "nop" because the ip is already one instruction further down
    //   "addls" is followed by a jump to the "default" branch
    
    // find all "b" instructions and tag the branch destinations
    for (i=pAt; i<pNext; i+=4) {
      unsigned int cmd = rom_w(i);
      if ((cmd&0x0f000000)==0x0a000000) {
        rom_flags_set(BDISP(cmd)*4+i+8, 1);
      }
    }
    // write some simple C-style code (of course it is not real C code)
    for (i=pAt; i<pNext; i+=4) {
      char buf[4096];
      if (rom_flags(i)&1) 
        fprintf(f, "L%08X:\n", i);
      disarm_c(buf, i, rom_w(i));
      fprintf(f, "  %s\n", buf);
    }
    fprintf(f, "}\n\n");
  }
};


void AlClass::write_h(char const *path) {
  char filename[4096];
  sprintf(filename, "%s%s.h", path, pSym);
  FILE *f = fopen(filename, "wb");
  fprintf(f, "// This file was automatically generated by Albert\n\n");
  fprintf(f, "#ifndef %s_H\n#define %s_H\n\n", pSym, pSym);
  fprintf(f, "#include \"albert/types.h\"\n\n");
  if (pBaseClass)
    fprintf(f, "class %s : public %s\n{\npublic:\n", pSym, pBaseClass);
  else
    fprintf(f, "class %s\n{\npublic:\n", pSym);
  int i;
  for (i=0; i<pnf; i++) {
    pFn[i]->write_h(f);
  }
  fprintf(f, "};\n\n");
  fprintf(f, "\n#endif\n");
}

void AlClass::write_cpp(char const *path) {
  char filename[4096];
  sprintf(filename, "%s%s.cpp", path, pSym);
  FILE *f = fopen(filename, "wb");
  fprintf(f, "// This file was automatically generated by Albert\n\n");
  fprintf(f, "#include \"%s.h\"\n\n", pSym);
  // FIXME insert class description and comments here
  int i;
  for (i=0; i<pnf; i++) {
    pFn[i]->write_cpp(f);
  }
  fprintf(f, "\n\n");
}


/**
 * Read the entire database into RAM
 */
void load_db(char const *path, char const *filename)
{
  printf("Reading %s...\n", filename);
  char name[4096];
  strcpy(name, path); strcat(name, filename);
  FILE *f = fopen(name, "rb");
  if (!f) {
    printf("ERROR: can't open database file %s\n", name);
    return;
  }
  while (!feof(f)) {
    char buf[1024], cmd[80], arg[80];
    char *s = fgets(buf, 1024, f);
    if (!s) break;
    s = skipSpace(s);
    char *hash = strchr(s, '#');
    if (hash) *hash = 0;
    if (*s==0) continue;
    cmd[0] = arg[0] = 0;
    sscanf(s, "%s %s\n", cmd, arg);
    if (strcmp(cmd, "import")==0) {
      load_db(path, arg);
    } else if (strcmp(cmd, "begin")==0) {
      if (strcmp(arg, "cpp_member_function")==0) {
        // create a cpp member function and read it
        new AlMemberFunction(f);
      } else if (strcmp(arg, "class")==0) {
        AlClass::load(f);
      } else if (strcmp(arg, "file")==0) {
        goto skip; // FIXME:
      } else if (strcmp(arg, "c_file")==0) {
        goto skip; // FIXME:
      } else if (strcmp(arg, "cpp_function")==0) {
        goto skip; // FIXME:
      } else if (strcmp(arg, "c_function")==0) {
        goto skip; // FIXME:
      } else if (strcmp(arg, "cstring")==0) {
        // create a "C"-style ASCII string
        new AlCString(f);
      } else if (strcmp(arg, "RAM")==0) {
        goto skip; // FIXME:
      } else if (strcmp(arg, "nsSymbol")==0) {
        goto skip; // FIXME:
      } else if (strcmp(arg, "data")==0) {
        goto skip; // FIXME:
      } else if (strcmp(arg, "stub")==0) {
        goto skip; // FIXME:
      } else if (strcmp(arg, "global_data")==0) {
        goto skip; // FIXME:
      } else if (strcmp(arg, "const_data")==0) {
        goto skip; // FIXME:
      } else if (strcmp(arg, "unknown")==0) {
        goto skip; // FIXME:
      } else {
        // skip to "end"
        // FIXME 
        printf("WARNING: unsupported class '%s'\n", arg);
      skip:
        int depth = 1;
        for (;;) {
          s = fgets(buf, 1024, f);
          if (!s) break;
          s = skipSpace(s);
          cmd[0] = 0;
          sscanf(s, "%s", cmd);
          if (strcmp(cmd, "begin")==0) {
            depth++;
          } else if (strcmp(cmd, "end")==0) {
            depth--;
            if (depth==0)
              break;
          } else {
            // skip unknown command
          }
        }
      }
    } else {
      printf("WARNING: unsupported command '%s'\n", cmd);
    }
  }
}

unsigned int branch_address(unsigned int addr, unsigned int cmd)
{
  if (cmd==0xffffffff)
    cmd = rom_w(addr);
  if (cmd&0x00800000) { // jump backwards
    return (((cmd&0x00ffffff)|0xff000000)<<2)+addr+8;
  } else { // jump forward
    return ((cmd&0x007fffff)<<2)+addr+8;
  }
}

unsigned int branch_address_in_ROM(unsigned int addr, unsigned int cmd)
{
  unsigned int dest = branch_address(addr);
  if (dest>=0x01A00000 && dest<0x1D00000) {
    unsigned int next = ( ((dest>>5)&0xffffff80) | (dest&0x0000007f) ) - 0xCE000;
    dest = branch_address(dest, rom_w(next));
  }
  return dest;
}

void tagOffset(unsigned int addr, unsigned int cmd, unsigned int flags)
{
  if (((cmd & 0x000f0000) == 0x000f0000) && ((cmd & 0x02000000) == 0))
  {
    int offset = cmd & 0xfff;
    if ((cmd & 0x00800000) == 0)
      offset = -offset;
    rom_flags_set(offset+addr+8, flags);
  }
}

void check_code_coverage(unsigned int addr, unsigned int flags=0);

void check_switch_case(unsigned int addr, int n_case) 
{
  int i;
  printf("Switch/Case statement with %d cases starting at %08X\n", n_case, addr);
  for (i=0; i<n_case; i++) { // nuber of cases plus default case
    VERB4 printf("case %d at %08X\n", i, addr+4*i);
    check_code_coverage(addr+4*i, 0);
  }
}

// recursively dive into the ARM code from this point on and follow all possible
// execution paths
void check_code_coverage(unsigned int addr, unsigned int flags)
{
  // mark this as jump target
  if (addr<0x00800000 && flags) rom_flags_set(addr, flags); // mark this as a jump target

  for(;;) {
    
    if (addr>=0x00800000) {
      // see: http://40hz.org/Pages/Newton%20Patches
      if (addr>=0x01A00000 && addr<0x1D00000) {
        unsigned int prev = addr;
        unsigned int next = ( ((addr>>5)&0xffffff80) | (addr&0x0000007f) ) - 0xCE000;
        addr = branch_address(addr, rom_w(next));
        const char *prev_sym = get_symbol_at(prev);
        const char *addr_sym = get_symbol_at(addr);
        VERB3 printf("Redirecting call from %08X to %08X (%s->%s)\n", prev, addr, prev_sym, addr_sym);
        if ( prev!=0x01bdef54 && prev!=0x01bb294c && prev!=0x01bb4a50 && prev!=0x01bdef64 && prev!=0x01b4c658
            && (prev_sym==0 || addr_sym==0 || strcmp(prev_sym, addr_sym)!=0) ) {
          //VERB2 printf("ERROR: Symbols don't match. Verify lookup table offsets! At: 0x%08X (0x%08X)\n", prev, addr);
          //VERB2 printf("  (%s!=%s)\n", prev_sym, addr_sym);
          return;
        }
      } else {
        VERB2 printf("Can't follow addresses outside of ROM: %08X (%s)\n", addr, get_symbol_at(addr));
        return;
      }
    }
    
    // hmm, this is a dead end!
    if (addr==0x0001862C) return; // FIXME: dead end!
    
    // we verified this address already - leave
    if (rom_flags_type(addr)) return;
        
    // mark this as executable
    rom_flags_type(addr, flags_type_arm_code);
    
    // crude interpretation of commands
    unsigned int cmd = rom_w(addr);
    
    {
      char buf[1024]; memset(buf, 0, 1024);
      sprintf(buf, ":  ");
      disarm(buf+3, addr, cmd);
      VERB4 puts(buf);
    }
    
    if ( (cmd&0xf0000000) == 0xf0000000) {
      // special treatment for 0xf... commands
      VERB1 printf("Aborting: Hit unknown command at %08X: %08X\n", addr, cmd);
      return;
    } else {
      
      // ldr immediate test
      if ( (cmd&0x0e5f0000) == 0x041f0000) { // test for word access inside the ROM
        int offset = cmd & 0xfff;
        if ((cmd & 0x00800000) == 0) offset = -offset;
        VERB5 printf("  Read word at 0x%08X\n", addr+8+offset);
        rom_flags_type(addr+8+offset, flags_type_arm_word);
      }
      
      // follow execution threads
      if ( (cmd&0x0fefffff)==0x01a0f00e) { // quick return command (mov pc,lr)
        if ( (cmd&0xf0000000)==0xe0000000) return; // unconditional - we are done, otherwise continue
      } else if ( (cmd&0x0f000000) == 0x0a000000) { // jump instruction
        if ( (cmd&0xf0000000) == 0xe0000000) { // unconditional
          VERB3 printf("%08X: unconditional jump to %08X\n", addr, branch_address(addr));
          addr = branch_address(addr);
          rom_flags_set(addr, flags_is_target);
          continue;
        } else { // conditional, follow both branches (finsih-thread-first recursion)
          unsigned int next = branch_address(addr);
          VERB3 printf("%08X: conditional jump to %08X, follow later\n", addr, next);
          check_code_coverage(addr+4, 0);
          VERB3 printf("%08X: following up on conditional jump to %08X\n", addr, next);
          addr = next; 
          rom_flags_set(addr, flags_is_target);
          continue;
        }
      } else if ( (cmd&0x0f000000) == 0x0b000000) { // branch instruction, follow both branches
        unsigned int next = branch_address(addr);
        VERB3 printf("%08X: subroutine call to %08X, follow later\n", addr, next);
        check_code_coverage(addr+4, 0);
        VERB3 printf("%08X: following up on subroutine call to %08X\n", addr, next);
        addr = next;
        rom_flags_set(addr, flags_is_target);
        continue;
      } else if ( (cmd&0x0db6f000) == 0x0120f000) { // msr command does not modifiy pc
      } else if ( (cmd&0x0c000000) == 0x00000000) { // data processing, only important if pc is changed
        if ( (cmd&0x0000f000) == 0x0000f000) { // is the destination register the pc?
          if ( (cmd&0xfffffff0) == 0x908FF100 && (rom_w(addr-4)&0xfff0f000) == 0xE3500000) {
            // cmp rx, #n; addls pc, pc, rx lsl 2
            // This is the pattern for a switch/case statement with default clause. A jump table of size n+1 follows.
            int n_case = (rom_w(addr-4)&0x00000fff)+1, i; // FIXME: is this right?
            VERB3 printf("Switch/Case statement with %d cases at %08X: %08X\n", n_case, addr, cmd);
            rom_add_comment(addr, "switch/case statement (0..%d)", n_case);
            addr+=4;
            for (i=0; i<=n_case; i++) { // nuber of cases plus default case
              if (i>0) {
                rom_add_comment(addr+4*i, "case %d (0x%02X):", i-1, i-1);
              } else {
                rom_add_comment(addr+4*i, "default:");
              }
              unsigned int cmd = rom_w(addr+4*i);
              if ((cmd&0x0f000000)==0x0a000000) {
                unsigned int dst = BDISP(cmd)*4+(addr+4*i)+8;
                if (i>0) {
                  rom_add_comment(dst, "switch at 0x%08X: case %d (0x%02X)", addr-4, i-1, i-1);
                } else {
                  rom_add_comment(dst, "switch at 0x%08X: default", addr-4);
                }
              }
              VERB4 printf("case %d at %08X\n", i-1, addr+4*i);
              check_code_coverage(addr+4*i, 0);
            }
            return;
          }
          unsigned int cmd1 = rom_w(addr-4);
          if ( (cmd1&0x0fffffff)==0x01A0E00F && (cmd&0xf0000000)==(cmd1&0xf0000000)) { // mov lr,pc; ...
            // The return address is written into the link register, so in all probability this is a function call
            VERB2 printf("Later: Register based call at %08X: %08X\n", addr, cmd);
            addr += 4; continue;
          }
          if (addr==0x0038d9a4) { check_switch_case(0x0038d9ac, 33); return; }
          if (addr==0x0038ec98) { check_switch_case(0x0038eca0,  9); return; }
          if (addr==0x00393384) { check_switch_case(0x0039338C,  4); return; }
          if ((cmd&0xf0000000)==0xe0000000) { // always
            VERB1 printf("Aborting: Data processing command modifying R15 at %08X: %08X\n", addr, cmd);
            ABORT_SCAN;
          } else {
            addr += 4; continue;
          }
        }          
      } else if ( (cmd&0x0f000000) == 0x0e000000) { // mcr, mrc (FIXME: probably not changing pc)
      } else if ( (cmd&0x0e000010) == 0x06000010) { // unknown (used to trigger interrupt, FIXME: and then?)
        int i;
        switch ((cmd&0x00ffff00)>>8) {
          case 0: // SystemBoot
          case 1: // ExitToShell
          case 2: // Debugger
          case 3: // DebugStr
          case 4: // PublicFiller
          case 7: // SendTestResults
          case 8: // TapFileCntl
            return;
          case 5: // SystemPanic
            for (i=addr+4; i<0x00800000; i++) {
              rom_flags_type(i, flags_type_arm_text);
              if (ROM[i]==0) break;
            }
            addr = (i+1+3)&0xfffffffc; // align i+1 to 4
            if (rom_w(addr)==0) return;
            continue;
        }
        VERB2 printf("Aborting: opcode 'undefined' found at %08X: %08X\n", addr, cmd);
        ABORT_SCAN;
      } else if ( (cmd&0x0c100000) == 0x04000000) { // str (store to memory)
      } else if ( (cmd&0x0c100000) == 0x04100000) { // ldr (load from memory)
        if ( (cmd&0x0000f000) == 0x0000f000) { // is the destination register the pc?
          unsigned int cmd1 = rom_w(addr-4);
          if ( (cmd1&0x0fffffff)==0x01A0E00F && (cmd&0xf0000000)==(cmd1&0xf0000000)) { // mov lr,pc; ...
            // The return address is writte into the link register, so in all probability this is a function call
            VERB2 printf("Later: Register based call at %08X: %08X\n", addr, cmd);
            addr += 4; continue;
          }
          if ((cmd&0xf0000000)==0xe0000000) { // always
            if (addr!=0x003AD74C)
            VERB1 printf("Aborting: LDR command modifying R15 at %08X: %08X\n", addr, cmd);
            ABORT_SCAN;
          } else {
            addr += 4; continue;
          }
        }
        tagOffset(addr, cmd, flags_is_target);
      } else if ( (cmd&0x0f000000) == 0x0f000000) { // swi (software interrupt)
      } else if ( (cmd&0x0e000000) == 0x0c000000) { // (coprocessor dat transfer) FIXME: may actuall tfer to pc?!
      } else if ( (cmd&0x0e100000) == 0x08000000) { // stm (store multiple to memory)
      } else if ( (cmd&0x0e100000) == 0x08100000) { // ldm (load from memory)
        if ( (cmd&0x00008000) == 0x00008000) { // is the pc among the destination registers?
                                               // we'll assume it's a return command
          if ( (cmd&0xf0000000)==0xe0000000) return; // unconditional - we are done
                                                     // conditional - continue to check
        }
      } else {
        VERB1 printf("Aborting: Hit unknown command at %08X: %08X\n", addr, cmd);
        ABORT_SCAN;
      }
    }
    addr += 4;
  }
}


void preset_rom_use()
{
  int i;
  for (i=0x00002000; i<0x0001285c; i++ ) {
    rom_flags_type(i, flags_type_patch_table);
  }
  for (i=0x00013000; i<0x00015e0c; i++ ) {
    rom_flags_type(i, flags_type_jump_table);
  }
  for (i=0x0071fc4c; i<0x007ec7fc; i++ ) {
    rom_flags_type(i, flags_type_rex);
  }
  for (i=0x007ec7fc; i<0x00800000; i++ ) {
    rom_flags_type(i, flags_type_unused);
  }
  for (i=0x006853dc; i<0x0071a95c; i++ ) {
    rom_flags_type(i, flags_type_dict);
  }
}


void check_classinfo(unsigned int addr)
{
  unsigned int i;
  if (rom_w(addr)!=0xE24F0044) {
    printf("ClassInfo: unsupported offset at %08X: %08X\n", addr, rom_w(addr));
    return;
  }
  unsigned int base = addr - 0x44 + 8;
  unsigned int class_name = rom_w(base+4)+base+4;
  unsigned int base_class = rom_w(base+8)+base+8;
  printf("class \"%s\" is derived from \"%s\"\n", ROM+class_name, ROM+base_class);
  //rom_flags_type(class_name, flags_type_arm_text);
  //rom_flags_type(base_class, flags_type_arm_text);
  unsigned int vtbl_start = rom_w(base+16)+base+16;
  unsigned int vtbl_end   = rom_w(base+20)+base+20;
  check_code_coverage(addr+8, flags_is_function);
  for (i=vtbl_start; i<vtbl_end; i+=4) {
    if (rom_w(i)) check_code_coverage(i, flags_is_function);
  }
  if (rom_w(base+24)) check_code_coverage(base+24, flags_is_function); // sizeof
  if (rom_w(base+28)) check_code_coverage(base+28, flags_is_function); // ??
  if (rom_w(base+32)) check_code_coverage(base+32, flags_is_function); // ??
  if (rom_w(base+36)) check_code_coverage(base+36, flags_is_function); // ctor
  if (rom_w(base+40)) check_code_coverage(base+40, flags_is_function); // dtor
  if (rom_w(base+56)) check_code_coverage(base+56, flags_is_function); // ClassInfo
  for (i=base; i<vtbl_end; i+=4) {
    if (!rom_flags_type(i)) rom_flags_type(i, flags_type_classinfo);
  }
  return;
}

static unsigned int classinfo[] = {
#include "classinfo.h"
};

void check_all_classinfos()
{
  int i;
  for (i=0; i<sizeof(classinfo)/sizeof(unsigned int); i++) {
    check_classinfo(classinfo[i]);
  }
}


static unsigned int db_cpp[] = {
#include "db_cpp.h"
};

void check_all_code_coverage()
{
  int i;
  // all ::CLassInfo functions and structures
  check_all_classinfos();
  // system vectors
  check_code_coverage(0x00000000, flags_is_function);
  check_code_coverage(0x00000004, flags_is_function);
  check_code_coverage(0x00000008, flags_is_function);
  check_code_coverage(0x0000000c, flags_is_function);
  check_code_coverage(0x00000010, flags_is_function);
  check_code_coverage(0x00000014, flags_is_function);
  check_code_coverage(0x00000018, flags_is_function);
  check_code_coverage(0x0000001c, flags_is_function);
  // known entry points (C++ functions)
  for (i=0; i<sizeof(db_cpp)/sizeof(unsigned int); i++) {
    check_code_coverage(db_cpp[i], flags_is_function);
  }
  // ROMBoot unclear positions
  check_code_coverage(0x000188d0, 0);
  // ROMPublicJumpTable
  for (i=0x00013000; i<0x00015e0c; i+=4 ) {
    check_code_coverage(i, flags_is_function);
  }  
  // some jump vector table we found. These jumps all go into the ROM patch table
  for (i=0x0001a618; i<0x00021438; i+=4 ) {
    check_code_coverage(i, 0);
  }  
  // some places that are not reached via static analysis  
#include "manual_checks.h"
}


/*
 3a.f000: Magic Pointer Table (873 entries)
   num_entries
   pointer to entry[0]  NSRef: Pointer
   ...
 
 67.fa40: Binary Objects
 71.fc4c: ROM code end, beginning of REX (ROM Extension, may contain code)
 */

unsigned int check_ns_obj(unsigned int addr);

void ns_print_binary(unsigned int addr, unsigned int size) 
{
  int i;
  switch (rom_w(addr+8)) {
    case 0x00055552:
      printf(":  Symbol '%s\n", ROM+addr+16);
      break;
    case 0x003c13a5:
      printf(":  'string \"");
      for (i=13; i<size; i+=2) putchar(ROM[addr+i]);
      printf("\"\n");
      i=0;
      break;
  }
}


void check_ns_ref(unsigned int addr, char target=1)
{
  if (rom_flags_type(addr)) return;
  rom_flags_type(addr, flags_type_ns_ref);
  if (target) rom_flags_set(addr, flags_is_target);
  unsigned int val = rom_w(addr);
  if ( (val&0x00000003) == 0x00000000 ) { // Integer
    int v = (int)val;
    VERB3 printf("NSRef: Integer %d at %08X: %08X\n", v/4, addr, val);
    // top 30 bit are a (small) integer
  } else if ( (val&0x00000003) == 0x00000001 ) { // Pointer
    VERB3 printf("NSRef: Pointer at %08X: %08X\n", addr, val);
    check_ns_obj(val&0xfffffffc);
  } else if ( (val&0x00000003) == 0x00000002 ) { // Special
    // TODO: ...
    if (val==0x1a) {
      VERB3 printf("NSRef: TRUE %08X\n", addr);
    } else if (val==0x02) {
      VERB3 printf("NSRef: NIL at %08X\n", addr);
    } else if (val==0x00055552) {
      VERB3 printf("NSRef: symbol definition at %08X\n", addr);
    } else if (val==0x32) {
      VERB3 printf("NSRef: native function (?) at %08X\n", addr);
    } else if ((val&0xfff0000f) == 0x0000000a ) { // Character
      VERB1 printf("NSRef: Unicode Char 0x%04x at %08X\n", val, addr);
    } else {
      VERB3 printf("NSRef: unknown special at %08X: %08X\n", addr, val);
      ABORT_SCAN;
    }
  } else if ( (val&0x00000003) == 0x00000003 ) { // Magic Pointer
    // no need to follow, we have all magic pointers covered
    VERB3 printf("NSRef: Magic %d:%d at %08X: %08X\n", (val&0xffffc000)>>14, (val&0x00003ffc)>>2, addr, val);
  }
}

unsigned int check_ns_obj(unsigned int addr)
{
  unsigned int val = rom_w(addr);
  unsigned int flags = rom_w(addr+4);
  
  if ( (val&0x0000007c) != 0x00000040 || flags!=0 ) {
    VERB1 printf("int: not an NS object at %08X: %08X\n", addr, val);
    ABORT_SCAN_0;
  }
  unsigned int i, size = (val&0xffffff00)>>8;
  if (rom_flags_type(addr)) return size;
  rom_flags_type(addr, flags_type_ns_obj);
  rom_flags_type(addr+4, flags_type_ns);
  rom_flags_set(addr, flags_is_target);
                   
  // follow the members of the object
  if ( (val&0x00000003) == 0x00000000 ) {
    // binary object
    VERB3 printf("NS Binary Object at %08X (%d bytes):\n", addr, size);
    check_ns_ref(addr+8, 0);
    // mark the binary part as checked
    for (i=12; i<size; i+=4) rom_flags_type(addr+i, flags_type_ns);
    VERB4 ns_print_binary(addr, size);
    // followed by binary data and optional fill bytes
  } else if ( (val&0x00000003) == 0x00000001 ) {
    // array
    VERB3 printf("NS Array at %08X (%d bytes = %d entries):\n", addr, size, size/4-3);
    VERB3 printf("class: %08X\n", rom_w(addr+8));
    for (i=12; i<size; i+=4) {
      VERB3 printf("%5d: %08X\n", i/4-3, rom_w(addr+i));
    }
    for (i=8; i<size; i+=4) {
      check_ns_ref(addr+i, 0);
    }
  } else if ( (val&0x00000003) == 0x00000003 ) {
    // frame
    VERB3 printf("NS Frame at %08X (%d bytes = %d entries):\n", addr, size, size/4-3);
    VERB3 printf("  map: %08X\n", rom_w(addr+8));
    for (i=12; i<size; i+=4) {
      VERB3 printf("%5d: %08X\n", i/4-3, rom_w(addr+i));
    }
    for (i=8; i<size; i+=4) {
      check_ns_ref(addr+i, 0);
    }
  } else {
    VERB1 printf("ERROR: unsupported NS object at %08X: %08X\n", addr, val);
    ABORT_SCAN_0;
  }
  return size;
}

void check_all_ns_coverage()
{
  int i;
  // 0x0068068C
  
  // array of asm/NS pointers. Why are they ordered like this?
  // these are always two words: 
  //   the first word, Rlabel, is a NSRef
  //   the second word, RSlabel, is always an asm pointer to the previous word
  for (i=0x0067fa44; i<0x00681c9c; i+=8) {
    check_ns_ref(i);
  } 
  // simple pointers, but what are they used for?
  for (i=0x00681c9c; i<0x006853dc; i+=8) {
    check_ns_ref(i);
  }

  int n_magic = rom_w(0x003af000);  
  for (i=0; i<n_magic; i++) {
    check_ns_ref(0x003af004+4*i, 0);
  }
  // 0x003afda8 gROMSoupData to 0x0067FA40 gROMSoupDataSize
  for (i=0x003afda8; i<0x0067fa40; ) {
    unsigned int size = check_ns_obj(i);
    if (size>0) {
      i+=(size+3)&0xfffffffc; // align to four bytes
    } else {
      VERB1 printf("ERROR: lost track of NS objects at %08X!\n", i);
      ABORT_SCAN;
    }
  }
  
}

// Dictionaries starting at 0x006853DC (InitROMDictionaryData)


void writeNewtonROMTexts()
{  
  printf("\n====> Writing all Newton ROM ASCII text entries\n\n");
  unsigned int i;
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "data/text.txt");
  FILE *text = fopen(filename_buffer, "wb");
  if (!text) {
    puts("Can't write text!");
  } else {
    int j, n = 0;
    for (i=0; i<0x00800000; i+=4) {
      if (rom_flags_type(i)==flags_type_arm_text) {
        for (j=0; j<4; j++) {
          if (n==0) {
            fprintf(text, "0x%08X: \"", i+j);
            n = 1;
          }
          char c = ROM[i+j];
          if (c==0) {
            fprintf(text, "\\0\"\n");
            n = 0;
          } else if (c<' ') {
            fprintf(text, "\\%c", c+96); // FIXME: lookup table needed
          } else if (c>=127) {
            fprintf(text, "\\x%2x", c);            
          } else {
            fputc(c, text);
          }        
        }
      }
    }
    fclose(text);
  }
}

char hasLabel(unsigned int i)
{
  if (rom_flags_is_set(i, flags_is_target)) {
    return 1;
  }  
  if (get_plain_symbol_at(i)) {
    return 1;
  }
  if (get_symbol_at(i)) {
    return 1;
  }
  return 0;
}

void writeLabel(FILE *newt, unsigned int i)
{
  const char *sym = get_symbol_at(i);
  const char *psym = get_plain_symbol_at(i);
  if (psym && sym) {
    AsmPrintf(newt, "\n");
    AsmPrintf(newt, "%s:\t@ 0x%08X: %s\n", psym, i, sym);
  } else if (psym) {
    AsmPrintf(newt, "\n");
    AsmPrintf(newt, "%s:\n", psym);
  } else if (sym) {
    AsmPrintf(newt, "L%08X:\t@ %s\n", i, sym);
  } else if (rom_flags_is_set(i, flags_is_target)) {
    AsmPrintf(newt, "L%08X:\n", i);
  }
}

void writeLabel_c(FILE *newt, unsigned int i)
{
  const char *sym = get_symbol_at(i);
  const char *psym = get_plain_symbol_at(i);
  if (psym && sym) {
    AsmPrintf(newt, "\n");
    AsmPrintf(newt, "a_return_t %s // @ 0x%08X: %s\n", sym, i, psym);
    AsmPrintf(newt, "{\n");
  } else if (psym) {
    AsmPrintf(newt, "\n");
    AsmPrintf(newt, "a_return_t %s(a_reglist_t) // @ 0x%08X\n", psym, i);
    AsmPrintf(newt, "{\n");
  } else if (sym) {
    AsmPrintf(newt, "L%08X:\t@ %s\n", i, sym);
  } else if (rom_flags_is_set(i, flags_is_target)) {
    AsmPrintf(newt, "L%08X:\n", i);
  }
}

void writeLabelGlobal(FILE *newt, unsigned int i)
{
  const char *sym = get_symbol_at(i);
  const char *psym = get_plain_symbol_at(i);
  if (psym && sym) {
    AsmPrintf(newt, "\t.global\t%s\n", psym);
  } else if (psym) {
    AsmPrintf(newt, "\t.global\t%s\n", psym);
  }
}

/*
 Write a numeric label, only if the function above did not write a label yet.
 */
void writeLabelIfNone(FILE *newt, unsigned int i)
{
  const char *sym = get_symbol_at(i);
  const char *psym = get_plain_symbol_at(i);
  if (psym || sym) {
  } else if (rom_flags_is_set(i, flags_is_target)) {
  } else {
    AsmPrintf(newt, "L%08X:\n", i);
  }
}

const char *getLabelAlways(unsigned int i)
{
  static char buf[32];
  const char *sym = get_symbol_at(i);
  if (sym) return sym;
  
  const char *psym = get_plain_symbol_at(i);
  if (psym) return psym;

  if (rom_flags_is_set(i, flags_is_target)) {
    sprintf(buf, "L%08X", i);
    return buf;
  }

  sprintf(buf, "G%08X", i);
  return buf;
}


void writeComments(FILE* newt, unsigned int i)
{
  AlROMCommentIterator it;
  AlROMCommentIteratorPair its = ROMComment.equal_range(i);
  for (it = its.first; it!=its.second; ++it) {
    const char *rem = (*it).second;
    if (rem)
      fprintf(newt, "\t@; %s\n", rem);
  }
}

void writeComments_c(FILE* newt, unsigned int i)
{
  AlROMCommentIterator it;
  AlROMCommentIteratorPair its = ROMComment.equal_range(i);
  for (it = its.first; it!=its.second; ++it) {
    const char *rem = (*it).second;
    if (rem)
      fprintf(newt, "\t// %s\n", rem);
  }
}

void writeNorcroftROM()
{
  int i, j;
  const int parts = 16;
  const int partSize = (8*1024*1024)/parts;
  for (i=0; i<parts; i++) {
    char buf[4096];
    sprintf(buf, "%sNewtonOS/nos_p%02d.a", base_path, i);
    FILE *newt = fopen(buf, "wb");
    fprintf(newt, "\n\tAREA\tnewtonos,CODE,READONLY\n\n");
    if (i==0)
      fprintf(newt, "\n\tENTRY\n\nReset\n");
    for (j=0; j<partSize; j+=4) {
      fprintf(newt, "\tDCD\t0x%08X\n", rom_w(j+i*partSize));
    }
    fprintf(newt, "\n\tEND\n\n");
    fclose(newt);
  }
}

void writeNewtonROMGlobals()
{
  printf("\n====> Writing Newton ROM globals in ARM assembler code\n\n");
  unsigned int i;
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "NewtonOS/globals.s");
  FILE *newt = fopen(filename_buffer, "wb");
  if (!newt) {
    perror("Can't write NewtonOS!");
  } else {
    // write .global statements for labels in ROM
    for (i=0; i<0x00800000; i+=4) {
      writeLabelGlobal(newt, i);
    }
    // write .global statements for symbols that are not within the ROM area, but are called from ROM
    fprintf(newt, "\n\n@\n@ Symbols outside of the ROM\n@\n\n");
    AlMemoryMapIterator it(gMemoryMap);
    while (!it.end()) {
      unsigned int addr = it.address();
      if (addr>=0x00800000) { // beyond ROM
        AlData *s = it.data();
        const char *sym = s->label();
        if (sym) {
          fprintf(newt, "\t.equ\tVEC_%s, _start+0x%08X\n", sym, addr);
//          fprintf(newt, "\t.global\tVEC_%s\n", sym);
        }
      }
      it.incr();
    }
    fclose(newt);
  }
  printf("\n====> DONE\n\n");
}


void writeNewtonROM()
{
  printf("\n====> Writing Newton ROM in ARM assembler code\n\n");
  unsigned int i;
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "NewtonOS/newtonos.s");
  FILE *newt = fopen(filename_buffer, "wb");
  if (!newt) {
    perror("Can't write NewtonOS!");
  } else {
    
    rom_flags_type(0x003AE204, flags_type_arm_word); // will create illegal instructions!
    rom_flags_type(0x003AE20C, flags_type_arm_word);
    
    unsigned int n_unused = 0, unused_filler = 0;
    fprintf(newt, "\n\t.include\t\"macros.s\"\n\n\t.text\n\t.org\t0\n\n");
    fprintf(newt, "\t.global\t_start\n_start:\n");
    for (i=0; i<0x00800000; i+=4) {
      unsigned int val = rom_w(i);
      writeLabel(newt, i);
      writeComments(newt, i);
      if (rom_flags_is_set(i, flags_include)) {
        const char *sym = 0L;
        sym = get_plain_symbol_at(i);
        if (sym) AsmPrintf(newt, "\t.include \"%s.s\"\n", sym);
      }
      switch (rom_flags_type(i)) {
        case flags_type_unused:
          if (!n_unused) unused_filler = rom_w(i);
          n_unused++;
          if (  (i+4)>=0x00800000
              || rom_flags_type(i+4)!=flags_type_unused
              || rom_w(i+4)!=unused_filler) {
            AsmPrintf(newt, "\t.fill\t%d, %d, 0x%08X\n", n_unused, 4, unused_filler);
            n_unused = 0;
          }
          break;
        case flags_type_ns_ref:
          i = decodeNSRef(newt, i);
          break;
        case flags_type_ns_obj:
          i = decodeNSObj(newt, i);
          break;
        case flags_type_arm_code: {
          AlData *d = gMemoryMap.find(i);
          if (d) d->exportAsm(newt);
          char buf[4096];
          disarm(buf, i, rom_w(i));
          char *cmt = strchr(buf, ';');
          if (cmt) *cmt = '@';
          AsmPrintf(newt, "\t%s\n", buf);
          break; }
          // mcr p15, 0, r8, c1, c0, 2
          // mrc p15, 0, r0, c1, c0, 0
        case flags_type_arm_text: {
          int n = 0;
          writeLabelIfNone(newt, i);
          AsmPrintf(newt, "\t.ascii\t\"");
          i -= 4;
          do {
            i += 4; n++;
            AsmPrintf(newt, "%s", p_ascii(ROM[i]));
            AsmPrintf(newt, "%s", p_ascii(ROM[i+1]));
            AsmPrintf(newt, "%s", p_ascii(ROM[i+2]));
            AsmPrintf(newt, "%s", p_ascii(ROM[i+3]));
          } while (
                   rom_flags_type(i+4)==flags_type_arm_text
                   && ROM[i] && ROM[i+1] && ROM[i+2] && ROM[i+3]
                   && !hasLabel(i+4)
                   && n<16);
          AsmPrintf(newt, "\"\n");
          break; }
        case flags_type_patch_table:
        case flags_type_jump_table: // TODO: these are jump tables! Find out how to calculate the offsets!
        case flags_type_arm_byte:   // TODO: currently not used
        case flags_type_rex:        // TODO: interprete the contents
        case flags_type_ns:
        case flags_type_dict:
        case flags_type_classinfo:  // TODO: differentiate this
          AsmPrintf(newt, "\t.word\t0x%08X\t@ 0x%08X \"%c%c%c%c\" %d (%s)\n", val, i,
                    printable(ROM[i]), printable(ROM[i+1]), printable(ROM[i+2]), printable(ROM[i+3]),
                    rom_w(i), type_lut[rom_flags_type(i)]);
          break;
        case flags_type_data:
        case flags_type_unknown:
        case flags_type_arm_word:
        default:
        {
          // if it matches a label, is it a pointer?
          // if all bytes are ASCII, is it an ID?
          // if it is a negative number, is it an error message?
          const char *sym = 0L;
          if (val) sym = get_plain_symbol_at(val);
          if (!sym) sym = "";
          AsmPrintf(newt, "\t.word\t0x%08X\t@ 0x%08X \"%c%c%c%c\" %d (%s) %s?\n", val, i,
                    printable(ROM[i]), printable(ROM[i+1]), printable(ROM[i+2]), printable(ROM[i+3]),
                    rom_w(i), type_lut[rom_flags_type(i)], sym);
        }
          break;
      }
    }
    // write all symbols that are not within the ROM area, but are called from ROM
    fprintf(newt, "\n\n@\n@ Symbols outside of the ROM\n@\n\n");
    AlMemoryMapIterator it(gMemoryMap);
    while (!it.end()) {
      unsigned int addr = it.address();
      if (addr>=0x00800000) { // beyond ROM
        AlData *s = it.data();
        const char *sym = s->label();
        if (sym) {
          /* FIXME: cpp support
           const char *cppsym = s->decodedName();
           if (cppsym)
           fprintf(newt, "\t.equ\tVEC_%s, _start+0x%08X\t@ %s\n", sym, addr, cppsym);
           else
           fprintf(newt, "\t.equ\tVEC_%s, _start+0x%08X\n", sym, addr);
           */
          fprintf(newt, "\t.equ\tVEC_%s, _start+0x%08X\n", sym, addr);
        }
      }
      it.incr();
    }
    
    fprintf(newt, "\n\t.end\n\n");
    fclose(newt);
  }
  printf("\n====> DONE\n\n");
}


void writeNewtonROM_C()
{
  printf("\n====> Writing Newton ROM in PseudoC code\n\n");
  unsigned int i;
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "NewtonOS/newtonos.cpp");
  FILE *newt = fopen(filename_buffer, "wb");
  if (!newt) {
    perror("Can't write NewtonOS!");
  } else {
    
    rom_flags_type(0x003AE204, flags_type_arm_word); // will create illegal instructions!
    rom_flags_type(0x003AE20C, flags_type_arm_word);
    
    unsigned int n_unused = 0, unused_filler = 0;
    char isFunction = 0, wasFunction = 0;
    for (i=0; i<0x00800000; i+=4) {
      char labelDone = 0;
      unsigned int val = rom_w(i);
      if (get_plain_symbol_at(i) || get_symbol_at(i)) {
        wasFunction = isFunction;
        isFunction = (rom_flags_type(i)==flags_type_arm_code);
        if (wasFunction) {
          AsmPrintf(newt, "}\n");
          AsmPrintf(newt, "\n");
          wasFunction = 0;
        }
        if (isFunction) {
          writeLabel_c(newt, i);
          labelDone = 1;
        }
      }
      writeComments_c(newt, i);
      switch (rom_flags_type(i)) {
        case flags_type_unused:
          if (!labelDone) { writeLabel(newt, i); labelDone = 1; }
          if (!n_unused) unused_filler = rom_w(i);
          n_unused++;
          if (  (i+4)>=0x00800000
              || rom_flags_type(i+4)!=flags_type_unused
              || rom_w(i+4)!=unused_filler) {
            AsmPrintf(newt, "\tA_FILL(%d, %d, 0x%08X);\n", n_unused, 4, unused_filler);
            n_unused = 0;
          }
          break;
        case flags_type_ns_ref:
          if (!labelDone) { writeLabel(newt, i); labelDone = 1; }
          i = decodeNSRef(newt, i);
          break;
        case flags_type_ns_obj:
          if (!labelDone) { writeLabel(newt, i); labelDone = 1; }
          i = decodeNSObj(newt, i);
          break;
        case flags_type_arm_code: {
          if (!labelDone) { writeLabel(newt, i); labelDone = 1; }
          char buf[4096];
          disarm_c(buf, i, rom_w(i));
          AsmPrintf(newt, "\t%s\n", buf);
          break; }
        case flags_type_arm_text: {
          int n = 0;
          AsmPrintf(newt, "\tstatic char *%s = \"", getLabelAlways(i));
          i -= 4;
          do {
            i += 4; n++;
            AsmPrintf(newt, "%s", p_ascii(ROM[i]));
            AsmPrintf(newt, "%s", p_ascii(ROM[i+1]));
            AsmPrintf(newt, "%s", p_ascii(ROM[i+2]));
            AsmPrintf(newt, "%s", p_ascii(ROM[i+3]));
          } while (
                   rom_flags_type(i+4)==flags_type_arm_text
                   && ROM[i] && ROM[i+1] && ROM[i+2] && ROM[i+3]
                   && !hasLabel(i+4)
                   && n<16);
          AsmPrintf(newt, "\";\n");
          break; }
        case flags_type_patch_table:
        case flags_type_jump_table: // TODO: these are jump tables! Find out how to calculate the offsets!
        case flags_type_arm_byte:   // TODO: currently not used
        case flags_type_rex:        // TODO: interprete the contents
        case flags_type_ns:
        case flags_type_dict:
        case flags_type_classinfo:  // TODO: differentiate this
          AsmPrintf(newt, "\tstatic unsigned int %s = 0x%08X;\t// 0x%08X \"%c%c%c%c\" %d (%s)\n",
                    getLabelAlways(i), val, i,
                    printable(ROM[i]), printable(ROM[i+1]), printable(ROM[i+2]), printable(ROM[i+3]),
                    rom_w(i), type_lut[rom_flags_type(i)]);
          break;
        case flags_type_data:
        case flags_type_unknown:
        case flags_type_arm_word:
        default:
        {
          // if it matches a label, is it a pointer?
          // if all bytes are ASCII, is it an ID?
          // if it is a negative number, is it an error message?
          const char *sym = 0L;
          if (val) sym = get_symbol_at(val);
          if (!sym) sym = "";
          AsmPrintf(newt, "\tstatic unsigned int %s = 0x%08X;\t// 0x%08X \"%c%c%c%c\" %d (%s) %s?\n",
                    getLabelAlways(i), val, i,
                    printable(ROM[i]), printable(ROM[i+1]), printable(ROM[i+2]), printable(ROM[i+3]),
                    rom_w(i), type_lut[rom_flags_type(i)], sym);
        }
          break;
      }
    }
    // write all symbols that are not within the ROM area, but are called from ROM
    fprintf(newt, "\n\n//\n// Symbols outside of the ROM\n//\n\n");
    AlMemoryMapIterator it(gMemoryMap);
    while (!it.end()) {
      unsigned int addr = it.address();
      if (addr>=0x00800000) { // beyond ROM
        AlData *s = it.data();
        const char *sym = s->label();
        if (sym) {
          fprintf(newt, "a_unknown_t %s; // @ 0x%08X\n", sym, addr);
        }
      }
      it.incr();
    }
    
    fprintf(newt, "\n\t.end\n\n");
    fclose(newt);
  }
  printf("\n====> DONE\n\n");
}


void writeNewtonPseudoC()
{
  unsigned int startAt = 0x002F443C;
  unsigned int endAt = startAt+4;
  for (;endAt<0x08000000; endAt+=4) {
    if (get_plain_symbol_at(endAt)) break;
  }

  printf("\n====> Writing Newton ROM in PseudoC code\n\n");
  unsigned int i;
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "NewtonOS/newtonos.cxx");
  FILE *newt = fopen(filename_buffer, "wb");
  if (!newt) {
    perror("Can't write NewtonOS!");
  } else {
    
    rom_flags_type(0x003AE204, flags_type_arm_word); // will create illegal instructions!
    rom_flags_type(0x003AE20C, flags_type_arm_word);
    
    AlData *fn = 0;
    unsigned int n_unused = 0, unused_filler = 0;
//    fprintf(newt, "//\n// Auto Generated by Albert\n//\n\n");
    for (i=startAt; i<endAt; i+=4) {
      unsigned int val = rom_w(i);
      writeLabel(newt, i);
      writeComments(newt, i);
      switch (rom_flags_type(i)) {
        case flags_type_unused:
          if (!n_unused) unused_filler = rom_w(i);
          n_unused++;
          if (  (i+4)>=0x00800000
              || rom_flags_type(i+4)!=flags_type_unused
              || rom_w(i+4)!=unused_filler) {
            AsmPrintf(newt, "\t.fill\t%d, %d, 0x%08X\n", n_unused, 4, unused_filler);
            n_unused = 0;
          }
          break;
        case flags_type_ns_ref:
          i = decodeNSRef(newt, i);
          break;
        case flags_type_ns_obj:
          i = decodeNSObj(newt, i);
          break;
        case flags_type_arm_code: {
          AlData *d = gMemoryMap.find(i);
          if (dynamic_cast<AlCMethod*>(d)) {
            if (fn)
              fn->exportCppCallEnd(newt);
            fn = d;
          }
          if (d) d->exportCppCall(newt);
          char buf[16384];
          disarm(buf, i, rom_w(i));
          char *cmt = strchr(buf, ';');
          if (cmt) *cmt = '@';
          AsmPrintf(newt, "//\t%s\n", buf);
          
          disarm_c(buf, i, rom_w(i));
          AsmPrintf(newt, "%s\n", buf);
          
          break; }
        case flags_type_arm_text: {
          int n = 0;
          writeLabelIfNone(newt, i);
          AsmPrintf(newt, "\t.ascii\t\"");
          i -= 4;
          do {
            i += 4; n++;
            AsmPrintf(newt, "%s", p_ascii(ROM[i]));
            AsmPrintf(newt, "%s", p_ascii(ROM[i+1]));
            AsmPrintf(newt, "%s", p_ascii(ROM[i+2]));
            AsmPrintf(newt, "%s", p_ascii(ROM[i+3]));
          } while (
                   rom_flags_type(i+4)==flags_type_arm_text
                   && ROM[i] && ROM[i+1] && ROM[i+2] && ROM[i+3]
                   && !hasLabel(i+4)
                   && n<16);
          AsmPrintf(newt, "\"\n");
          break; }
        case flags_type_patch_table:
        case flags_type_jump_table: // TODO: these are jump tables! Find out how to calculate the offsets!
        case flags_type_arm_byte:   // TODO: currently not used
        case flags_type_rex:        // TODO: interprete the contents
        case flags_type_ns:
        case flags_type_dict:
        case flags_type_classinfo:  // TODO: differentiate this
          AsmPrintf(newt, "\t.word\t0x%08X\t@ 0x%08X \"%c%c%c%c\" %d (%s)\n", val, i,
                    printable(ROM[i]), printable(ROM[i+1]), printable(ROM[i+2]), printable(ROM[i+3]),
                    rom_w(i), type_lut[rom_flags_type(i)]);
          break;
        case flags_type_data:
        case flags_type_unknown:
        case flags_type_arm_word:
        default:
        {
          // if it matches a label, is it a pointer?
          // if all bytes are ASCII, is it an ID?
          // if it is a negative number, is it an error message?
          const char *sym = 0L;
          if (val) sym = get_symbol_at(val);
          if (!sym) sym = "";
          AsmPrintf(newt, "\t.word\t0x%08X\t@ 0x%08X \"%c%c%c%c\" %d (%s) %s?\n", val, i,
                    printable(ROM[i]), printable(ROM[i+1]), printable(ROM[i+2]), printable(ROM[i+3]),
                    rom_w(i), type_lut[rom_flags_type(i)], sym);
        }
          break;
      }
    }
    if (fn)
      fn->exportCppCallEnd(newt);
    fclose(newt);
  }
}


unsigned int get_op2(unsigned int given) {
  if ((given & 0x02000000) != 0)
  {
    int rotate = (given & 0xf00) >> 7;
    int immed = (given & 0xff);
    return ((immed << (32 - rotate)) | (immed >> rotate)) & 0xffffffff;
  }
  else
    printf("Decoder not yet implemented!\n");
  //arm_decode_shift(given, str);
  return 0;
}

unsigned int get_r0(unsigned int addr, char *err=0L) {
  unsigned int cmd = rom_w(addr);
  if (err) *err = 0;
  if ((cmd&0xfdfff000)==0xE1A00000) {
    unsigned int size = get_op2(cmd);
    return size;
  } else if ((cmd&0xfdfff000)==0xE0800000) {
    unsigned int size = get_op2(cmd);
    cmd = rom_w(addr-4);
    if ((cmd&0xfdfff000)==0xE1A00000) {
      size += get_op2(cmd);
      return size;
    }
  }
  if (err) *err = 1;
  return 0;
}

void analyse_Sizeof(const char *name, unsigned int addr, const char *sym) {
  int i;
  // find the "return" sequence which may tell us the class size
  for (i=4; i<12; i+=4) {
    unsigned int cmd = rom_w(addr+i);
    if (cmd==0xE1A0F00E) { // mov pc,lr
      printf("    return\n");
      char err;
      unsigned int r0 = get_r0(addr+i-4, &err);
      if (!err) {
        printf("    Size is %d bytes\n", r0);
      } else {
        char buf[200];
        disarm(buf, addr+i-4, rom_w(addr+i-4));
        printf("    %s\n", buf);
      }
      return;
    }
  }
}

void analyse_ClassInfo(const char *name, unsigned int addr, const char *sym) {
  // start of Class Info struct:
  //  sub     r0, pc, #68                     @ 0x00382C4C 0xE24F0044 - .O.D 
  // word 1: 0
  // word 2: offset from here to classname
  // word 3: offset from here to superclass name
  // word 4: offset ?? 
  // word 5: offset ??
  // call to Sizeof()
}

void analyse_ctor(const char *name, unsigned int addr, const char *sym) {
  int i;
  // find the "operator new" sequence which may tell us the class size
  for (i=24; i<40; i+=4) {
    unsigned int cmd = rom_w(addr+i);
    if ((cmd & 0xff000000) == 0xeb000000) { // it is a funciton call
      unsigned int dst = branch_address(addr+i, cmd);
      if (dst==0x01BCE738) { 
        printf("    operator new\n");
        char err;
        unsigned int r0 = get_r0(addr+i-4, &err);
        if (!err) {
          printf("    Size is %d bytes\n", r0);
        } else {
          char buf[200];
          disarm(buf, addr+i-4, rom_w(addr+i-4));
          printf("    %s\n", buf);
        }
      }
      break;
    }
  }
  // find out if we are derived from another class
  for (i=36; i<48; i+=4) {
    if (rom_w(addr+i)==0xE1A00004) {
      if ((rom_w(addr+i+4)&0xff000000)==0xEB000000) {
        unsigned int d = branch_address(addr+i+4);
        const char *supersym = get_plain_symbol_at(d);
        if (strncmp(supersym, "__ct__", 6)==0 || strncmp(supersym, "VEC___ct__", 10)==0) {
          supersym = get_symbol_at(d);
          printf("    ctor %s calls ctor %s for base address 0, so it is probably derived from it\n", sym, supersym);
        }
      }
      break;
    }
  }
}

void find_all_methods(const char *name) {
  char buf[100];
  sprintf(buf, "%s::", name);
  int len = strlen(buf);
  AlMemoryMapIterator it(gMemoryMap);
  while (!it.end()) {
    AlData *s = it.data();
    unsigned int addr = it.address();
    const char *sym = s->label(); // FIXME: CPP support!
    if (sym && addr<0x00800000 && strncmp(sym, buf, len)==0) {
      if (strncmp(sym+len, name, len-2)==0) {
        //printf("  Constructor found at 0x%08X: %s\n", addr, sym);
        analyse_ctor(name, addr, sym);
      } else if (sym[len]=='~' && strncmp(sym+len+1, name, len-2)==0) {
        // printf("  Destructor found at 0x%08X: %s\n", addr, sym);
      } else if (strncmp(sym+len, "Sizeof(", 7)==0) {
        // This is likely a PClass. Get the size of this class:
        printf("  Method found at 0x%08X: %s\n", addr, sym);
        analyse_Sizeof(name, addr, sym);
      } else if (strncmp(sym+len, "ClassInfo(", 10)==0) {
        // This is a PClass. Get more information
        //printf("  PClass method found at 0x%08X: %s\n", addr, sym);
        //analyse_ClassInfo(name, addr, sym);
      } else {
        // printf("  Method found at 0x%08X: %s\n", addr, sym);
        // We can also find the SizeOf function for P-Classes
      }
    }
    it.incr();
  }
}

void find_class_info(const char *name) {
  //ClassInfo__16TQDLibraryDriverSFv:               @ 0x003887C8: TQDLibraryDriver::ClassInfo(void) static
  //TKeyboardService
}

void analyse_class(const char *name) {
  printf("Analysing class %s\n", name);
  find_all_methods(name);
  find_class_info(name);
}


void analyse_all_classes() {
  char **prev_class = (char**)calloc(1000, sizeof(char*));
  int i, n = 0;
  AlMemoryMapIterator it(gMemoryMap);
  while (!it.end()) {
    AlData *s = it.data();
    const char *sym = s->label(); // FIXME: cpp function!
    const char *dd = strstr(sym, "::");
    if (dd) {
      char *name = strdup(sym);
      name[dd-sym] = 0;
      for (i=0; i<n; i++) {
        if (strcmp(name, prev_class[i])==0) 
          break;
      }
      if (i==n) {
        prev_class[n++] = strdup(name);
        analyse_class(name);
      }
      free(name);
    }
    it.incr();
  }
}


void find_virtual_methods() {
  unsigned int i;
  unsigned int vtables = 0x0001A618;
  unsigned int vtend = 0x00021438;
  for (i=vtables; i<vtend; i+=4) {
    unsigned int d = branch_address(i);
    const char *sym = get_symbol_at(d);
    if (sym) {
      printf("%s is virtual\n", sym);
    } else {
      sym = get_plain_symbol_at(d);
      if (sym) {
        printf("%s is virtual\n", sym);
      } else {
        printf("ERROR: can't find symbol for 0x%08X at 0x%08X\n", d, i);
      }
    }
  }
}


void writeRexBlock(FILE *f, unsigned int addr, unsigned int length) {

  if (length>1024) length = 1024; // FIXME
  
  for ( ; length>0; length-=4, addr+=4) {
    char buf[4096];
    memset(buf, 255, 0);
    disarm(buf, addr, rom_w(addr));
    puts(buf);
  }
}

void writeRExConfigBlock(FILE *f, unsigned int addr) {
  printf("\nRExBlock Entry ('%c%c%c%c', %d bytes at 0x%08X):\n", ROM[addr], ROM[addr+1], ROM[addr+2], ROM[addr+3], rom_w(addr+8), addr+rom_w(addr+4));
  const char *id = (char*)(ROM+addr);
  unsigned int length = rom_w(addr+8);
  addr = addr + rom_w(addr+4);
  if (strncmp(id, "dio ", 4)==0) {
    writeRexBlock(f, addr, length); // ??
  } else if (strncmp(id, "gpio", 4)==0) {
    writeRexBlock(f, addr, length); // ??
  } else if (strncmp(id, "ralc", 4)==0) {
    writeRexBlock(f, addr, length); // kRAMAllocationTag
  } else if (strncmp(id, "pkgl", 4)==0) {
    writeRexBlock(f, addr, length); // kPackageListTag
  } else if (strncmp(id, "pad ", 4)==0) {
    writeRexBlock(f, addr, length); // kPadBlockTag
  } else if (strncmp(id, "ptpt", 4)==0) {
    writeRexBlock(f, addr, length); // kPatchTablePageTableTag
  } else if (strncmp(id, "glpt", 4)==0) {
    writeRexBlock(f, addr, length); // kGelatoPageTableTag
  } else if (strncmp(id, "fexp", 4)==0) {
    writeRexBlock(f, addr, length); // kFrameExportTableTag
  } else if (strncmp(id, "jump", 4)==0) {
    writeRexBlock(f, addr, length); // kCPatchTableTag
  }
}

void writeRExConfigEntry(FILE *f, unsigned int addr) {
  printf("\nRExBlock Config Entry:\n");
  printf("tag:    '%c%c%c%c'\n", ROM[addr], ROM[addr+1], ROM[addr+2], ROM[addr+3]);
  printf("offset: 0x%08X\n", rom_w(addr+4));
  printf("length: 0x%08X\n", rom_w(addr+8));
}


void writeREx(const char *filename, unsigned int addr) 
{
  FILE *f = fopen(filename, "wb");
  if (!f) return;
  
  if (::strncmp((char*)(ROM+addr), "RExBlock", 8)!=0) {
    printf("ERROR: no REx at this address.\n");
    fclose(f);
    return;
  }
  // +8 checksum
  // +12 headerVersion
  // +16 manufacturer
  // +20 version
  // +24 length
  // +28 id
  // +32 start
  unsigned int iCfg, numConfigEntries = rom_w(addr+36);
  for (iCfg = 0; iCfg<numConfigEntries; iCfg++) {
    writeRExConfigEntry(f, addr+40+iCfg*12);
  }
  for (iCfg = 0; iCfg<numConfigEntries; iCfg++) {
    writeRExConfigBlock(f, addr+40+iCfg*12);
  }
  
  fclose(f);
}


void analyzePerfFile()
{
  unsigned int i, addr;
  unsigned long long hits, fnHits = 0;
  char buf[4096];
  
  AlData *fn = 0, *nextFn = 0;

  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "../p1.txt");
  FILE *f = fopen(filename_buffer, "rb");
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "../p1fn.txt");
  FILE *o = fopen(filename_buffer, "wb");
  for (i=0; i<0x00800000; i++) {
    fgets(buf, 2047, f);
    if (buf[0]=='-' || buf[0]==0) continue;
    sscanf(buf, "%08X: %19llu", &addr, &hits);
    nextFn = gMemoryMap.find(addr);
//  if (dynamic_cast<AlCMethod*>(d)) {
    if (nextFn) {
      if (fn==0) {
        fn = nextFn;
      } else if (nextFn) {
        fprintf(o, "%19llu 0x%08X: %s\n", fnHits, fn->address(), fn->label());
        fn = nextFn;
        fnHits = 0;
      }
    }
    fnHits += hits;
  }
  fclose(o);
  fclose(f);
}


void zeroFlags()
{
  memset(ROM_flags, 0, 0x00200000*sizeof(int));
}


void readFlags()
{
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "data/flags");
  FILE *rom_flags = fopen(filename_buffer, "rb");
  if (!rom_flags) {
    puts("Can't read ROM flags, assuming zero!");
    memset(ROM_flags, 0, 0x00200000*sizeof(int));
  } else {
    fread(ROM_flags, 0x00200000, sizeof(unsigned int), rom_flags);
    fclose(rom_flags);
  }
}


void checkCodeCoverage()
{
  printf("\n====> Checking Newton ROM use for ARM assembler code\n\n");
  if (gCodeCoverageChecked)
    return;
  try {
    check_all_code_coverage();
  } catch(char *err) {
    puts(err);
  }
  gCodeCoverageChecked = true;
}


void checkScriptCoverage()
{
  printf("\n====> Checking Newton ROM use for NewtonScript code\n\n");
  try {
    check_all_ns_coverage();
  } catch(char *err) {
    puts(err);
  }
}


void writePseudoC()
{
  printf("\n====> Writing Newton ROM in pseudo C++ code (C++ methods only)\n\n");
  AlClass::write_all(cpp_path);
  
  FILE *f;
  
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "src/strings.h");
  f = fopen(filename_buffer, "wb");
  // FIXME: add headers
  AlCString::write_all_h(f);
  fclose(f);
  
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "src/strings.c");
  f = fopen(filename_buffer, "wb");
  // FIXME add includes
  AlCString::write_all_c(f);
  fclose(f);
}


void writeBinaryROMMap()
{
  printf("\n====> Writing binary Newton ROM usage map\n\n");
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "data/flags");
  FILE *rom_flags = fopen(filename_buffer, "wb");
  if (!rom_flags) {
    puts("Can't write ROM flags!");
  }
  fwrite(ROM_flags, 0x00200000, sizeof(int), rom_flags);
  fclose(rom_flags);
}


void writeBinaryROMMapForC()
{
  printf("\n====> Writing binary Newton ROM usage map\n\n");
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "data/cMap.c");
  FILE *rom_flags = fopen(filename_buffer, "wb");
  if (!rom_flags) {
    puts("Can't write ROM flags!");
  }
  int i=0, j;
  while (i<0x00800000) {
    fprintf(rom_flags, "/* 0x%08X */ ", i);
    for (j=0; j<256; j+=4) {
      unsigned int f = rom_flags_type(i+j);
      if (f>9) f = 9;
      fprintf(rom_flags, "%d,", f);
    }
    fprintf(rom_flags, "\n");
    i+=256;
  }
  fclose(rom_flags);
}


void writeRGBROMMap()
{
  FILE *rom_flags;
  int i;
  printf("\n====> Writing Newton ROM usage map as RGB pixel stream\n\n");
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "data/flags.rgb");
  rom_flags = fopen(filename_buffer, "wb");
  if (!rom_flags) {
    puts("Can't write ROM flags!");
  }
  for (i=0; i<0x00800000; i+=4) {
#define pix(n) { fputc((n>>16)&0xff, rom_flags); fputc((n>>8)&0xff, rom_flags); fputc(n&0xff, rom_flags); }
    switch (rom_flags_type(i)) {
      case flags_type_unknown:      pix(0x000000); break;
      case flags_type_arm_code:     pix(0xff0000); break;
      case flags_type_arm_word:     pix(0xff8800); break;
      case flags_type_arm_byte:     pix(0xff0088); break;
      case flags_type_patch_table:  pix(0xff8800); break;
      case flags_type_jump_table:   pix(0xff0088); break;
      case flags_type_unused:       pix(0x888888); break;
      case flags_type_rex:          pix(0x0000ff); break;
      case flags_type_ns:           pix(0x00ff00); break;
      case flags_type_ns_obj:       pix(0x88ff00); break;
      case flags_type_ns_ref:       pix(0x00ff88); break;
      case flags_type_dict:         pix(0xffff00); break;
      default: pix(0xffffff); break;
    }
#undef pix
  }
  fclose(rom_flags);
}


void readROMAndSymbols()
{
  char buf[1024];
  getcwd(buf, 1024);
  puts(buf);
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "../");
  chdir(filename_buffer);
  
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "data/717006");
  FILE *rom = fopen(filename_buffer, "rb");
  if (!rom) {
    fprintf(stderr, "ERROR: Can't read ROM!");
    fprintf(stderr, "Please copy the binary ROM file 717006 to %s\n", filename_buffer);
    return;
  }
  fread(ROM, 0x00800000, 1, rom);
  fclose(rom);
  
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "data/717006.cppsymbols.txt");
  char buf2[2048];
  strcpy(buf2, base_path);
  strcat(buf2, "data/717006.symbols.txt");
  readSymbols(filename_buffer, buf2);
  {
    strcpy(filename_buffer, base_path);
    strcat(filename_buffer, "test.txt");
    AlDatabase test(filename_buffer, "wb");
    gMemoryMap.write(test, true);
  }
  {
    strcpy(filename_buffer, base_path);
    strcat(filename_buffer, "type.txt");
    AlDatabase test(filename_buffer, "wb");
    gTypeList.write(test, true);
  }
}


void loadDatabase()
{
  load_db(db_path, "symbols.txt");
  preset_rom_use();
}


void writeROMFlags()
{
  writeROMFlags();
  int i, n=0;
  for (i=0; i<0x00200000; i++) {
    if (ROM_flags[i]) n++;
  }
  printf("\n====================\n");
  printf("%7.3f%% of ROM words covered (%d of 2097152)\n", n/20971.52, n);
  printf("====================\n\n");
}


extern char writeBitmap(unsigned int);
extern char writeBitmap(const char *, unsigned int);
extern char extractColorBitmap(unsigned int bits);
extern char writePICT(unsigned int, unsigned int);


void writeWAV(unsigned int addr, unsigned int size)
{
  char buf[4096];
  sprintf(buf, "%sAlbert/NewtonOS/sounds/SND_%08X.wav", base_path, addr-12);
  FILE *f = fopen(buf, "wb");
  char RIFF[] = "RIFF"; fwrite(RIFF, 1, 4, f);
  unsigned int chunkSize = size+36; fwrite(&chunkSize, 1, 4, f);
  char WAVE[] = "WAVE"; fwrite(WAVE, 1, 4, f);
  char fmt[] = "fmt "; fwrite(fmt, 1, 4, f);
  unsigned int fmtSize = 16; fwrite(&fmtSize, 1, 4, f);
  unsigned short pcm = 1; fwrite(&pcm, 1, 2, f);
  unsigned short chans = 1; fwrite(&chans, 1, 2, f);
  unsigned int rate = 22050; fwrite(&rate, 1, 4, f);
  unsigned int byteRate = 44100; fwrite(&byteRate, 1, 4, f);
  unsigned short block = 2; fwrite(&block, 1, 2, f);
  unsigned short bps = 16; fwrite(&bps, 1, 2, f);
  char data[] = "data"; fwrite(data, 1, 4, f);
  unsigned int dataSize = size; fwrite(&dataSize, 1, 4, f);
  int i;
  for (i=0; i<size; i+=2) {
    unsigned short sample = (ROM[addr+i]<<8) + ROM[addr+i];
    fwrite(&sample, 1, 2, f);
  }
  fclose(f);
}


void findBitmaps()
{
  unsigned int i, prev = 0;
  for (i=0; i<0x800000; i+=4) {
    unsigned int bin = rom_w(i);
    if (((bin&0x000000ff)!=0x00000040)&&((bin&0x000000ff)!=0x000000C0)) continue;
    unsigned int size = bin>>8;
    unsigned int zero = rom_w(i+4);
    if (zero!=0) continue;
    unsigned int pSym = (rom_w(i+8) & 0xfffffffc);
    if (pSym>=0x800000) continue;
    //    if (pSym==0x003BBC78) continue; // thise are all the bitmaps we found already
    unsigned int hash = rom_w(pSym+12);
    if (hash==0x1E4F7F22) {
//      printf("found a bitmap image at 0x%08x\n", i);
      printf("<data type=\"unknown\" start=\"0x%08X\" end=\"0x%08X\" />\n",
             prev, i);
      prev = i+size;
      
      printf("<include.bitmap label=\"L%08X\" top=%d left=%d sym=\"0x%08X\" start=\"0x%08X\" end=\"0x%08X\" />\n",
             i, rom_s(i+20), rom_s(i+22), rom_w(i+8)-1, i, i+size);
//      writeBitmap(i);
    } else if (hash==0x6902A4CC) {
//      printf("found a bitmap mask at 0x%08x\n", i);
//      writeBitmap(i);
      printf("<data type=\"unknown\" start=\"0x%08X\" end=\"0x%08X\" />\n",
             prev, i);
      prev = i+size;
      printf("<include.bitmap label=\"L%08X\" top=%d left=%d sym=\"0x%08X\" start=\"0x%08X\" end=\"0x%08X\" />\n",
             i, rom_s(i+20), rom_s(i+22), rom_w(i+8)-1, i, i+size);
    } else if (hash==0x86D45A8D) {
//      printf("found a color bitmap at 0x%08x\n", i);
//      extractColorBitmap(i);
    } else if (hash==0xBD04C23C) {
//      printf("found a pict at 0x%08x\n", i);
//      writePICT(i+12, size-12);
//    } else
//    if (hash==0x69806E2D) { // samples
//      printf("found a sound at 0x%08x\n", i);
//      writeWAV(i+12, size-12);
    }
  }
  //writeBitmap(i);
}


int main(int argc, char **argv)
{
  strcpy(base_path, src_file_path);
  char *end = strrchr(base_path, '/');
  *end = 0;
  end = strrchr(base_path, '/');
  end[0] = '/';
  end[1] = 0;
  
  
  readROMAndSymbols(); // this must always be enabled

#if 0
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "symbysize.txt");
  writeSymbolStatistics(filename_buffer);
#endif

#if 1
  zeroFlags();
#else
  readFlags();
#endif
  
  loadDatabase();

#if 1
  checkCodeCoverage();
#endif

#if 1
  checkScriptCoverage();
#endif
  
#if 0
  writePseudoC();
#endif
  
#if 0
//  writeBinaryROMMap();
  writeBinaryROMMapForC();
#endif
  

#if 0
  writeRGBROMMap();
#endif

#if 0
  writeNewtonROMTexts();
#endif
  
#if 1
  writeNewtonROM();
#endif
  
#if 0
  writeNewtonROM_C();
#endif
  
#if 0
  writeNewtonROMGlobals();
#endif
  
#if 0
  writeNorcroftROM();
#endif
  
#if 0
  writeNewtonPseudoC();
#endif
  
#if 0
  extractStencils();
#endif
  
#if 0
  writeROMFlags();
#endif
    
#if 0
  {
    strcpy(filename_buffer, base_path);
    strcat(filename_buffer, "fn.cpp");
    FILE *f = fopen(filename_buffer, "wb");
    decompile_function(f, 0x00394688, 0x00394714);
    fclose(f);
  }
#endif
  
#if 0
  //analyse_class("CItemComparer");
  analyse_all_classes();
#endif
  
#if 0
  find_virtual_methods();
#endif
  
#if 1
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "NewtonOS/apple.rex.s");
  writeREx(filename_buffer, 0x0071FC4C);
#endif
  
#if 0
  createCallGraphInformation();
  writeCallsPerFunction();
  writeNumUniqueCallsPerFunction();
#endif
  
#if 0
  strcpy(filename_buffer, base_path);
  strcat(filename_buffer, "DataAbortHandler.graph");
  FILE *f = fopen(filename_buffer, "wb");
  writeCallGraph(f, 0x00393114, 9999);
  fclose(f);
#endif
  
#if 0
  listAllEntryPoints();
  listAllEndPoints();
#endif 
  
#if 0
  analyzePerfFile();
#endif
  
#if 0
  findBitmaps();
#endif
  
  return 0;
}


void writeImageFiles()
{
  gMemoryMap.at(0x003C8A6C)->label("IMG_003C8A6C");
  rom_flags_set(0x003C8A6C, 0x003C8ABC, flags_include);
}


/**
 * Analyse the ROM and output an ARM assembler file that reassembles into a ROM.
 */
int bmain(int argc, char **argv)
{
  readROMAndSymbols(); // this must always be enabled
  zeroFlags();
  checkCodeCoverage();
  checkScriptCoverage();
  writeImageFiles();
  writeNewtonROM();
  return 0;
}


void pathToMakeVar(char *buf, char *path, char appendix=0)
{
  const char *s = path;
  char *d = buf;
  for (;;) {
    char c = *s++;
    if (c==0)
      break;
    if (isalnum(c)) {
      c = toupper(c);
    } else {
      c = '_';
    }
    *d++ = c;
  }
  if (appendix) *d++ = appendix;
  *d = 0;
}



int fillType(const char **srcp, char *dst, const char *sym)
{
  static int depth = 0;
  
  int len, rpt, idx;
  char pre[1024], post[1024];
  pre[0] = 0; post[0] = 0;
  const char *src = *srcp;
  char c = *src++;
  *srcp = src;
  if (c==0)
    return 0;
  depth++;
  for (;;) {
    switch (c) {
      case 0:
        depth--; return 0; // unexpected!
      case 'P':
        if (*src=='C') { strcat(post, "const "); src++; }
        strcat(post, "*"); break;
      case 'R':
        if (*src=='C') { strcat(post, "const "); src++; }
        strcat(post, "&"); break;
      case 'C': strcat(post, "const "); break;
      case 'U': strcat(pre, "unsigned "); break;
      case 'S': strcat(pre, "signed "); break;
      case 'V': strcat(pre, "volatile "); break;
      case 'F': { // PFiii_v  ->  void (*)(int, int, int)
        if (depth>3) {
          return 0;
        }
        char buf[4096];
        char args[4096];
        args[0] = 0;
        for (;;) {
          if (fillType(&src, buf, sym)==0)
            break;
          if (args[0])
            strcat(args, ", ");
          strcat(args, buf);
        }
        if (src[-1]=='_') {
          fillType(&src, buf, sym);
        }
        sprintf(dst, "%s (%s)(%s)", buf, post, args);
        *srcp = src;
        depth--; return 1; }
      case 'M': { // Pointer to member, followed by two arbitrary types
        char buf[4096];
        fillType(&src, buf, sym);
        strcat(post, buf);
        strcat(post, "::");
        break; }
      case 'Q': {
        char buf[4096];
        int i;
        rpt = (*src++)-'0';
        for (i=rpt; i>0; i--) {
          fillType(&src, buf, sym);
          strcat(pre, buf);
          if (i>1)
            strcat(pre, "::");
        }
        goto done; }
      case 'A':
        len = atoi(src);
        while (isdigit(*src)) src++;
        sprintf(post, "[%d]", len);
        if (*src=='_') c = *src++;
        goto done;
      case 'T':
        idx = (*src++)-'0';
        sprintf(pre, "#1%d", idx);
        goto done;
      case 'N':
        rpt = (*src++)-'0';
        idx = (*src++)-'0';
        sprintf(pre, "#%d%d", rpt, idx); // FIXME: this does not work if called in a recursion!
        goto done;
      case 'l': strcat(pre, "long"); goto done;
      case 'd': strcat(pre, "double"); goto done;
      case 's': strcat(pre, "short"); goto done;
      case 'i': strcat(pre, "int"); goto done;
      case 'c': strcat(pre, "char"); goto done;
      case 'v': strcat(pre, "void"); goto done;
      case 'e': strcat(pre, "..."); goto done;
        //case '_': strcat(post, ")"); goto done; // end of function pointer arguments
      case '0'...'9':
        len = atoi(src-1);
        while (isdigit(c)) c = *src++;
        strncat(pre, src-1, len);
        src = src + len -1;
        goto done; // verbatim copy
      case '_':
        depth--; return 0;
      default:
        printf("\nUnknown type: '%c' (%s)\n", c, sym);
        depth--; return 0;
    }
    c = *src++;
    *srcp = src;
  }
done:
  strcpy(dst, pre);
  if (post[0]) strcat(dst, " ");
  strcat(dst, post);
  *srcp = src;
  depth--;
  return 1;
}



void recursiveMakePath(const char *path)
{
  char buf[4096];
  strcpy(buf, path);
  char *nn = strrchr(buf, '/');
  if (nn && nn>buf) {
    *nn = 0;
    recursiveMakePath(buf);
    mkdir(path, 0777);
  }
}


#define NOS_FILE_CPP  0xFFFFFFF0
#define NOS_FILE_C    0xFFFFFFF1
#define NOS_FILE_OBJ  0xFFFFFFF2
#define NOS_FILE_NS   0xFFFFFFF3
#define NOS_FILE_END  0xFFFFFFFF

typedef struct { unsigned int start; unsigned int end; const char *name; } Grouping;
typedef Grouping *GroupingPtr;

#include "grouping.h"



char *decodeCppSymbol(const char *sym) {
  static char buf[4096];
  buf[0] = 0;
  char isConst = 0;
  if (strstr(sym, "__")) {
    char name[4096], klass[4096];
    const char *src = sym;
    char *dst = name;
    char c = *src++;
    *dst++ = c;
    for (;;) {
      c = *src++;
      if (c==0) break;
      if (c=='_' && src[0]=='_' && src[1]!='_') {
        *dst = 0;
        break;
      }
      *dst++ = c;
    }
    if (c=='_') {
      src++;
      c = *src;
      if (isdigit(c)) {
        int len = atoi(src);
        while (isdigit(c)) c = *src++;
        memcpy(klass, src-1, len);
        klass[len] = 0;
        src+=len-1;
      } else {
        klass[0] = 0;
      }
      if (name[0]=='_' && name[1]=='_') {
        if (strcmp(name, "__ct")==0) strcpy(name, klass);
        else if (strcmp(name, "__dt")==0) sprintf(name, "~%s", klass);
        else if (strcmp(name, "__ne")==0) strcpy(name, "operator!=");
        else if (strcmp(name, "__as")==0) strcpy(name, "operator=");
        else if (strcmp(name, "__eq")==0) strcpy(name, "operator==");
        else if (strcmp(name, "__gt")==0) strcpy(name, "operator>");
        else if (strcmp(name, "__lt")==0) strcpy(name, "operator<");
        else if (strcmp(name, "__rs")==0) strcpy(name, "operator>>");
        else if (strcmp(name, "__ls")==0) strcpy(name, "operator<<");
        else if (strcmp(name, "__vc")==0) strcpy(name, "operator[]");
        else if (strcmp(name, "__nw")==0) strcpy(name, "operator new");
        else if (strcmp(name, "__dl")==0) strcpy(name, "operator delete");
        else if (strcmp(name, "__nw_v")==0) strcpy(name, "operator new[]");
        else if (strcmp(name, "__dl_v")==0) strcpy(name, "operator delete[]");
        else if (strcmp(name, "__opl")==0) strcpy(name, "operator (long)");
        else if (strcmp(name, "__opUs")==0) strcpy(name, "operator (unsigned short)");
        else if (strcmp(name, "__opPc")==0) strcpy(name, "operator (char*)");
        else if (strcmp(name, "__opd")==0) strcpy(name, "operator (double)");
        else if (strcmp(name, "__push_ddtor")==0) { /* unknown */ }
        else if (strcmp(name, "__GetErrNo")==0) strcpy(name, "_GetErrNo");
        else if (strcmp(name, "__pvfn")==0) { /* non-existing virtaul function (=0) */ }
        // pvfn pointer to a function returning a void
        else {
          fprintf(stderr, "decodeCppSymbol: unknown '%s'\n", name);
          // unknown shortcut, maybe a bad name, maybe intentional, maybe a symbol starting in a single "_"
        }
      }
      c = *src++;
      if (c=='S') { strcat(buf, "static "); c = *src++; }
      if (c=='C') { isConst = 1; c = *src++; }
      if (klass[0]) { strcat(buf, klass); strcat(buf, "::"); }
      strcat(buf, name);
      if (c=='F') {
        strcat(buf, "(");
        char prev[30][128];
        int i, rpt = 0, idx = 0;
        for (i=1;;i++) {
          char abuf[1024]; abuf[0] = 0;
          if (rpt==0) {
            if (fillType(&src, abuf, sym)==0) break;
            if (abuf[0]=='#') {
              rpt = abuf[1]-'0';
              idx = abuf[2]-'0';
            }
          }
          if (rpt) {
            strcpy(abuf, prev[idx]);
            rpt--;
          }
          strcpy(prev[i], abuf);
          if (i>1) strcat(buf, ", ");
          strcat(buf, abuf);
        }
        strcat(buf, ")");
      }
      if (isConst) strcat(buf, " const");
    }
  } else {
    return 0;
  }
  return buf;
}


/*
:map <f1> kdwdwj$p052xi  rom_flags_type(<esc>11li, flags_type_arm_word); //  <esc>19xkddjj0
:map <f2> 0df;xi  rom_flags_type(<esc>11li, flags_type_arm_word); // <esc>j0
:map <f3> 0df;xi  rom_flags_type(<esc>11li, flags_type_arm_text); // <esc>j0
:map <f4> 0df;xi  check_code_coverage(<esc>11li); // <esc>j0
:map <f5> ma0k12lywj$p0df;xi  rom_flags_type(<esc>11li, , flags_type_data); // <esc>j0/:<cr>
:map <f6> kmbjjf;llyw'af,lpj0V'bd/unknown<cr>
 
// serach for /\.word\t0xE and press F1 if it is a function. Command will be added to end of file.
:map <f1> 21l10ylmaGA<cr>check_code_coverage(<esc>pA);<esc>'a
*/

// FIXME: differentiate between text and bytes!

// Di 10 aug 2010, 15:05:   0.649% of ROM words covered (13604 of 2097152)
//                 15:17:   1.106% of ROM words covered (23196 of 2097152)
//                 20:12:  34.491% of ROM words covered (723334 of 2097152) ARM
// Mi 11 aug 2010, 01:31:  47.176% of ROM words covered (989357 of 2097152) ARM,NS
//                 01:51:  47.874% of ROM words covered (1003989 of 2097152)
//                 01:57:  50.384% of ROM words covered (1056626 of 2097152) +unused+rex
//                 09:08:  50.792% of ROM words covered (1065179 of 2097152)
//                 21:31:  82.094% of ROM words covered (1721642 of 2097152) +more NS
// Do 12 aug 2010, 00:44:  89.386% of ROM words covered (1874570 of 2097152) +dictionaries
//                 00:47:  89.938% of ROM words covered (1886141 of 2097152) +ldr word access
// Sa 14 aug 2010, 08:53:  90.163% of ROM words covered (1890855 of 2097152) +ClassInfo
//                 12:51:  92.562% of ROM words covered (1941167 of 2097152) +register based calls detection
//                 22:23:  94.200% of ROM words covered (1975523 of 2097152) +manual additions
//                 23:14:  94.477% of ROM words covered (1981325 of 2097152) +more
// So 15 aug 2010, 09:45:  94.499% of ROM words covered (1981780 of 2097152)
//                 13:50:  94.885% of ROM words covered (1989887 of 2097152)
//                 15:04:  96.456% of ROM words covered (2022828 of 2097152) +more functions found
//                 20:37:  98.305% of ROM words covered (2061610 of 2097152)
//                 21:13:  99.591% of ROM words covered (2088566 of 2097152)
//                 21:44:  99.987% of ROM words covered (2096871 of 2097152)
//                 21:59: 100.000% of ROM words covered (2097152 of 2097152) PARTY!!

// So 13 feb 2011, 21:10: 731,205 unknown words, 209,643 of those are in the REX
//                 22:04: 719,163 unknown words: ASCII text concatenating
// Mo 14 feb 2011, 01:52: 689,304 unknown words: Unicode text 
//                 02:32: 682,173 unknown words: Newton Script native function calls
//                 02:54: 681,763 unknown words: Newton Script unichar encoding
//                 17:45: 681,623 unknown words: Newton Script real encoding
//                 21:11: 664,123 unknown words: Newton Script bitmaps

// compressed tables (Dictionary) at gEnum80DaysMonths (0x006853DC) and following!
// where are the graphics that are shown after cold boot?
// how are the REXes handled? Starting at 0x0071FC4C to 0x007EC7FC
// there are a few "funny" .fill instructions at the end of the ROM

/*
 
 //writeCallGraph("/Users/matt/dev/Albert/callgraph.dot", 0x000466CC CBufferList::DeleteLast(void) , 8 );
//writeCallGraph("/Users/matt/dev/Albert/callgraph.dot", 0x0004590C, 8 );
//writeCallGraph("/Users/matt/dev/Albert/callgraph.dot", 0, 6);



ROM analyser:
 Ansatz wie im Matherätsel, Sudoku, etc.
 1: array mit 2M worten: was macht jedes einzelne Wort? Wie kann es übersetzt werden
 2: array mit vielen Adresslabels (auch ausserhalb des ROMs!): auf welche art daten zeigt das label
 3: array mit typen
 3a: einache typen: wann ist ein int eine ID, wann ein numerischer wert, etc.?
 3b: structs und class: wie gross, wovon abgeleitet, welcher typ an welchem Index?
 4: weiter: Return Werte aller Funktionen
 5: einfache Symbole (z.B. fehlermeldungen)
 6: pseudo C++ code in guter Dateistruktur (kompilierbar!)
 7: Erkennen von Loops und Conditions
 8: Umwandlung von stack und regsiter spaces in lokale variabeln.
 
 Die bekannten Includes müssen analysiert werden und eine Type-Datenbank erstellen
 Eine ROM-Datenbank zeigt zu bekannten ROM bereichen (z.B. Magics)
 Eine Cross-Reference zwischen bekannten Labels und ROM wird erstellt.
 
 Alle words im ROM werden nacheinander analysiert, bis wir keine neue Information finden
 Jede Iteration kann in eine weitere Datenbank geschrieben werden, die dann später
 wieder hergestellt werden kann, um an dieser Stelle weiterzuarbeiten.
 
 Anlegen mehrerer Ausgabeformate: Machine code (funktioniert bereits!), Bilder,
 Texte, Audio, pseudo C/C++ mit Doxygen Tags (Verzeichnis- und Dateiliste für alle Daten!)
 
  1: AlAddress -> what's happening at a specific space in memory (address, value)
       AlARMCode -> something that will be run by the CPU (hasLabel, isEntryPoint, isExitPoint, etc. etc.)
       AlARMData -> data that is inside a function
       AlNSData -> NewtonScript
  2: AlSymbol -> where does this symbol point (symbol, address)
  3: AlType -> what type is this (symbol)
       AlAtomicType -> integer, float, ID's, char, fixed, etc.
       AlComplexType -> Structs and classes
 
 
 ---
 
 Alle includes sind hier:
 "/Users/matt/Desktop/ALLES AUFRÄUMEN/Newton/Newton Mix/Newton/NewtonDev/C++/NCT_Projects"

 ---
 
 We need a database, files that can be edited, and that update the database, and then generate a compilable setup, using the ROM
 
 
 <?xml version="1.0"?>
 <dir name=ROM>
   <file name=all.s type=ARM>
     <data type=ARMData at=00000000 size=00800000>
       Generate simple output
     </data>
   </file>
 </dir>
 <dir name=REx>
 </dir>
 
 <?xml version="1.0"?>
 <dir name=ROM>
 <file name=all.s type=ARM>
 <data type=ARMData at=00000000 size=00800000>
 Generate simple output
 </data>
 </file>
 </dir>
 <dir name=REx>
 </dir>

 
 http://www.unna.org/unna/development/Debugger_Images/MP2x00_US.sit
 

 ./getROM
 bin/db2src 717006.debug newtonos.xml
 bin/gmake

 
 
 */

/*
 Inline database:
 
 Create a databse inside the source code. As the code evolves, the database is 
 dragged along, helping to decompilation effort.
 
 Start condition: 
  * original files, usually in banary format
  * helper files, headers, code snippets, documentation
  * a Makefile
 
 Goal:
  * 'make source' will generate a source tree from its own database
  * 'make clean; make' will regenerate files identical to the originals
  * 'make distclean' will generate a pure databse with no copyrihted material,
     but enough information to recreate the entire source when combined with the
     original
  * 'make update' grabs all changes from the main repository
  * 'make commit' sends updates (it implies 'make distclean')
 
 Requirements:
  * incrmental process, no duplicate work needed
  * all source code files are ASCII and contain the databse as well
  * easy editing on any platform including mobiles and tablets
  * everything can be checked in and out of a source code repository (svn, git)
  * original tools should be used, but new tools can be generated as needed
    'make tools' should be able to generate all tools from scratch
 
 Solution:
  * the database is integrated into all source code and helper files
  * the database contains commands and options, for example: 
      apply spilt file="hello.c"
  * db commands can be put anywhere into comments of the particular file type 
    in square brackets:
      .cpp   // [[data size=32 type=long]]
      .S     ; [[apply split file="Hello.cpp"]]
  * the db command [[*]] delimits copyrighted material: everything from the 
    beginning of the line to this point - except for the comment marker - will 
    be deleted for distclean
      .cpp    x = 1234;  // [[secret info on 123]] [[*]] [[public infor on 123]]
  * the [[@1234]] syntax refers to some useful label in the original file. The
    tools determine how a useful label is constructed
 
 Process:
  * 'make apply' finds [[apply... sequences in any of the source codes, helpers,
     or Makefiles. The command following the 'apply' command is then executed.
     This command may change Makefiles!
  * 'make source' then find all changed files and recreates the autogenerated
     code portion, leaving the database portion intact
  * 'make' runs every tool needed to create a copy of the original
 
 Details:
  * start in a new directory, set the path to the existing binary tool set
  * add the '_original_' directory, containing the original code file
  * add the '_clone_' directory which will receive the clones of the originals
  * add the '_tools_' directory and subdirs for every specialized tool
  * add the '_docs_' and '_rsrc_' directory for further information about the environment
  * add a Makefile. The first command is probably something like # [[apply create file='all.s' type=asm cpu=arm from='_original_/ROM.bin]]
 
 Ideas for commands:
 
 [*] : generated and possibly copyrighted code. Will be cleared on 'make distclean' and regenerated during 'make src'
 [@1234] : this and all following commands refer to this address - usually the byte position within the original file
 [set variable=value] : sets an environment variable
 [apply cmd arg=value ...] : during 'make apply', run the given shell command

 [file name=test.c path=my/path start=123 end=456 format=c] : 
 [code size=4 cpu=arm] :
 [data size=4 type=long value=123] :
 
 [push variable=value] [pop variable] : store, change and recall a variable
 

 Example:
 
 @ [set cpu=arm]
 @ [@0005266C]
 @ [method class=TCardServer name=DoPollLockSwitchAndCardDetected \
    type0=p_TCardServer name0=this \
    type1="unsigned long" \
    returntype="NewtErr" ]
    mov     r12, sp         @ [*]
 
    add   sp, sp, #12       @ [*] remove local variables [@00052904][code][data type=int value=12]
    b     L0005292C         @ [*][@00052908][code][data type=ptr value=L0005292C]
 L0005290C:                 @ [*][@0005290C][label]
    .word 0x00059FD8        @ [*][data type=ptr value=PowerInputOff__PCirrusSoundDriver... name=PCirrusSoundDriver::PowerInputOff(...)]
 L00052910:                 @ [*][@00052910]
    stmdb sp!, {r2, r3}     @ [*] save registers on stack [code]
    mov   r2, r7            @ [*][@00052914][code]
 */

