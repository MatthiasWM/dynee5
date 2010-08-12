/*
 *  db2src.cpp
 *  Albert
 *
 *  Created by Matthias Melcher on 08.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#define VERB1
#define VERB2 if (0) 
#define VERB3 if (0) 
#define VERB4 if (0) 
#define VERB5 if (0) 

#include "db2src.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <map>

#define BDISP(x) ((((x) & 0xffffff) ^ 0x800000) - 0x800000) /* 26 bit */

extern int disarm(char *dst, unsigned int addr, unsigned int cmd);
extern int disarm_c(char *dst, unsigned int addr, unsigned int cmd);


const char *db_path = "/Users/matt/dev/Albert/";
const char *src_path = "/Users/matt/dev/Albert/src/"; 
const char *c_path = "/Users/matt/dev/Albert/src/"; 
const char *cpp_path = "/Users/matt/dev/Albert/src/"; 

const unsigned int flags_type_mask          = 0x000000ff;
const unsigned int flags_type_unknown       = 0x00000000;
const unsigned int flags_type_arm_code      = 0x00000001;
const unsigned int flags_type_arm_byte      = 0x00000002;
const unsigned int flags_type_arm_word      = 0x00000003;
const unsigned int flags_type_patch_table   = 0x00000004;
const unsigned int flags_type_jump_table    = 0x00000005;
const unsigned int flags_type_unused        = 0x00000006;
const unsigned int flags_type_rex           = 0x00000007;
const unsigned int flags_type_ns            = 0x00000008;
const unsigned int flags_type_ns_obj        = 0x00000009;
const unsigned int flags_type_ns_ref        = 0x0000000a;
const unsigned int flags_type_dict          = 0x0000000b;

const unsigned int flags_is_target          = 0x10000000;

unsigned char ROM[0x00800000];
unsigned int ROM_flags[0x00200000];

/**
 * Return the 4-byte word in rom
 */
