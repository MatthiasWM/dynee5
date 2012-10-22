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

#include "Albert.h"

#include "db2src.h"

#include "decomp.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <map>

#define BDISP(x) ((((x) & 0xffffff) ^ 0x800000) - 0x800000) /* 26 bit */

extern int disarm(char *dst, unsigned int addr, unsigned int cmd);
extern int disarm_c(char *dst, unsigned int addr, unsigned int cmd);
extern const char *getSymbol(unsigned int i);
extern const char *getSafeSymbol(unsigned int i);


//const char *db_path = "./";
const char *db_path = "/Users/matt/dev/Albert/";
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
};

unsigned char ROM[0x00800000];
unsigned int ROM_flags[0x00200000];

typedef std::multimap < unsigned int, unsigned int > AlCallGraph;
typedef std::multimap < unsigned int, unsigned int >::iterator AlCallGraphIterator;
AlCallGraph callGraph;

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
  char dbuf[2048];
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
  static char buf[2048];
  static char *dst = buf;
  // concatenate segments until we can flush an entire line
  va_list va;
  va_start(va, pat);
  vsnprintf(dst, 2048, pat, va);
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
  static char buf[2048];
  va_list va;
  va_start(va, pat);
  vsnprintf(buf, 2048, pat, va);
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

const char *get_symbol_at(unsigned int addr)
{
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
          char buf[256];
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
    int n = sscanf(s, "0x%08x %[^\n]\n", &addr, sym);
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
    int n = sscanf(s, "0x%08x %[^\n]\n", &addr, sym);
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
      char buf[256], cmd[32], arg[256];
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
        sscanf(s, "%s 0x%08x", cmd, &pAt);
      } else if (strcmp(cmd, "next")==0) {
        sscanf(s, "%s 0x%08x", cmd, &pNext);
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
      char buf[256], cmd[32], arg[256];
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
      char buf[256], cmd[32], arg[256];
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
        sscanf(s, "%s 0x%08x", cmd, &pAt);
      } else if (strcmp(cmd, "next")==0) {
        sscanf(s, "%s 0x%08x", cmd, &pNext);
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
      char buf[256];
      if (rom_flags(i)&1) 
        fprintf(f, "L%08X:\n", i);
      disarm_c(buf, i, rom_w(i));
      fprintf(f, "  %s\n", buf);
    }
    fprintf(f, "}\n\n");
  }
};


void AlClass::write_h(char const *path) {
  char filename[2048];
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
  char filename[2048];
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
  char name[2048];
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

unsigned int branch_address(unsigned int addr, unsigned int cmd=0xffffffff)
{
  if (cmd==0xffffffff)
    cmd = rom_w(addr);
  if (cmd&0x00800000) { // jump backwards
    return (((cmd&0x00ffffff)|0xff000000)<<2)+addr+8;
  } else { // jump forward
    return ((cmd&0x007fffff)<<2)+addr+8;
  }
}

unsigned int branch_address_in_ROM(unsigned int addr, unsigned int cmd=0xffffffff)
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
  printf("Switch/Case statement with %d cases starting at %08x\n", n_case, addr);
  for (i=0; i<n_case; i++) { // nuber of cases plus default case
    VERB4 printf("case %d at %08x\n", i, addr+4*i);
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
        VERB3 printf("Redirecting call from %08x to %08x (%s->%s)\n", prev, addr, prev_sym, addr_sym);
        if ( prev!=0x01bdef54 && prev!=0x01bb294c && prev!=0x01bb4a50 && prev!=0x01bdef64 && prev!=0x01b4c658
            && (prev_sym==0 || addr_sym==0 || strcmp(prev_sym, addr_sym)!=0) ) {
          VERB2 printf("ERROR: Symbols don't match. Verify lookup table offsets! At: 0x%08x (0x%08x)\n", prev, addr);
          VERB2 printf("  (%s!=%s)\n", prev_sym, addr_sym);
          return;
        }
      } else {
        VERB2 printf("Can't follow addresses outside of ROM: %08x (%s)\n", addr, get_symbol_at(addr));
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
      VERB1 printf("Aborting: Hit unknown command at %08x: %08x\n", addr, cmd);
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
          VERB3 printf("%08x: unconditional jump to %08x\n", addr, branch_address(addr));
          addr = branch_address(addr);
          rom_flags_set(addr, flags_is_target);
          continue;
        } else { // conditional, follow both branches (finsih-thread-first recursion)
          unsigned int next = branch_address(addr);
          VERB3 printf("%08x: conditional jump to %08x, follow later\n", addr, next);
          check_code_coverage(addr+4, 0);
          VERB3 printf("%08x: following up on conditional jump to %08x\n", addr, next);
          addr = next; 
          rom_flags_set(addr, flags_is_target);
          continue;
        }
      } else if ( (cmd&0x0f000000) == 0x0b000000) { // branch instruction, follow both branches
        unsigned int next = branch_address(addr);
        VERB3 printf("%08x: subroutine call to %08x, follow later\n", addr, next);
        check_code_coverage(addr+4, 0);
        VERB3 printf("%08x: following up on subroutine call to %08x\n", addr, next);
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
            VERB3 printf("Switch/Case statement with %d cases at %08x: %08x\n", n_case, addr, cmd);
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
              VERB4 printf("case %d at %08x\n", i-1, addr+4*i);
              check_code_coverage(addr+4*i, 0);
            }
            return;
          }
          unsigned int cmd1 = rom_w(addr-4);
          if ( (cmd1&0x0fffffff)==0x01A0E00F && (cmd&0xf0000000)==(cmd1&0xf0000000)) { // mov lr,pc; ...
            // The return address is written into the link register, so in all probability this is a function call
            VERB2 printf("Later: Register based call at %08x: %08x\n", addr, cmd);
            addr += 4; continue;
          }
          if (addr==0x0038d9a4) { check_switch_case(0x0038d9ac, 33); return; }
          if (addr==0x0038ec98) { check_switch_case(0x0038eca0,  9); return; }
          if ((cmd&0xf0000000)==0xe0000000) { // always
            VERB1 printf("Aborting: Data processing command modifying R15 at %08x: %08x\n", addr, cmd);
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
        VERB2 printf("Aborting: opcode 'undefined' found at %08x: %08x\n", addr, cmd);
        ABORT_SCAN;
      } else if ( (cmd&0x0c100000) == 0x04000000) { // str (store to memory)
      } else if ( (cmd&0x0c100000) == 0x04100000) { // ldr (load from memory)
        if ( (cmd&0x0000f000) == 0x0000f000) { // is the destination register the pc?
          unsigned int cmd1 = rom_w(addr-4);
          if ( (cmd1&0x0fffffff)==0x01A0E00F && (cmd&0xf0000000)==(cmd1&0xf0000000)) { // mov lr,pc; ...
            // The return address is writte into the link register, so in all probability this is a function call
            VERB2 printf("Later: Register based call at %08x: %08x\n", addr, cmd);
            addr += 4; continue;
          }
          if ((cmd&0xf0000000)==0xe0000000) { // always
            VERB1 printf("Aborting: LDR command modifying R15 at %08x: %08x\n", addr, cmd);
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
        VERB1 printf("Aborting: Hit unknown command at %08x: %08x\n", addr, cmd);
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
    printf("ClassInfo: unsupported offset at %08x: %08x\n", addr, rom_w(addr));
    return;
  }
  unsigned int base = addr - 0x44 + 8;
  unsigned int class_name = rom_w(base+4)+base+4;
  unsigned int base_class = rom_w(base+8)+base+8;
  printf("class \"%s\" is derived from \"%s\"\n", ROM+class_name, ROM+base_class);
  unsigned int vtbl_start = rom_w(base+16)+base+16;
  unsigned int vtbl_end   = rom_w(base+20)+base+20;
  for (i=vtbl_start; i<vtbl_end; i+=4) {
    if (rom_w(i)) check_code_coverage(i, flags_is_function);
  }
  if (rom_w(base+24)) check_code_coverage(base+24, flags_is_function); // sizeof
  if (rom_w(base+28)) check_code_coverage(base+28, flags_is_function); // ??
  if (rom_w(base+32)) check_code_coverage(base+32, flags_is_function); // ??
  if (rom_w(base+36)) check_code_coverage(base+36, flags_is_function); // ctor
  if (rom_w(base+40)) check_code_coverage(base+40, flags_is_function); // dtor
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
    VERB3 printf("NSRef: Integer %d at %08x: %08x\n", v/4, addr, val);
    // top 30 bit are a (small) integer
  } else if ( (val&0x00000003) == 0x00000001 ) { // Pointer
    VERB3 printf("NSRef: Pointer at %08x: %08x\n", addr, val);
    check_ns_obj(val&0xfffffffc);
  } else if ( (val&0x00000003) == 0x00000002 ) { // Special
    // TODO: ...
    if (val==0x1a) {
      VERB3 printf("NSRef: TRUE %08x\n", addr);
    } else if (val==0x02) {
      VERB3 printf("NSRef: NIL at %08x\n", addr);
    } else if (val==0x00055552) {
      VERB3 printf("NSRef: symbol definition at %08x\n", addr);
    } else if (val==0x32) {
      VERB3 printf("NSRef: native function (?) at %08x\n", addr);
    } else if ((val&0xfff0000f) == 0x0000000a ) { // Character
      VERB1 printf("NSRef: Unicode Char 0x%04x at %08x\n", val, addr);
    } else {
      VERB3 printf("NSRef: unknown special at %08x: %08x\n", addr, val);
      ABORT_SCAN;
    }
  } else if ( (val&0x00000003) == 0x00000003 ) { // Magic Pointer
    // no need to follow, we have all magic pointers covered
    VERB3 printf("NSRef: Magic %d:%d at %08x: %08x\n", (val&0xffffc000)>>14, (val&0x00003ffc)>>2, addr, val);
  }
}

