

#include "symbols.h"
#include "main.h"
#include "asmview.h"
#include "dyne.h"

#include <stdio.h>
#include <string.h>

#include <string>
#include <map>

#ifdef WIN32
# include <direct.h>
#else
# include <sys/types.h>
# include <sys/stat.h>
#endif

char buf[1026];

typedef struct {
  FILE *h, *c;
  char *hn, *cn;
  bool is_c;
} Files;

typedef std::map < std::string, Files* > FileList;

FileList fileList;

static void newFiles(const char *path, const char *name, bool is_c=false) 
{
  char buf[1024];
  Files *f = new Files;
  f->is_c = is_c;
  fileList.insert(std::make_pair(name, f));
  // start the header file
  sprintf(buf, "%s/%s.h", path, name);
  f->hn = strdup(buf);
  f->h = fopen(buf, "wb");
  if (!f->h) {
    printf("ERR: can't open header %s\n", buf);
  }
  fprintf(f->h, "\n#ifndef %s_H\n#define %s_H\n\n", name, name);
  fprintf(f->h, "#include \"native.h\"\n\n");
  if (!is_c) {
    fprintf(f->h, "class %s\n{\npublic:\n", name);
  }
  fclose(f->h);
  // start the c++ file
  sprintf(buf, "%s/%s.cxx", path, name);
  f->cn = strdup(buf);
  f->c = fopen(buf, "wb");
  if (!f->c) {
    printf("ERR: can't open src %s\n", buf);
  }
  fprintf(f->c, "\n#include \"%s.h\"\n\n", name);
  fclose(f->c);
}

void closeAllFiles() 
{
  FileList::iterator i = fileList.begin();
  while (i!=fileList.end()) {
    Files *f = i->second;
    FILE *h = fopen(f->hn, "ab");
    if (!i->second->is_c) {
      fprintf(h, "};\n");
    }
    fprintf(h, "\n\n#endif\n\n");
    fclose(h);
    FILE *c = fopen(f->cn, "ab");
    fprintf(c, "\n\n");
    fclose(c);
    i++;
  }
}

static void add_cxx_line(const char *path, char *cl, char *nm, unsigned int start, unsigned int end) 
{
  FileList::iterator i = fileList.find(cl);
  if (i==fileList.end()) {
    newFiles(path, cl);
    i = fileList.find(cl);
  }
  Files *f = i->second;
  // put the 'extern' definition into the header
  FILE *h = fopen(f->hn, "ab");
  fprintf(h, "  unknown_t %s", nm);
  if (strstr(nm, "(")==0L) 
    fprintf(h, "()");
  fprintf(h, ";\n");
  fclose(h);
  // put the function and body into the source file
  FILE *c = fopen(f->cn, "ab");
  fprintf(c, "\n/*\n * at 0x%08x\n */\nunknown_t %s::%s", start, cl, nm);
  if (strstr(nm, "(")==0L) 
    fprintf(c, "()");
  fprintf(c, "\n{\n");
  wAsmView->save_range_cc(c, start, end);
  fprintf(c, "  return 0;\n");
  fprintf(c, "}\n");
  fclose(c);
}

static void add_c_line(const char *path, char *buf, unsigned int start, unsigned int end) 
{
  FileList::iterator i = fileList.find("base");
  if (i==fileList.end()) return;
  Files *f = i->second;
  // put the 'extern' definition into the header
  FILE *h = fopen(f->hn, "ab");
  fprintf(h, "extern unknown_t %s", buf);
  if (strstr(buf, "(")==0L) 
    fprintf(h, "()");
  fprintf(h, ";\n");
  fclose(h);
  // put the function and body into the source file
  FILE *c = fopen(f->cn, "ab");
  fprintf(c, "\n/*\n * at 0x%08x\n */\nunknown_t %s", start, buf);
  if (strstr(buf, "(")==0L) 
    fprintf(c, "()");
  fprintf(c, "\n{\n");
  wAsmView->save_range_cc(c, start, end);
  fprintf(c, "  return 0;\n");
  fprintf(c, "}\n");
  fclose(c);
}

void desym(const char *pathname)
{
#ifdef WIN32
  mkdir(pathname);
#else
  mkdir(pathname, 0777);
#endif
  newFiles(pathname, "base", true);
  Dn_Symbols::Dn_S_Symbol::iterator i = symbols->list.begin();
  for ( ; i!=symbols->list.end(); ++i) {
    if (i->first>0x00800000) break;
    strcpy(buf, i->second);
    Dn_Symbols::Dn_S_Symbol::iterator j = i; ++j;
    unsigned int start = i->first;
    unsigned int end   = j->first;
    if (strlen(buf)<1) continue;
    char *cc = strstr(buf, "::");
    if (cc) {
      *cc = 0;
      add_cxx_line(pathname, buf, cc+2, start, end);
    } else {
      add_c_line(pathname, buf, start, end);
    }
  }
  closeAllFiles();
}


