


#include "AlCPPMethod.h"
#include "AlTypeList.h"
#include "AlTypeRef.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>



AlCPPMethod::AlCPPMethod()
: pKlass(0L)
{
}


AlCPPMethod::~AlCPPMethod() 
{
  if (pKlass) free(pKlass);
}


bool AlCPPMethod::read(AlDatabase &db)
{
  const char *key = db.currentKey();
  if (strcmp(key, "klass")==0) {
    label(db.currentValue());
  } else {
    return AlCMethod::read(db);
  }
  return true;
}


void AlCPPMethod::write(AlDatabase &db, bool block)
{
  if (block) db.writeBlockBegin("CPPMethod");
  AlCMethod::write(db, false);
  if (klass()) db.write("klass", klass());
  if (block) db.writeBlockEnd();
}


const char *AlCPPMethod::prototype()
{
  static char *buf = 0;
  if (!buf) buf = (char*)malloc(512);
  *buf = 0;
  // return type
  strcat(buf, "t_unknown");
  strcat(buf, " ");
  // class
  if (klass()) {
    strcat(buf, klass());
    strcat(buf, "::");
  }
  // name
  strcat(buf, name());
  strcat(buf, "(");
  // parameters
  strcat(buf, "...");
  strcat(buf, ")");
  // return it
  return buf;
}


const char *AlCPPMethod::klass() 
{
  return pKlass;
}


void AlCPPMethod::klass(const char *l)
{
  if (pKlass)
    free(pKlass);
  if (l)
    pKlass = strdup(l);
  else
    pKlass = 0L;
}


void AlCPPMethod::decodeDecoratedLabel(const char *label)
{
  //printf("Decode %s\n", label);
  
  char buf[256];
  int bra = 0;
  const char *s = label;
  char *d = buf;
  
  // get the first word:
getFirstWord:
  if (isalpha(*s)||*s=='_'||*s=='~') {
    *d++ = *s++;
    for (;;) {
      if (isalnum(*s)||*s=='_') {
        *d++ = *s++;        
      } else {
        *d = 0;
        if (strcmp(buf, "operator")==0) {
          if (*s==' ') { // casting operator
            for (;;) {
              if (isalnum(*s)||*s=='_'||*s==' '||*s=='*') {
                *d++ = *s++;        
              } else 
                break;
            }
          } else if (strncmp(s, "==", 2)==0 || strncmp(s, "<<", 2)==0 || strncmp(s, ">>", 2)==0 || strncmp(s, "[]", 2)==0) {
            *d++ = *s++;
            *d++ = *s++;
          } else if (*s=='=' || *s=='<' || *s=='>') {
            *d++ = *s++;
          } else 
            break;
        } else {
          break;
        }
      }
    }
  }
  *d = 0;
  if (d==buf) {
    fprintf(stderr, "decodeDecoratedLabel: Label starts with unexpected character\n");
  }
  if (s[0]==':' && s[1]==':') {
    // class name is now in the buffer
    //printf("  class: '%s'\n", buf);
    klass(buf);
    // create this type if it isn't yet
    gTypeList.at(klass());
    s+=2;
    d = buf;
    goto getFirstWord; // Oh no! It's a GOTO!
  } else if (s[0]=='(') {
    //printf("  name: '%s'\n", buf);
    name(buf);
    s++;
  } else {
    fprintf(stderr, "decodeDecoratedLabel: Expecting a '('!\n");
  }
  // find out if we are "static" or "const"
  const char *e = s + strlen(s);
  if (strcmp(e-6, "static")==0) {
    setStatic(true);
  } 
  if (strcmp(e-5, "const")==0) {
    setConst(true);
  } 
  // add a type for "this"
//  if (klass() && !isStatic())
//    add(new AlTypeRef(klass(), 1, 0)); // klass, pointer, r0
  // now loop through all the parameters
  d = buf;
  for (;;) {
    char c = *s;
    if (c=='(') {
      bra++;
      *d++ = *s++;
    } else if (c==0) {
      fprintf(stderr, "decodeDecoratedLabel: Unexpected end of parameter list\n");
      break;
    } else if (c==',') {
      if (bra==0) {
        *d = 0;
        addParameter(buf);
        d = buf; s++;
      } else {
        *d++ = *s++;
      }
    } else if (c==')') {
      if (bra==0) {
        *d = 0;
        addParameter(buf);
        break;
      } else {
        bra--;
        *d++ = *s++;
      }
    } else {
      *d++ = *s++;
    }
  }
}


bool AlCPPMethod::isCPPLabel(const char *label)
{
  if (strchr(label, '('))
    return true;
  else
    return false;
}


void AlCPPMethod::exportAsm(FILE *f)
{
  AlCMethod::exportAsm(f);
  if (pKlass) fprintf(f, "\t@ class = '%s'\n", pKlass);
}


char *GetParam(int i) {
  static char buf[80];
  if (i<4) {
    sprintf(buf, "r%d", i);
  } else {
    sprintf(buf, "frame[%d]", i-4);
  }
  return buf;
}