unsigned int check_ns_obj(unsigned int addr)
{
  unsigned int val = rom_w(addr);
  unsigned int flags = rom_w(addr+4);
  
  if ( (val&0x0000007c) != 0x00000040 || flags!=0 ) {
    VERB1 printf("int: not an NS object at %08x: %08x\n", addr, val);
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
    VERB3 printf("NS Binary Object at %08x (%d bytes):\n", addr, size);
    check_ns_ref(addr+8, 0);
    // mark the binary part as checked
    for (i=12; i<size; i+=4) rom_flags_type(addr+i, flags_type_ns);
    VERB4 ns_print_binary(addr, size);
    // followed by binary data and optional fill bytes
  } else if ( (val&0x00000003) == 0x00000001 ) {
    // array
    VERB3 printf("NS Array at %08x (%d bytes = %d entries):\n", addr, size, size/4-3);
    VERB3 printf("class: %08x\n", rom_w(addr+8));
    for (i=12; i<size; i+=4) {
      VERB3 printf("%5d: %08x\n", i/4-3, rom_w(addr+i));
    }
    for (i=8; i<size; i+=4) {
      check_ns_ref(addr+i, 0);
    }
  } else if ( (val&0x00000003) == 0x00000003 ) {
    // frame
    VERB3 printf("NS Frame at %08x (%d bytes = %d entries):\n", addr, size, size/4-3);
    VERB3 printf("  map: %08x\n", rom_w(addr+8));
    for (i=12; i<size; i+=4) {
      VERB3 printf("%5d: %08x\n", i/4-3, rom_w(addr+i));
    }
    for (i=8; i<size; i+=4) {
      check_ns_ref(addr+i, 0);
    }
  } else {
    VERB1 printf("ERROR: unsupported NS object at %08x: %08x\n", addr, val);
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
      VERB1 printf("ERROR: lost track of NS objects at %08x!\n", i);
      ABORT_SCAN;
    }
  }
  
}

// Dictionaries starting at 0x006853DC (InitROMDictionaryData)