unsigned int rom_w(unsigned int addr)
{
  if (addr>=0x00800000-4)
    return 0;
  else
    return (ROM[addr]<<24)|(ROM[addr+1]<<16)|(ROM[addr+2]<<8)|ROM[addr+3];
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

typedef std::map < unsigned int, const char* > AlSymbolList;
AlSymbolList symbolList;

const char *get_symbol_at(unsigned int addr)
{
  AlSymbolList::iterator s = symbolList.find(addr);
  if (s==symbolList.end()) return 0L;
  return s->second;
}

void readSymbols(const char *filename)
{
  FILE *f = fopen(filename, "rb");
  if (!f) {
    puts("ERROR opening symbol file");
    return;
  }
  for (;;) {
    char buf[80], sym[80];
    unsigned int addr;
    char *s = fgets(buf, 80, f);
    if (!s) break;
    int n = sscanf(s, "0x%08x %[^\n]\n", &addr, sym);
    if (n==2) symbolList.insert(std::make_pair(addr, strdup(sym)));
  }
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
  static void write_all(char const *path) {
    int i;
    for (i=0; i<pn; i++) {
      pClass[i]->write(path);
    }
  }
  void init() {
    pSym = 0;
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
        goto skip; // FIXME:
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

void check_code_coverage(unsigned int addr);

void check_switch_case(unsigned int addr, int n_case) 
{
  int i;
  printf("Switch/Case statement with %d cases starting at %08x\n", n_case, addr);
  for (i=0; i<n_case; i++) { // nuber of cases plus default case
    VERB4 printf("case %d at %08x\n", i, addr+4*i);
    check_code_coverage(addr+4*i);
  }
}

// recursively dive into the ARM code from this point on and follow all possible
// execution paths
void check_code_coverage(unsigned int addr)
{
  // mark this as jump target
  if (addr<0x00800000) rom_flags_set(addr, flags_is_target); // mark this as a jump target

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
        if (prev_sym==0 || addr_sym==0 || strcmp(prev_sym, addr_sym)!=0) {
          VERB2 printf("ERROR: Symbols don't match. Verify lookup table offsets!\n");
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
      sprintf(buf, ":  ", addr);
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
          continue;
        } else { // conditional, follow both branches (finsih-thread-first recursion)
          unsigned int next = branch_address(addr);
          VERB3 printf("%08x: conditional jump to %08x, follow later\n", addr, next);
          check_code_coverage(addr+4);
          VERB3 printf("%08x: following up on conditional jump to %08x\n", addr, next);
          addr = next; 
          continue;
        }
      } else if ( (cmd&0x0f000000) == 0x0b000000) { // branch instruction, follow both branches (dive-first recursion)
        unsigned int next = branch_address(addr);
        VERB3 printf("%08x: subroutine call to %08x, follow later\n", addr, next);
        check_code_coverage(addr+4);
        VERB3 printf("%08x: following up on subroutine call to %08x\n", addr, next);
        addr = next;
        continue;
      } else if ( (cmd&0x0ff0ff00) == 0x0280F000) { // add pc,r#,# (typical virtual call)
        VERB2 printf("Later: Virtual call at %08x: %08x\n", addr, cmd);
        return;
      } else if ( (cmd&0x0db6f000) == 0x0120f000) { // msr command does not modifiy pc
      } else if ( (cmd&0x0c000000) == 0x00000000) { // data processing, only important if pc is changed
        if ( (cmd&0x0000f000) == 0x0000f000) { // is the destination register the pc?
          if ( (cmd&0x0ff0fff0) == 0x01A0F000 && (rom_w(addr-4)&0x0fffffff) == 0x01A0E00F) {
            // mov lr,pc; mov pc,r#
            // this is the  pattern for a function called via an address stored in a register
            // I can't guess the call address, but we should nevertheless continue to check code coverage
            VERB2 printf("Later: Register based call at %08x: %08x\n", addr, cmd);
            addr += 4; continue;
          }
          if ( (cmd&0xfffffff0) == 0x908FF100 && (rom_w(addr-4)&0xfff0fff0) == 0xE3500000) {
            // cmp rx, #n; addls pc, pc, rx lsl 2
            // This is the pattern for a switch/case statement with default clause. A jump table of size n+1 follows.
            int n_case = rom_w(addr-4)&0x0000000f, i;
            VERB3 printf("Switch/Case statement with %d cases at %08x: %08x\n", n_case, addr, cmd);
            addr+=4;
            for (i=0; i<=n_case; i++) { // nuber of cases plus default case
              VERB4 printf("case %d at %08x\n", i-1, addr+4*i);
              check_code_coverage(addr+4*i);
            }
            return;
          }
          if (addr==0x000456ec) { addr+=4; continue; } // FIXME: virtual call?
          if (addr==0x00045700) { addr+=4; continue; } // FIXME: virtual call?
          if (addr==0x00018624) { addr+=4; continue; } // FIXME: virtual call?
          if (addr==0x00019984) { return; } // FIXME: virtual call? fancy return?
          if (addr==0x000198CC) { return; } // FIXME: virtual call? fancy return?
          if (addr==0x000198F4) { return; } // FIXME: virtual call? fancy return?
          if (addr==0x0001991c) { return; } // FIXME: virtual call? fancy return?
          if (addr==0x0001A0EC) { return; } // FIXME: virtual call? fancy return?
          if (addr==0x00019ca0) { return; } // FIXME: virtual call? fancy return?
          if (addr==0x0001a028) { return; } // FIXME: virtual call? fancy return?
          if (addr==0x000D9938) { return; } // FIXME: ??
          if (addr==0x0038fa50) { addr+=4; continue; } // FIXME: ??
          if (addr==0x0038d9a4) { check_switch_case(0x0038d9ac, 33); return; }
          if (addr==0x0038ec98) { check_switch_case(0x0038eca0,  9); return; }
          VERB1 printf("Aborting: Data processing command modifying R15 at %08x: %08x\n", addr, cmd);
          //throw "abort";
          return;
        }          
      } else if ( (cmd&0x0f000000) == 0x0e000000) { // mcr, mrc (FIXME: probably not changing pc)
      } else if ( (cmd&0x0e000010) == 0x06000010) { // unknown (used to trigger interrupt, FIXME: and then?)
        if (cmd==0xE6000510) { return; } // Kernel Panic!
        if (addr==0x00392C2C) { return; } // FIXME: ??
        if (addr==0x000188CC) { addr+=4; continue; } // FIXME: ??
        if (addr==0x003ae188) { return; } // FIXME: ??
        if (addr==0x003ae36c) { return; } // FIXME: ??
        if (addr==0x000188F4) { return; } // FIXME: ??
        if (addr==0x003ae2e4) { return; } // FIXME: ??
        if (addr==0x0038ce6c) { return; } // DebugStr
        if (addr==0x0038ce70) { return; } // Debugger
        if (addr==0x0038ce74) { return; } // ExitToShell
        if (addr==0x0038ce78) { return; } // SendTestResults
        if (addr==0x0038ce7c) { return; } // TapFileCntl
        if (addr==0x0038ce80) { return; } // RawDebugStr
        if (addr==0x0038ce84) { return; } // RawDebugger
        VERB2 printf("Return: opcode 'undefined' found at %08x: %08x\n", addr, cmd);
        return;
      } else if ( (cmd&0x0c100000) == 0x04000000) { // str (store to memory)
      } else if ( (cmd&0x0c100000) == 0x04100000) { // ldr (load from memory)
        if ( (cmd&0x0000f000) == 0x0000f000) { // is the destination register the pc?
          if (addr==0x000188d8) { addr += 4; continue; } // FIXME: this is a long branch to 0x01A6A520
          if (addr==0x000455b4) { addr += 4; continue; } // FIXME: I do not know what this does!
          if (addr==0x0019190C) { return; } // FIXME: 
          if ( (cmd&0xfff0f000) == 0xE590F000 && (rom_w(addr-4)&0xffffffff) == 0xE1A0E00F) {
            // mov lr,pc; ldr pc,r#
            // this is the pattern for a function called via an address pointed to by a register
            // I can't guess the call address, but we should nevertheless continue to check code coverage
            VERB2 printf("Later: Register pointer based call at %08x: %08x\n", addr, cmd);
            addr += 4; continue;
          }
          VERB1 printf("Aborting: LDR command modifying R15 at %08x: %08x\n", addr, cmd);
          //throw "abort";
          return;
        }
      } else if ( (cmd&0x0f000000) == 0x0f000000) { // swi (software interrupt)
      } else if ( (cmd&0x0e000000) == 0x0c000000) { // (coprocessor dat transfer) FIXME: may actuall tfer to pc?!
      } else if ( (cmd&0x0e100000) == 0x08000000) { // stm (store multiple to memory)
      } else if ( (cmd&0x0e100000) == 0x08100000) { // ldm (load from memory)
        if ( (cmd&0x00008000) == 0x00008000) { // is the pc among the destination registers?
                                               // FIXME: we'll assume it's a return command
          if ( (cmd&0xf0000000)==0xe0000000) return; // unconditional - we are done
                                                     // conditional - continue to check
        }
      } else {
        VERB1 printf("Aborting: Hit unknown command at %08x: %08x\n", addr, cmd);
        //throw "abort";
        return;
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
  // 006853dc to 0071a95c dictionaries
  for (i=0x006853dc; i<0x0071a95c; i++ ) {
    rom_flags_type(i, flags_type_dict);
  }
}

static unsigned int db_cpp[] = {
#include "db_cpp.h"
};

void check_all_code_coverage()
{
  int i;
  // system vectors
  check_code_coverage(0x00000000);
  check_code_coverage(0x00000004);
  check_code_coverage(0x00000008);
  check_code_coverage(0x0000000c);
  check_code_coverage(0x00000010);
  check_code_coverage(0x00000014);
  check_code_coverage(0x00000018);
  check_code_coverage(0x0000001c);
  // known entry points (C++ functions)
  for (i=0; i<sizeof(db_cpp)/sizeof(unsigned int); i++) {
    check_code_coverage(db_cpp[i]);
  }
  // ROMBoot unclear positions
  check_code_coverage(0x000188d0);
  // ROMPublicJumpTable
  for (i=0x00013000; i<0x00015e0c; i+=4 ) {
    check_code_coverage(i);
  }  
  // some jump vector table we found. These jumps all go into the ROM patch table
  for (i=0x0001a618; i<0x00021438; i+=4 ) {
    check_code_coverage(i);
  }  
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


void check_ns_ref(unsigned int addr)
{
  if (rom_flags_type(addr)) return;
  rom_flags_type(addr, flags_type_ns_ref);
  unsigned int val = rom_w(addr);
  if ( (val&0xfff0000f) == 0x0000000a ) { // Character
  } else if ( (val&0x00000003) == 0x00000000 ) { // Integer
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
    } else {
      VERB3 printf("NSRef: Special at %08x: %08x\n", addr, val); // 0x32 for functions?
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
    VERB1 printf("ERROR: not an NS object at %08x: %08x\n", addr, val);
    return 0;
  }
  unsigned int i, size = (val&0xffffff00)>>8;
  if (rom_flags_type(addr)) return size;
  rom_flags_type(addr, flags_type_ns_obj);
  rom_flags_type(addr+4, flags_type_ns);
  
  // follow the members of the object
  if ( (val&0x00000003) == 0x00000000 ) {
    // binary object
    VERB3 printf("NS Binary Object at %08x (%d bytes):\n", addr, size);
    check_ns_ref(addr+8);
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
      check_ns_ref(addr+i);
    }
  } else if ( (val&0x00000003) == 0x00000003 ) {
    // frame
    VERB3 printf("NS Frame at %08x (%d bytes = %d entries):\n", addr, size, size/4-3);
    VERB3 printf("  map: %08x\n", rom_w(addr+8));
    for (i=12; i<size; i+=4) {
      VERB3 printf("%5d: %08x\n", i/4-3, rom_w(addr+i));
    }
    for (i=8; i<size; i+=4) {
      check_ns_ref(addr+i);          
    }
  } else {
    VERB1 printf("ERROR: unsupported NS object at %08x: %08x\n", addr, val);
    return 0;
  }
  return size;
}

void check_all_ns_coverage()
{
  int i, n_magic = rom_w(0x003af000);
  for (i=0; i<n_magic; i++) {
    check_ns_ref(0x003af004+4*i);
  }
  // 0x003afda8 gROMSoupData to 0x0067FA40 gROMSoupDataSize
  for (i=0x003afda8; i<0x0067fa40; ) {
    unsigned int size = check_ns_obj(i);
    if (size>0) {
      i+=(size+3)&0xfffffffc; // align to four bytes
    } else {
      VERB1 printf("ERROR: lost track of NS objects at %08x!\n", i);
    }
  }
  // simple pointers, but what are they used for?
  for (i=0x00681c9c; i<0x006853dc; i+=8) {
    check_ns_ref(i);
  }
  // simple pointers, but what are they used for?
  for (i=0x0067fa44; i<0x00681c9c; i+=8) {
    check_ns_ref(i);
  }  
}

// Dictionaries starting at 0x006853DC (InitROMDictionaryData)


/**
 * Convert the data base into a bunch of (pseudo) C and C++ source files
 */
int main(int argc, char **argv) 
{
  int i;
  
  readSymbols("/Users/matt/dev/Albert/data/717006.symbols");
  
  FILE *rom = fopen("/Users/matt/dev/Albert/data/717006", "rb");
  if (!rom) {
    puts("Can't read ROM!");
    return -1;
  }
  fread(ROM, 0x00800000, 1, rom);
  fclose(rom);

#if 1
  FILE *rom_flags;
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
  
#if 1
  preset_rom_use();
  try {
    check_all_code_coverage();
  } catch(char*) {
  }
#endif
  
#if 1
  try {
    check_all_ns_coverage();
  } catch(char*) {
  }
#endif
  
#if 0
  
  AlClass::write_all(cpp_path);
  
  FILE *f;
  
  f = fopen("/Users/matt/dev/Albert/src/strings.h", "wb");
  // FIXME add headers
  AlCString::write_all_h(f);
  fclose(f);
  
  f = fopen("/Users/matt/dev/Albert/src/strings.c", "wb");
  // FIXME add includes
  AlCString::write_all_c(f);
  fclose(f);

#endif
  
#if 0
  rom_flags = fopen("/Users/matt/dev/Albert/data/flags", "wb");
  if (!rom_flags) {
    puts("Can't write ROM flags!");
  }
  fwrite(ROM_flags, 0x00200000, sizeof(int), rom_flags);
  fclose(rom_flags);
#endif
#if 1
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

#if 1
  FILE *code = fopen("/Users/matt/dev/Albert/data/code.txt", "wb");
  if (!code) {
    puts("Can't write code!");
  } else {
    for (i=0; i<0x00800000; i+=4) {
      char buf[1024]; memset(buf, 0, 1024);
      switch (rom_flags_type(i)) {
        case flags_type_unknown:
          strcpy(buf, "unknown         "); break;
        case flags_type_arm_code:
          strcpy(buf, "                "); break;
        case flags_type_arm_word:
          strcpy(buf, "word            "); break;
        case flags_type_arm_byte:
        case flags_type_patch_table:
        case flags_type_jump_table:
        case flags_type_unused:
        case flags_type_rex:
        case flags_type_ns:
        case flags_type_ns_obj:
        case flags_type_ns_ref:
        case flags_type_dict:
        default:
          break;
      }
      if (buf[0]) {
        disarm(buf+16, i, rom_w(i));
        fprintf(code, "%s\n", buf);
      }
    }
    fclose(code);
  }
#endif
  
  int n=0;
  for (i=0; i<0x00200000; i++) {
    if (ROM_flags[i]) n++;
  }
  printf("\n====================\n");
  printf("%7.3f%% of ROM words covered (%d of 2097152)\n", n/20971.52, n);
  printf("====================\n\n");
    
  return 0;
}

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