void AlCPPMethod::exportCppCall(FILE *f)
{
  int i;
  //  fprintf(f, "EXPORT AlCPPMethod\n");
  //  AlCMethod::exportAsm(f);
  //  if (pKlass) fprintf(f, "class = '%s'\n", pKlass);
  
  fprintf(f, "\n#if 1\n\n");
  
  if (pKlass) {
    fprintf(f, "KUInt32 %s::%s(", klass(), name());
    for (i=1; i<nTypeRef;i++) {
      pTypeRef[i]->exportCppCall(f);
      fprintf(f, " %s", GetParam(i));
      if (i<nTypeRef-1) fprintf(f, ", ");
    }
    // all args
    fprintf(f, ") {\n");
    fprintf(f, "  KUInt32 ret;\n");
    fprintf(f, "  NEWT_PUSH_REGISTERS\n");
    // all args
    fprintf(f, "  gCurrentCPU->SetRegister(0, (KUInt32)this);\n");
    for (i=1; i<nTypeRef;i++) {
      fprintf(f, "  gCurrentCPU->SetRegister(%d, (KUInt32)%s);\n", i, GetParam(i));
    }
    fprintf(f, "  NewtCallJIT(0x%08X);\n", address());
    fprintf(f, "  ret = gCurrentCPU->GetRegister(0);\n");
    fprintf(f, "  NEWT_POP_REGISTERS\n");
    fprintf(f, "  return ret;\n");
    fprintf(f, "}\n\n");
  } else {
    fprintf(f, "KUInt32 %s(", name());
    for (i=0; i<nTypeRef;i++) {
      pTypeRef[i]->exportCppCall(f);
      fprintf(f, " %s", GetParam(i));
      if (i<nTypeRef-1) fprintf(f, ", ");
    }
    // all args
    fprintf(f, ") {\n");
    fprintf(f, "  KUInt32 ret;\n");
    fprintf(f, "  NEWT_PUSH_REGISTERS\n");
    // all args
    for (i=0; i<nTypeRef;i++) {
      fprintf(f, "  gCurrentCPU->SetRegister(%d, (KUInt32)%s);\n", i, GetParam(i));
    }
    fprintf(f, "  NewtCallJIT(0x%08X);\n", address());
    fprintf(f, "  ret = gCurrentCPU->GetRegister(0);\n");
    fprintf(f, "  NEWT_POP_REGISTERS\n");
    fprintf(f, "  return ret;\n");
    fprintf(f, "}\n\n");
  }
  
  fprintf(f, "#else\n\n");

  if (pKlass) {
    fprintf(f, "KUInt32 %s::%s(", klass(), name());
    for (i=1; i<nTypeRef;i++) {
      pTypeRef[i]->exportCppCall(f);
      fprintf(f, " in%d", i);
      if (i<nTypeRef-1) fprintf(f, ", ");
    }
  } else {
    fprintf(f, "KUInt32 %s(", name());
    for (i=0; i<nTypeRef;i++) {
      pTypeRef[i]->exportCppCall(f);
      fprintf(f, " in%d", i);
      if (i<nTypeRef-1) fprintf(f, ", ");
    }
  }
  fprintf(f, ") {\n");
  
  //  fprintf(f, "  KUInt32 r0, r1, r2, r3, r4, r5, r6, r7, r8, r9;\n\n");
  int start = 0;
  fprintf(f, "  KUInt32 tmp, ");
  if (pKlass) {
    fprintf(f, "r0 = (KUInt32)this, ");
    start++;
  }
  for (i=start; i<nTypeRef;i++) {
    fprintf(f, "r%d = (KUInt32)in%d, ", i, i);
  }
  for (; i<=11;i++) {
    fprintf(f, "r%d, ", i);
  }
  fprintf(f, "r12;\n");
  fprintf(f, "  bool cond_z, cond_c, cond_v, cond_n;\n\n");
}

void AlCPPMethod::exportCppCallEnd(FILE *f)
{
  int i;

  fprintf(f, "return r0;\n}\n\n");
  
  if (pKlass) {
    fprintf(f, "NEWT_INJECTION(0x%08X, \"%s::%s(...)\") {\n",
            address(), klass(), name());
    fprintf(f, "  NEWT_RETVAL NEWT_CLASS(%s)->%s(", klass(), name());
    for (i=1; i<nTypeRef;i++) {
      fprintf(f, "NEWT_ARG%d(", i);
      pTypeRef[i]->exportCppCall(f);
      fprintf(f, ")");
      if (i<nTypeRef-1) fprintf(f, ", ");
    }
    fprintf(f, ");\n");
  } else {
    fprintf(f, "NEWT_INJECTION(0x%08X, \"%s(...)\") {\n",
            address(), name());
    fprintf(f, "  NEWT_RETVAL %s(", name());
    for (i=0; i<nTypeRef;i++) {
      fprintf(f, "NEWT_ARG%d(", i);
      pTypeRef[i]->exportCppCall(f);
      fprintf(f, ")");
      if (i<nTypeRef-1) fprintf(f, ", ");
    }
    fprintf(f, ");\n");
  }
  fprintf(f, "  NEWT_RETURN;\n}\n\n#endif\n\n");

}

const char* AlCPPMethod::stringCppCall()
{
  static char buf[2048];
  char *dst = buf;
  int i;
  
  if (pKlass) {
    dst += sprintf(dst, "((%s*)r0)->%s(", klass(), name());
    for (i=1; i<nTypeRef;i++) {
      dst += sprintf(dst, "(%s)r%d", pTypeRef[i]->stringCpp(), i);
      if (i<nTypeRef-1) dst += sprintf(dst, ", ");
    }
    dst += sprintf(dst, ")");
  } else {
    dst += sprintf(dst, "%s(", name());
    for (i=0; i<nTypeRef;i++) {
      dst += sprintf(dst, "(%s)r%d", pTypeRef[i]->stringCpp(), i);
      if (i<nTypeRef-1) dst += sprintf(dst, ", ");
    }
    dst += sprintf(dst, ")");
  }
  return buf;
}




