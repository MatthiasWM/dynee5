/*
 *  db2src.cpp
 *  Albert
 *
 *  Created by Matthias Melcher on 08.11.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "db2src.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>


const char *db_path = "/Users/matt/dev/Albert/";
const char *src_path = "/Users/matt/dev/Albert/src/"; 
const char *c_path = "/Users/matt/dev/Albert/src/"; 
const char *cpp_path = "/Users/matt/dev/Albert/src/"; 

unsigned char ROM[0x00800000];

class AlClass;
class AlArg;
class AlMemberFunction;


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
      fprintf(f, "   * \\param %s", pSym);
    } else {
      fprintf(f, "   * \\param arg%d", i);
    }
    if (pComment)
      printComment(f, pComment, " ", "   *   ");
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
  char pIsStatic, pIsConst;
public:
  AlMemberFunction(FILE *f) {
    pAt = pNext = 0xffffffff;
    pSym = 0;
    pReturns = 0;
    pComment = 0;
    pClass = 0;
    pArg = 0;
    pnArg = 0;
    pIsStatic = pIsConst = 0;
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
      } else if (strcmp(cmd, "returns")==0) {
        sscanf(s, "%s %[^\n]", cmd, arg);
        if (strcmp(arg, "FIXME")==0) {
          // ctor, dtor and some operators have no return type!
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
    // write the doxygen commentary
    fprintf(f, "  /**\n");
    if (pComment)
      printComment(f, pComment, "   * ");
    else
      fprintf(f, "   * Member function %s of class %s.\n", pSym, pClass->pSym);
    fprintf(f, "   *\n");
    for (i=0; i<pnArg;i++) {
      pArg[i]->write_doxy(f, i);
    }
    if (pReturns) {
      fprintf(f, "   * \\returns %s\n", pReturns);
    }
    fprintf(f, "   */\n");
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
    fprintf(f, ";\n\n");
  }
  void write_cpp(FILE *f) {
    int i;
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
    fprintf(f, "  /* ARM code interpreter output here */\n"); // pAt to pNext
    fprintf(f, "}\n\n");
  }
};


void AlClass::write_h(char const *path) {
  char filename[2048];
  sprintf(filename, "%s%s.h", path, pSym);
  FILE *f = fopen(filename, "wb");
  fprintf(f, "// auto-generated\n\n");
  fprintf(f, "#ifndef %s_H\n#define %s\n\n", pSym, pSym);
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
  fprintf(f, "// auto-generated\n\n");
  fprintf(f, "#include \"%s.h\"\n\n", pSym);
  int i;
  for (i=0; i<pnf; i++) {
    pFn[i]->write_cpp(f);
  }
  fprintf(f, "\n\n");
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
      } else {
        // skip to "end"
        // FIXME printf("WARNING: unsupported class '%s'\n", arg);
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

/**
 * Convert the data base into a bunch of (pseudo) C and C++ source files
 */
int main(int argc, char **argv) 
{
  FILE *rom = fopen("/Users/matt/dev/Albert/data/717006", "rb");
  if (!rom) {
    puts("Can't read ROM!");
    return -1;
  }
  fread(ROM, 0x00800000, 1, rom);
  fclose(rom);

  load_db(db_path, "symbols.txt");
    
  AlClass::write_all(cpp_path);
  
  return 0;
}