void writeNewtonROMTexts()
{  
  printf("\n====> Writing all Newton ROM ASCII text entries\n\n");
  unsigned int i;
  FILE *text = fopen("/Users/matt/dev/Albert/data/text.txt", "wb");
  if (!text) {
    puts("Can't write text!");
  } else {
    int j, n = 0;
    for (i=0; i<0x00800000; i+=4) {
      if (rom_flags_type(i)==flags_type_arm_text) {
        for (j=0; j<4; j++) {
          if (n==0) {
            fprintf(text, "0x%08x: \"", i+j);
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
    AsmPrintf(newt, "%s:\t 0x%08X: %s\n", psym, i, sym);
  } else if (psym) {
    AsmPrintf(newt, "\n");
    AsmPrintf(newt, "%s:\n", psym);
  } else if (sym) {
    AsmPrintf(newt, "L%08X:\t@ %s\n", i, sym);
  } else if (rom_flags_is_set(i, flags_is_target)) {
    AsmPrintf(newt, "L%08X:\n", i);
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

void writeNewtonROM()
{
  printf("\n====> Writing Newton ROM in ARM assembler code\n\n");
  unsigned int i;
  FILE *newt = fopen("/Users/matt/dev/Albert/NewtonOS/newtonos.s", "wb");
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
      switch (rom_flags_type(i)) {
        case flags_type_unused:
          if (!n_unused) unused_filler = rom_w(i);
          n_unused++; 
          if (  (i+4)>=0x00800000 
              || rom_flags_type(i+4)!=flags_type_unused
              || rom_w(i+4)!=unused_filler) {
            AsmPrintf(newt, "\t.fill\t%d, %d, 0x%08x\n", n_unused, 4, unused_filler);
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
          char buf[256];
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
          if (val) sym = get_symbol_at(val);
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
}


void writeNewtonPseudoC()
{
  printf("\n====> Writing Newton ROM in PseudoC code\n\n");
  unsigned int i;
  FILE *newt = fopen("/Users/matt/dev/Albert/NewtonOS/newtonos.cxx", "wb");
  if (!newt) {
    perror("Can't write NewtonOS!");
  } else {
    
    rom_flags_type(0x003AE204, flags_type_arm_word); // will create illegal instructions!
    rom_flags_type(0x003AE20C, flags_type_arm_word);
    
    unsigned int n_unused = 0, unused_filler = 0;
    fprintf(newt, "//\n// Auto Generated by Albert\n//\n\n");
    for (i=0x002EDE40; i<0x002EE0A8; i+=4) {
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
            AsmPrintf(newt, "\t.fill\t%d, %d, 0x%08x\n", n_unused, 4, unused_filler);
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
          if (d) d->exportCpp(newt);
          char buf[2048];
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
    char buf[256];
    memset(buf, 255, 0);
    disarm(buf, addr, rom_w(addr));
    puts(buf);
  }
}

void writeRExConfigBlock(FILE *f, unsigned int addr) {
  printf("\nRExBlock Entry ('%c%c%c%c', %d bytes at 0x%08x):\n", ROM[addr], ROM[addr+1], ROM[addr+2], ROM[addr+3], rom_w(addr+8), addr+rom_w(addr+4));
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
  printf("offset: 0x%08x\n", rom_w(addr+4));
  printf("length: 0x%08x\n", rom_w(addr+8));
}


void writeREx(const char *filename, unsigned int addr) 
{
  FILE *f = fopen(filename, "wb");
  if (!f) return;
  
  if (::strncmp((char*)(ROM+addr), "RExBlock", 8)!=0) {
    printf("ERRORO: no REx at this address.\n");
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


/**
 * List all functions that are not called by another function.
 */
void listAllEntryPoints() 
{
  printf("=== Call Graph Entry Points ===\n");
  unsigned int addr, count =0;
  for ( addr = 0x00000000; addr < 0x00000020; addr+=4 ) {
    printf("0x%08X: %s\n", addr, get_symbol_at(addr));
    count++;
  }
  printf("  %d entries found (many more here!)\n\n", count);
}


/**
 * List all functions that don't call another function.
 * This delivers many false positives, like virtual base functions and binary data!
 */
void listAllEndPoints() 
{
  printf("=== Call Graph End Points ===\n");
  unsigned int addr, count =0;
  for ( addr = 0x00021438; addr < 0x003AF000; addr+=4 ) {
    const char *sym = get_symbol_at(addr);
    if (sym) {
      AlCallGraphIterator it = callGraph.find(addr);
      if (it == callGraph.end()) {
        printf("0x%08X: %s\n", addr, sym);
        count++;
      }
    }
  }
  printf("  %d entries found\n\n", count);
}

/**
 For every known entry point, list the number of functions it calls.
 */
void writeDepthPerFunction()
{
  FILE *f = fopen("/Users/matt/dev/Albert/calees.txt", "wb");
  printf("=== Call Graph Depth ===\n");
  unsigned int addr;
  for ( addr = 0x00021438; addr < 0x003AF000; addr+=4 ) {
    const char *sym = get_symbol_at(addr);
    if (sym) {
      int n = 0;
#if 0
      multimap<char,int> mymm;
      multimap<char,int>::iterator it;
      pair<multimap<char,int>::iterator,multimap<char,int>::iterator> ret;
      
      mymm.insert(pair<char,int>('a',10));
      mymm.insert(pair<char,int>('b',20));
      mymm.insert(pair<char,int>('b',30));
      mymm.insert(pair<char,int>('b',40));
      mymm.insert(pair<char,int>('c',50));
      mymm.insert(pair<char,int>('c',60));
      mymm.insert(pair<char,int>('d',60));
      cout << "mymm contains:\n";
      for (char ch='a'; ch<='d'; ch++)
      {
        cout << ch << " =>";
        ret = mymm.equal_range(ch);
        for (it=ret.first; it!=ret.second; ++it)
          cout << " " << (*it).second;
        cout << endl;
      }
#endif
      AlCallGraphIterator it;
      std::pair<AlCallGraphIterator, AlCallGraphIterator> ret;
      ret = callGraph.equal_range(addr);
      for (it=ret.first; it!=ret.second; ++it) {
        n++;
      }
      fprintf(f, "%6d 0x%08X: %s\n", n, addr, sym);
    }
  }
  fclose(f);
}

/**
 * Make a list of caller/calee pairs.
 * TODO: it would be nice to find subroutines that have no label!
 */
void createCallGraphInformation() 
{
  unsigned int addr, curr = 0;
  for ( addr = 0x00000000; addr < 0x00000020; addr+=4 ) {
    callGraph.insert(std::make_pair(addr, branch_address_in_ROM(addr)));
  }
  for ( addr = 0x00021438; addr < 0x003AF000; addr+=4 ) {
    if (get_plain_symbol_at(addr)) curr = addr;
    unsigned int cmd = rom_w(addr);
    if ( ((cmd&0x0f000000) == 0x0b000000) || ((cmd&0x0f000000) == 0x0a000000) ) { // branch or jump instruction
      unsigned int dest = branch_address_in_ROM(addr);
      const char *dest_node = get_plain_symbol_at(dest);
      if (dest_node) {
        AlCallGraphIterator it, itlow = callGraph.lower_bound(curr);
        AlCallGraphIterator itup = callGraph.upper_bound(curr);
        for ( it=itlow ; it != itup; it++ ) {
          if ((*it).second==dest)
            break;
        }
        if (it==itup) {
          callGraph.insert(std::make_pair(curr, dest));
          //printf("%s calls %s\n", get_symbol_at(curr), get_symbol_at(dest));
        }
      }
    }
    if ( ((cmd&0x0f000000) == 0x0b000000) ) { // branch subroutine
      unsigned int dest = branch_address_in_ROM(addr);
      const char *dest_node = get_plain_symbol_at(dest);
      if (!dest_node) {
        printf("Unlabeled branch destination from 0x%08X to 0x%08X\n", addr, dest);
      }
    }
  }
}

/**
 * Write a callgraph for a single function. Recursive.
 */
int writeCallGraph(FILE *f, unsigned int start, unsigned int depth) 
{
  // don't descend below a defined depth
  if (depth==0) return 0;
  
  // fix invalid addresses
  if (start>=0x01A00000 && start<0x1D00000) {
    unsigned int next = ( ((start>>5)&0xffffff80) | (start&0x0000007f) ) - 0xCE000;
    start = branch_address(start, rom_w(next));
  }
  if (start>=0x00800000) {
    return 0;
  }

  // did we write this node already?
  if (rom_flags_is_set(start, flags_walked))
    return 2;
  
  // get the plain symbol as a reference point
  const char *node = get_plain_symbol_at(start);
  if (!node) {
    fprintf(f, "  at_%08x [shape=box,label=\"unnamed\"];\n", start);
    return 0;
  }
  
  // create a manageable name with arguments
  char label[1024], *d;
  const char *s = get_symbol_at(start);
  if (!s) s = get_plain_symbol_at(start);
  for (d=label;*s;s++,d++) {
    switch (*s) {
      case ',': *d++ = '\\'; *d = 'n'; break;
      case '(': if (s[1]==')') s++; else { *d++ = '\\'; *d = 'n'; } break;
      case ')': *d++ = '\\'; *d = 'n'; break;
      default: *d = *s;
    }
  }
  *d = 0;
  fprintf(f, "  %s [shape=box,label=\"%s\"];\n", node, label);
  
  // now find all the links
  char has_nodes = 0;
  for (;;) {
    rom_flags_set(start, flags_walked);
    unsigned int cmd = rom_w(start);
    if ( ((cmd&0x0f000000) == 0x0b000000) || ((cmd&0x0f000000) == 0x0a000000) ) { // branch or jump instruction
      unsigned int dest = branch_address(start, cmd);
      const char *dest_node = get_plain_symbol_at(dest);
      if (dest_node) {
        has_nodes = 1;
        writeCallGraph(f, dest, depth-1);
        fprintf(f, "  %s -> %s;\n", node, dest_node);
      }
    }
    start += 4;
    if (get_plain_symbol_at(start)) break;
  }
  if (!has_nodes) {
    fprintf(f, "  %s[style=filled,color=gray];\n", node);
  }
  
  return 1;
}

/**
 * Write a callgraph in Graphviz' dot format.
 */
void writeCallGraph(const char *filename, unsigned int start = 0xffffffff, unsigned int depth = 65536) 
{
  FILE *f = fopen(filename, "wb");
  if (!f) return;
  
  fprintf(f, "## Generated by Albert for NewtonOS\n\nstrict Digraph G {\n");
  writeCallGraph(f, start, depth);
  //  a -> b;  a [label="abc"] -> b; a -> {c, d, e}; node [shape=box,style=filled,color=".7 .3 1.0"];
  fprintf(f, "}\n");
  
  fclose(f);
}




class AlPrecompiler {
  
  int mIndent;
  unsigned int mChunkStart;
  unsigned int mChunkEnd;
  
  static const char *kConditionLUT[];
  
public:
  
  enum {
    CAN_PRECOMPILE = 0,
    WRITE_FUNCTION_START,
    WRITE_FUNCTION_END,
    WRITE
  };
  
  enum {
    RES_MEM = 0x80000000
  };
  
  AlPrecompiler() :
    mIndent(0),
    mChunkStart(0),
    mChunkEnd(0)
  {
  }
  
  const char *indent() {
    static const char *space = "                                ";
    return space + 32 - mIndent;
  }
  
  void indentMore() { mIndent+=2; }
  
  void indentLess() { mIndent-=2; }

  void setStart(unsigned int addr) { mChunkStart = addr; }
  
  void writeFunctionStart(unsigned int addr, unsigned int &res, FILE *f=0L) {
    fprintf(f, "PRECOMPILED_FUNCTION(0x%08X)\n", addr);
    fprintf(f, "{\n");
    if (res & RES_MEM) {
      fprintf(f, "  %sTMemory* theMemoryInterface = ioCPU->GetMemory();\n", indent());
    }
    mChunkStart = addr;
    mIndent = 0;
  }

  void writeFunctionEnd(unsigned int addr, unsigned int &res, FILE *f=0L) {
    //fprintf(f, "  SETPC(0x%08x);\n", addr );
    fprintf(f, "  MMUSMARTCALLNEXT(0x%08x);\n", addr+4 );
    fprintf(f, "}\n\n");
  }
  
  
  unsigned int precompileDataProcessingPSRTransfer(unsigned int addr, unsigned int cmd, unsigned int &res, FILE *f=0L) {
    unsigned int ret = 0;
    /*
     //unsigned int r2 = (cmd & 0x0f);
     unsigned int rd = ((cmd>>12)&0x0f);
     unsigned int rn = ((cmd>>16)&0x0f);
     
     char rm[80]; rm[0] = 0;
     if ( (cmd>>25)&1 ) {
     // [rot:4][imm:8]
     } else {
     // [shift:8][reg:4]
     }
     */
    
    if ( (cmd&0x0fff0ff0) == 0x01A00000 ) { // simple "mov rd, r2"
      unsigned int r2 = (cmd & 0x0f);
      unsigned int rd = ((cmd>>12)&0x0f);
      if (rd<15 && r2<15) {
        if (!f) {
          ret =1;
          res |= ((1<<r2) | (1<<rd));
        } else {
          fprintf(f, "  %sioCPU->mCurrentRegisters[%d] = ioCPU->mCurrentRegisters[%d];\n", indent(), rd, r2);
        }
        return 1;
      }
    }

    unsigned int Rd = ((cmd>>12)&0x0f);
    unsigned int Rn = ((cmd>>16)&0x0f);
    unsigned int Rm = (cmd&0x0f);
    bool theFlagS = (cmd & 0x00100000) != 0;
    
    if (cmd & 0x02000000)
    {
      return 0;
      /*
      KUInt32 theImmValue = inInstruction & 0xFF;
      KUInt32 theRotateAmount = ((inInstruction >> 8) & 0xF) * 2;
      if (theRotateAmount != 0)
      {
        theImmValue = 
				(theImmValue >> theRotateAmount)
				| (theImmValue << (32 - theRotateAmount));
        if (theFlagS)
        {
          theMode = ImmC;
        } else {
          theMode = Imm;
        }
      } else {
        theMode = Imm;
      }
      thePushedValue = theImmValue;
       */
    } else if ((cmd & 0x00000FFF) >> 4) {
      return 0;
      /*
      theMode = Regular;
      thePushedValue = inInstruction;
       */
    } else {
      //theMode = NoShift;
      //thePushedValue = __Rm;
    }

    switch ((cmd & 0x01E00000) >> 21) {
      case 0x9:	// TEQ
        if (theFlagS == 0)
        {
          return 0;
          /*
           if (theMode != NoShift)
           {
           // Undefined Instruction (there is no MRS with Imm bit set or low bits set)
           PUSHFUNC(UndefinedInstruction);
           doPush = false;
           doPushPC = true;
           } else {
           PUSHFUNC(MRS_Func(1, __Rd));
           doPush = false;
           }
           */
        } else {
          // "teq..."
          if (!f) {
            ret = 1;
          } else {
            fprintf(f, "  %s{\n", indent());
            indentMore();
            fprintf(f, "  %sKUInt32 Opnd2 = ioCPU->mCurrentRegisters[%d];\n", indent(), Rm);
            fprintf(f, "  %sKUInt32 Opnd1 = ioCPU->mCurrentRegisters[%d];\n", indent(), Rn);
            fprintf(f, "  %sconst KUInt32 theResult = Opnd1 ^ Opnd2;\n", indent());
            //fprintf(f, "  %sconst KUInt32 Negative1 = Opnd1 & 0x80000000;\n", indent());
            //fprintf(f, "  %sconst KUInt32 Negative2 = Opnd2 & 0x80000000;\n", indent());
            fprintf(f, "  %sconst KUInt32 NegativeR = theResult & 0x80000000;\n", indent());
            //fprintf(f, "  %sioCPU->mCPSR_C = (Negative1 && !Negative2) || (Negative1 && !NegativeR) || (!Negative2 && !NegativeR);\n", indent());
            //fprintf(f, "  %sioCPU->mCPSR_V = (Negative1 && !Negative2 && !NegativeR) || (!Negative1 && Negative2 && NegativeR);\n", indent());
            fprintf(f, "  %sif (theResult == 0) {\n", indent());
            fprintf(f, "  %s  ioCPU->mCPSR_Z = true;\n", indent());
            fprintf(f, "  %s  ioCPU->mCPSR_N = false;\n", indent());
            fprintf(f, "  %s} else {\n", indent());
            fprintf(f, "  %s  ioCPU->mCPSR_Z = false;\n", indent());
            fprintf(f, "  %s  if (NegativeR) {\n", indent());
            fprintf(f, "  %s    ioCPU->mCPSR_N = true;\n", indent());
            fprintf(f, "  %s  } else {\n", indent());
            fprintf(f, "  %s    ioCPU->mCPSR_N = false;\n", indent());
            fprintf(f, "  %s  }\n", indent());
            fprintf(f, "  %s}\n", indent());
            indentLess();
            fprintf(f, "  %s}\n", indent());
          }
        }
        break;
      case 0xA:	// 0b1010
                // MRS (SPSR) & CMP
        if (theFlagS == 0)
        {
          return 0;
          /*
          if (theMode != NoShift)
          {
            // Undefined Instruction (there is no MRS with Imm bit set or low bits set)
            PUSHFUNC(UndefinedInstruction);
            doPush = false;
            doPushPC = true;
          } else {
            PUSHFUNC(MRS_Func(1, __Rd));
            doPush = false;
          }
           */
        } else {
          // "cmp..."
          if (!f) {
            ret = 1;
          } else {
            fprintf(f, "  %s{\n", indent());
            indentMore();
            fprintf(f, "  %sKUInt32 Opnd2 = ioCPU->mCurrentRegisters[%d];\n", indent(), Rm);
            fprintf(f, "  %sKUInt32 Opnd1 = ioCPU->mCurrentRegisters[%d];\n", indent(), Rn);
            fprintf(f, "  %sconst KUInt32 theResult = Opnd1 - Opnd2;\n", indent());
            fprintf(f, "  %sconst KUInt32 Negative1 = Opnd1 & 0x80000000;\n", indent());
            fprintf(f, "  %sconst KUInt32 Negative2 = Opnd2 & 0x80000000;\n", indent());
            fprintf(f, "  %sconst KUInt32 NegativeR = theResult & 0x80000000;\n", indent());
            fprintf(f, "  %sioCPU->mCPSR_C = (Negative1 && !Negative2) || (Negative1 && !NegativeR) || (!Negative2 && !NegativeR);\n", indent());
            fprintf(f, "  %sioCPU->mCPSR_V = (Negative1 && !Negative2 && !NegativeR) || (!Negative1 && Negative2 && NegativeR);\n", indent());
            fprintf(f, "  %sif (theResult == 0) {\n", indent());
            fprintf(f, "  %s  ioCPU->mCPSR_Z = true;\n", indent());
            fprintf(f, "  %s  ioCPU->mCPSR_N = false;\n", indent());
            fprintf(f, "  %s} else {\n", indent());
            fprintf(f, "  %s  ioCPU->mCPSR_Z = false;\n", indent());
            fprintf(f, "  %s  if (NegativeR) {\n", indent());
            fprintf(f, "  %s    ioCPU->mCPSR_N = true;\n", indent());
            fprintf(f, "  %s  } else {\n", indent());
            fprintf(f, "  %s    ioCPU->mCPSR_N = false;\n", indent());
            fprintf(f, "  %s  }\n", indent());
            fprintf(f, "  %s}\n", indent());
            indentLess();
            fprintf(f, "  %s}\n", indent());
          }
        }
        break;
      default:
        return 0;
    }
    
    return 1;
  }
  
  
  unsigned int precompile00(unsigned int addr, unsigned int &res, FILE *f=0L) {
    unsigned int ret = 0;
    unsigned int cmd = rom_w(addr);
    
    if ((cmd & 0x020000F0) == 0x90)	
    {
      if (cmd & 0x01000000)
      {
        // Single Data Swap
        return 0;
      } else {
        // Multiply
        return 0;
      }
    } else {
      ret = precompileDataProcessingPSRTransfer(addr, cmd, res, f);
    }

    return ret;
  }
  
  
  unsigned int precompileSingleDataTransfer(unsigned int addr, unsigned int cmd, unsigned int &res, FILE *f=0L) {
    unsigned int ret = 0;

    if ( (cmd&0x0fff0000) == 0x059F0000 ) { // "ldr rd, r15+n", positive offset only
      unsigned int rd = ((cmd>>12)&0x0f);
      unsigned int src = (cmd & 0xfff) + addr + 8;
      if (src<0x00800000) {
        if (!f) {
          ret = 1;
          res |= rd;
        } else {
          unsigned int w = rom_w(src);
          fprintf(f, "  %sioCPU->mCurrentRegisters[%d] = 0x%08x; // '%s' %d\n", indent(), rd, w, four_cc(w), w);
        }
        return ret;
      }
    }

    unsigned int Rd = ((cmd>>12)&0x0f);
    unsigned int Rn = ((cmd>>16)&0x0f);
    unsigned int offset = 0;
    
    bool FLAG_I = ((cmd & 0x02000000) != 0);
    bool FLAG_P = ((cmd & 0x01000000) != 0);
    bool FLAG_U = ((cmd & 0x00800000) != 0);
    bool FLAG_B = ((cmd & 0x00400000) != 0);
    bool FLAG_W = ((cmd & 0x00200000) != 0);
    bool FLAG_L = ((cmd & 0x00100000) != 0);
    
    bool WRITEBACK = (!FLAG_P || FLAG_W);
    bool UNPRIVILEDGED = (!FLAG_P && FLAG_W);
    
    // TODO: move this into the resources flags
    if (f) {
      fprintf(f, "  %s{ // use local storage\n", indent()); 
      indentMore();
    }
    
    /*
    KUInt32 offset;
     */
    
    if (FLAG_I) {
      return 0;
      /*
      if ((theInstruction & 0x00000FFF) >> 4)
      {
        // Shift.
        // PC should not be used as Rm.
        offset = GetShiftNoCarryNoR15( theInstruction, ioCPU->mCurrentRegisters, ioCPU->mCPSR_C );
      } else {
        offset = ioCPU->mCurrentRegisters[theInstruction & 0x0000000F];
      }
       */
    } else {
      // Immediate
      offset = cmd & 0x00000FFF;
    }
    
    if (Rn == 15) {
      return 0;
      /*
      KUInt32 theAddress = GETPC();
       */
    } else {
      if (f) fprintf(f, "  %sKUInt32 theAddress = ioCPU->mCurrentRegisters[%d];\n", indent(), Rn);
    }
    
    if (FLAG_P) {
      if (FLAG_U) {
        // Add.
        if (f && offset!=0) fprintf(f, "  %stheAddress += %d;\n", indent(), offset);
      } else {
        // Substract.
        if (f && offset!=0) fprintf(f, "  %stheAddress -= %d;\n", indent(), offset);
      }
    }
    
    if (UNPRIVILEDGED) {
      if (f) {
        fprintf(f, "  %sif (ioCPU->GetMode() != TARMProcessor::kUserMode) {\n", indent());
        fprintf(f, "  %s  theMemoryInterface->SetPrivilege( false );\n", indent());
        fprintf(f, "  %s}\n", indent());
      }
    }
    
    if (FLAG_L) {
      // Load.
      if (FLAG_B) {
        return 0;
        /*
        // Byte
        KUInt8 theData;
        if (theMemoryInterface->ReadB( theAddress, theData ))
        {
          // No need to restore mMemory->SetPrivilege since
          // we'll access memory in privilege mode from now anyway.
          SETPC(GETPC());
          ioCPU->DataAbort();
          MMUCALLNEXT_AFTERSETPC;
        }
         */
      } else {
        // Word
        if (f) {
          fprintf(f, "  %sKUInt32 theData;\n", indent());
          fprintf(f, "  %sif (theMemoryInterface->Read( theAddress, theData )) {\n", indent());
          fprintf(f, "  %s  SETPC(0x%08x);\n", indent(), addr+8);
          fprintf(f, "  %s  ioCPU->DataAbort();\n", indent());
          fprintf(f, "  %s  MMUCALLNEXT_AFTERSETPC;\n", indent());
          fprintf(f, "  %s}\n", indent());
        }
      }
      
      if (Rd == 15) {
        return 0;
        /*
        if (FLAG_B) {
          // UNPREDICTABLE
        } else {
          // Clear high bits if required.
          // +4 for PREFETCH
          SETPC(theData + 4);
        }
         */
      } else {
        // Clear high bits if required.
        if (f) fprintf(f, "  %sioCPU->mCurrentRegisters[%d] = theData;\n", indent(), Rd);
      }
    } else {
      // Store.
      
      // If PC is Rd, the stored value is +12 instead of +8
      if (Rd == 15) {
        return 0;
        /*
        KUInt32 theValue = GETPC() + 4;
         */
      } else {
        if (f) fprintf(f, "  %sKUInt32 theValue = ioCPU->mCurrentRegisters[%d];\n", indent(), Rd);
      }
      
      if (FLAG_B) {
        return 0;
        /*
        // Byte
        if (theMemoryInterface->WriteB(theAddress, (KUInt8) (theValue & 0xFF) ))
        {
          SETPC(GETPC());
          ioCPU->DataAbort();
          MMUCALLNEXT_AFTERSETPC;
        }
         */
      } else {
        // Word.
        if (f) {
          fprintf(f, "  %sif (theMemoryInterface->Write( theAddress, theValue )) {\n", indent());
          fprintf(f, "  %s  SETPC(0x%08x);\n", indent(), addr+8);
          fprintf(f, "  %s  ioCPU->DataAbort();\n", indent());
          fprintf(f, "  %s  MMUCALLNEXT_AFTERSETPC;\n", indent());
          fprintf(f, "  %s}\n", indent());
        }
      }
    }
    
    if (WRITEBACK) {
      // Store the address to the base register.
      if (Rn == 15) {
        return 0; // UNPREDICTABLE CASE
      } else {
        if (!FLAG_P) {
          if (FLAG_U) {
            // Add.
            if (f) fprintf(f, "  %sioCPU->mCurrentRegisters[%d] = theAddress + %d;\n", indent(), Rn, offset);
          } else {
            // Substract.
            if (f) fprintf(f, "  %sioCPU->mCurrentRegisters[%d] = theAddress - %d;\n", indent(), Rn, offset);
          }
        } else {
          if (f) fprintf(f, "  %sioCPU->mCurrentRegisters[%d] = theAddress;\n", indent(), Rn);
        }
      }
    }
    
    if (UNPRIVILEDGED) {
      if (f) {
        fprintf(f, "  %sif (ioCPU->GetMode() != TARMProcessor::kUserMode) {\n", indent());
        fprintf(f, "  %s  theMemoryInterface->SetPrivilege( true );\n", indent());
        fprintf(f, "  %s}\n", indent());
      }
    }
    
    /*
    if ((Rd == 15) && FLAG_L) {
      FURTHERCALLNEXT_AFTERSETPC;
    } else {
      CALLNEXTUNIT;
    }
     */
    
    if (f) {
      indentLess();
      fprintf(f, "  %s}\n", indent());
    }

    res |= RES_MEM;
    return 1;
  }
  
  unsigned int precompile01(unsigned int addr, unsigned int &res, FILE *f=0L) {
    unsigned int ret = 0;
    unsigned int cmd = rom_w(addr);
    
    if ((cmd & 0x02000010) == 0x02000010) {
      ret = 0; // undefined instruction
    } else {
      ret = precompileSingleDataTransfer(addr, cmd, res, f);
    }
    
    return ret;
  }
  
  
  unsigned int precompileBranch(unsigned int addr, unsigned int cmd, unsigned int &res, FILE *f=0L) {
    if (addr==0x000D9954) 
      return 0;
    unsigned int ret = 0;
    unsigned int target = branch_address(addr, cmd);
    if ( target>=mChunkStart && target<addr ) {
      if (!f) {
        res |= 0x40000000;
        ret = 1;
      } else {
        fprintf(f, "  %sgoto L%08X;\n", indent(), target);
      }
    }
    return ret;
  }

  
  unsigned int precompile10(unsigned int addr, unsigned int &res, FILE *f=0L) {
    unsigned int ret = 0;
    unsigned int cmd = rom_w(addr);
    if ( (cmd&0x0f000000)==0x0a000000 ) {
      ret = precompileBranch(addr, cmd, res, f);
    }
    return ret;
  }
  
  unsigned int precompile11(unsigned int addr, unsigned int &res, FILE *f=0L) {
    unsigned int ret = 0;
    //unsigned int cmd = rom_w(addr);
    return ret;
  }
  
  unsigned int precompile(unsigned int addr, unsigned int &res, FILE *f=0L) {
    unsigned int ret = 0;
    if (f) fprintf(f, "L%08X:\n", addr);
    if (rom_flags_type(addr)==flags_type_arm_code) {
      unsigned int cmd = rom_w(addr);
      if ( (cmd&0xf0000000) == 0xf0000000 ) { // "never"
        // this group of commands is not used and not supported
      } else {
        if ( (cmd&0xf0000000) != 0xe0000000 ) {
          // print condition code start
          if (f) {
            fprintf(f, "  %sif (ioCPU->Test%s()) {\n", indent(), kConditionLUT[cmd>>28]);
            indentMore();
          }
        }
        switch ( (cmd>>26) & 0x03 ) {
          case 0:
            ret = precompile00(addr, res, f);
            break;
          case 1:
            ret = precompile01(addr, res, f);
            break;
          case 2:
            ret = precompile10(addr, res, f);
            break;
          case 3:
            ret = precompile11(addr, res, f);
            break;
        }
        if ( (cmd&0xf0000000) != 0xe0000000 ) {
          // print condition code end
          if (f) {
            indentLess();
            fprintf(f, "  %s}\n", indent());
          }
        }
      }
    }
    return ret;
  }
  
};

const char *AlPrecompiler::kConditionLUT[] = {
  "EQ", "NE", "CS", "CC", "MI", "PL", "VS", "VC",
  "HI", "LS", "GE", "LT", "GT", "LE", "AL", "NV"
};



/**
 * Convert the data base into a bunch of (pseudo) C and C++ source files
 */
int main(int argc, char **argv) 
{
  int i;
  
  char buf[1024];
  getcwd(buf, 1024);
  puts(buf);
  
  FILE *rom = fopen("/Users/matt/dev/Albert/data/717006", "rb");
  if (!rom) {
    puts("Can't read ROM!");
    return -1;
  }
  fread(ROM, 0x00800000, 1, rom);
  fclose(rom);
  
  readSymbols(
              "/Users/matt/dev/Albert/data/717006.cppsymbols.txt",
              "/Users/matt/dev/Albert/data/717006.symbols.txt"              
              );
  
  {
    AlDatabase test("/Users/matt/dev/Albert/test.txt", "wb");
    gMemoryMap.write(test, true);
  }
  {
    AlDatabase test("/Users/matt/dev/Albert/type.txt", "wb");
    gTypeList.write(test, true);
  }

  {
    //AlDatabase test("/Users/matt/dev/Albert/test.txt", "rb");
    //test.read();
  }
    
#if 0
  writeSymbolStatistics("/Users/matt/dev/Albert/symbysize.txt");
#endif

#if 1
  memset(ROM_flags, 0, 0x00200000*sizeof(int));
#else
  FILE *rom_flags = fopen("/Users/matt/dev/Albert/data/flags", "rb");
  if (!rom_flags) {
    puts("Can't read ROM flags, assuming zero!");
    memset(ROM_flags, 0, 0x00200000*sizeof(int));
  } else {
    fread(ROM_flags, 0x00200000, sizeof(unsigned int), rom_flags);
    fclose(rom_flags);
  }
#endif
  
  load_db(db_path, "symbols.txt");
  
  preset_rom_use();

#if 1
  printf("\n====> Checking Newton ROM use for ARM assembler code\n\n");
  try {
    check_all_code_coverage();
  } catch(char *err) {
    puts(err);
  }
#endif
  
#if 1
  printf("\n====> Checking Newton ROM use for NewtonScript code\n\n");
  try {
    check_all_ns_coverage();
  } catch(char *err) {
    puts(err);
  }
#endif
  
#if 0
  printf("\n====> Writing Newton ROM in pseudo C++ code (C++ methods only)\n\n");
  AlClass::write_all(cpp_path);
  
  FILE *f;
  
  f = fopen("/Users/matt/dev/Albert/src/strings.h", "wb");
  // FIXME: add headers
  AlCString::write_all_h(f);
  fclose(f);
  
  f = fopen("/Users/matt/dev/Albert/src/strings.c", "wb");
  // FIXME add includes
  AlCString::write_all_c(f);
  fclose(f);

#endif
  
#if 0
  printf("\n====> Writing binary Newton ROM usage map\n\n");
  FILE *rom_flags = fopen("/Users/matt/dev/Albert/data/flags", "wb");
  if (!rom_flags) {
    puts("Can't write ROM flags!");
  }
  fwrite(ROM_flags, 0x00200000, sizeof(int), rom_flags);
  fclose(rom_flags);
#endif
  
#if 0
  printf("\n====> Writing Newton ROM usage map as RGB pixel stream\n\n");
  rom_flags = fopen("/Users/matt/dev/Albert/data/flags.rgb", "wb");
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
#endif

#if 0
  printf("\n====> Writing Newton ROM layout per word\n\n");
  FILE *code = fopen("/Users/matt/dev/Albert/data/code.a", "wb");
  if (!code) {
    puts("Can't write code!");
  } else {
    for (i=0; i<0x00800000; i+=4) {
      const char *type = "";
      char buf[1024]; memset(buf, 0, 1024);
      switch (rom_flags_type(i)) {
        case flags_type_unknown:    type="unknown"; break;
        case flags_type_arm_code:   type="arm_code"; break;
        case flags_type_arm_byte:   type="arm_byte"; break;
        case flags_type_arm_word:   type="arm_word"; break;
        case flags_type_arm_text:   type="arm_text"; break;
        case flags_type_patch_table:type="patch_table"; break;
        case flags_type_jump_table: type="jump_table"; break;
        case flags_type_unused:     type="unused"; break;
        case flags_type_rex:        type="rex"; break;
        case flags_type_ns:         type="NS"; break;
        case flags_type_ns_obj:     type="NSObj"; break;
        case flags_type_ns_ref:     type="NSRef"; break;
        case flags_type_dict:       type="dict"; break;
        case flags_type_classinfo:  type="Class"; break;
        case flags_type_data:       type="data"; break;
        default:                    type="???"; break; 
      }
      sprintf(buf, "%s                  ", type);
      if (buf[0]!=' ') {
        const char *sym = get_symbol_at(i);
        if (sym) 
          fprintf(code, "            %s:\n", sym);
        else if (rom_flags_is_set(i, flags_is_target)) 
          fprintf(code, "            L%08X:\n", i);
        disarm(buf+16, i, rom_w(i));
        fprintf(code, "%s\n", buf);
      }
    }
    fclose(code);
  }
#endif

#if 0
  writeNewtonROMTexts();
#endif
  
#if 0
  writeNewtonROM();
#endif
  
#if 1
  writeNewtonPseudoC();
#endif
  
#if 0
  extractStencils();
#endif
  
#if 0
  int n=0;
  for (i=0; i<0x00200000; i++) {
    if (ROM_flags[i]) n++;
  }
  printf("\n====================\n");
  printf("%7.3f%% of ROM words covered (%d of 2097152)\n", n/20971.52, n);
  printf("====================\n\n");
#endif
    
#if 0
  {
    FILE *f = fopen("/Users/matt/dev/Albert/fn.cpp", "wb");
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
  
#if 0
  writeREx("/Users/matt/dev/Albert/NewtonOS/apple.rex.s", 0x0071FC4C);
#endif
  
  createCallGraphInformation();
  writeDepthPerFunction();
#if 0
  FILE *f = fopen("/Users/matt/dev/Albert/DataAbortHandler.graph", "wb");
  writeCallGraph(f, 0x00393114, 9999);
  fclose(f);
#endif
#if 0
  listAllEntryPoints();
  listAllEndPoints();
#endif 
  
  //writeCallGraph("/Users/matt/dev/Albert/callgraph.dot", 0x000466CC /* CBufferList::DeleteLast(void) */, 8 );
  //writeCallGraph("/Users/matt/dev/Albert/callgraph.dot", 0x0004590C, 8 );
  //writeCallGraph("/Users/matt/dev/Albert/callgraph.dot", 0, 6);
  
  
#if 0
  {
    AlPrecompiler pre;
    unsigned int i, j, k, longestChunk = 0, precompiledChunks = 0; 
    const unsigned int MIN_PREC_SIZE = 4;
    // 3: 7050
    // 4: 6990 16100
    // 5:
    // 6: 7030
    // X: 7040 16000
    
    printf("Precompiling chunks of ROM\n");
    FILE *f = fopen("/Users/matt/dev/Einstein/Emulator/ROM/PrecompiledPatches.cp", "wb");
    fprintf(f, "//\n// Precompiled code chunks\n//\n\n");
    fprintf(f, "#include <K/Defines/KDefinitions.h>\n#include \"PrecompiledPatches.h\"\n\n");
    
    // search 
    for (i=0; i<0x00800000; i+=4) {
      unsigned int resources = 0;
      pre.setStart(i);
      for (j=i; j<0x00800000; j+=4) {
        if (!pre.precompile(j, resources)) 
          break;
      }
      // FIXME: avoid crossing precompilation into another function!
      if ( (j > i+4*MIN_PREC_SIZE) /*|| (resources & 0x40000000)*/ ) {
        pre.writeFunctionStart(i, resources, f);
        for (k=i; k<j; k+=4) {
          pre.precompile(k, resources, f);
        }
        pre.writeFunctionEnd(k, resources, f);
        precompiledChunks++;
        if (j-i>longestChunk) longestChunk = j-i;
        i = j+4;
      }
    }
    
    fprintf(f, "\n\n//\n// End of precompiled code chunks\n//\n");
    fclose(f);
    
    printf("Precompiled %d chunks with a maximum of %d commands\n", precompiledChunks, longestChunk/4);
  }
#endif 
  
  return 0;
}

/*
:map <f1> kdwdwj$p052xi  rom_flags_type(<esc>11li, flags_type_arm_word); //  <esc>19xkddjj0
:map <f2> 0df;xi  rom_flags_type(<esc>11li, flags_type_arm_word); // <esc>j0
:map <f3> 0df;xi  rom_flags_type(<esc>11li, flags_type_arm_text); // <esc>j0
:map <f4> 0df;xi  check_code_coverage(<esc>11li); // <esc>j0
:map <f5> ma0k12lywj$p0df;xi  rom_flags_type(<esc>11li, , flags_type_data); // <esc>j0/:<cr>
:map <f6> kmbjjf;llyw'af,lpj0V'bd/unknown<cr>
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
 
 */
