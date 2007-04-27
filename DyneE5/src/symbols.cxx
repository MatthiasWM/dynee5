
#ifdef WIN32
# include <winsock2.h>
#else
# include <arpa/inet.h>
#endif

#include "symbols.h"
#include "memory.h"
#include "cpu.h"
#include "main.h"

#include <stdio.h>
#include <string.h>


static const unsigned int BREAKPOINT = 0x00000200;
static const unsigned int TMPBREAK   = 0x00000400;
static const unsigned int WATCHPOINT = 0x00000800;


const char *Dn_Symbols::get(unsigned int addr) {
  Dn_S_Symbol::iterator s = list.find(addr);
  if (s==list.end()) return 0L;
  return s->second;
}

void Dn_Symbols::load(const char *filename) {
  char buf[2048];
  FILE *f = fopen(filename, "r");
  if (!f) return;
  while (!feof(f)) {
    buf[0] = 0;
    fgets(buf, 255, f);
    if (feof(f)) break;
    int n = strlen(buf);
    if (buf[n-1]=='\n') buf[n-1] = 0;
    if (n<11) continue;
    if (buf[0]=='0' && buf[1]=='x') {
      unsigned int addr = -1;
      if (sscanf(buf+2, "%x", &addr)==1) {
        list.insert(std::make_pair(addr, strdup(buf+11)));
      }
    } else if (buf[0]=='c' && buf[1]==' ') {
      unsigned int addr = -1;
      if (sscanf(buf+2, "%x", &addr)==1) {
        comments.insert(std::make_pair(addr, strdup(buf+11)));
      }
    } else if (buf[0]=='b' && buf[1]==' ') {
      unsigned int addr = -1;
      sscanf(buf+2, "%x", &addr);
      set_breakpoint(addr);
    } else if (buf[0]=='b' && buf[1]=='w' && buf[2]==' ') {
      unsigned int addr = -1;
      sscanf(buf+2, "%x", &addr);
      set_watchpoint(addr);
    } else if (buf[0]=='n' && buf[1]==' ') {
      // patch some memory to do a native call
      unsigned int addr = -1;
      unsigned int fid = -1;
      sscanf(buf+2, "%x %d", &addr, &fid);
      fid = (fid&0xf)|((fid&0x70)<<1)|((fid&0x7f80)<<5);
      mem->set32(addr+0, 0xee000210|fid, DN_MEM_PATCH);
    } else if (buf[0]=='p' && buf[1]==' ') {
      // patch some memory to a new value
      unsigned int data = -1;
      unsigned int addr = -1;
      sscanf(buf+2, "%x %x", &addr, &data);
      mem->set32(addr, data, DN_MEM_PATCH);
    }
  }
  fclose(f);
}

unsigned int Dn_Symbols::get_flags(unsigned int addr) {
  Dn_S_Flags::iterator fi = flags.find(addr);
  if (fi==flags.end()) return 0;
  return fi->second;
}

void Dn_Symbols::set_flags(unsigned int addr, unsigned int ff) {
  Dn_S_Flags::iterator fi = flags.find(addr);
  if (fi==flags.end()) {
    if (ff) {
      flags.insert(std::make_pair(addr, ff));
    } else {
      ;
    }
  } else {
    if (ff) {
      fi->second = ff;
    } else {
      fi->second = 0;
      //flags.remove(fi);
    }
  }
}

unsigned char Dn_Symbols::is_breakpoint(unsigned int addr) {
  if (get_flags(addr) & BREAKPOINT) return 1;
  return 0;
}

void Dn_Symbols::set_breakpoint(unsigned int addr) {
  unsigned int f = get_flags(addr);
  if ((f & BREAKPOINT)==0) 
    set_flags(addr, f|BREAKPOINT);
}

void Dn_Symbols::clear_breakpoint(unsigned int addr) {
  unsigned int f = get_flags(addr);
  if (f & BREAKPOINT) 
    set_flags(addr, f&~BREAKPOINT);
}


unsigned char Dn_Symbols::is_tmp_breakpoint(unsigned int addr) {
  if (get_flags(addr) & TMPBREAK) return 1;
  return 0;
}

void Dn_Symbols::set_tmp_breakpoint(unsigned int addr) {
  unsigned int f = get_flags(addr);
  if ((f & TMPBREAK)==0) 
    set_flags(addr, f|TMPBREAK);
}

void Dn_Symbols::clear_tmp_breakpoint(unsigned int addr) {
  unsigned int f = get_flags(addr);
  if (f & TMPBREAK) 
    set_flags(addr, f&~TMPBREAK);
}


unsigned char Dn_Symbols::is_watchpoint(unsigned int addr) {
  if (get_flags(addr) & WATCHPOINT) return 1;
  return 0;
}

void Dn_Symbols::set_watchpoint(unsigned int addr) {
  unsigned int f = get_flags(addr);
  if ((f & WATCHPOINT)==0) 
    set_flags(addr, f|WATCHPOINT);
}

void Dn_Symbols::clear_watchpoint(unsigned int addr) {
  unsigned int f = get_flags(addr);
  if (f & WATCHPOINT) 
    set_flags(addr, f&~WATCHPOINT);
}


void Dn_Symbols::clear() {
  list.clear();
  flags.clear();
  comments.clear();
}

const char *Dn_Symbols::get_comment(unsigned int addr) {
  Dn_S_Comment::iterator s = comments.find(addr);
  if (s==comments.end()) return 0L;
  return s->second;
}



void Dn_Symbols::add_detail(unsigned int addr, details_fn f) {
  details.insert(std::make_pair(addr, f));
}

void Dn_Symbols::print_details(unsigned int addr) {
  Dn_S_Details::iterator d = details.find(addr);
  if (d==details.end()) return;
  d->second(addr);
}



void print_Rect(unsigned int addr) {
  printf("{l:%d, t:%d, r:%d, b:%d}", 
    mem->get16(addr,   DN_MEM_PEEK|DN_MEM_VIRT), mem->get16(addr+2, DN_MEM_PEEK|DN_MEM_VIRT), 
    mem->get16(addr+4, DN_MEM_PEEK|DN_MEM_VIRT), mem->get16(addr+6, DN_MEM_PEEK|DN_MEM_VIRT));
}

void print_TTask(unsigned int addr) {
  unsigned int a;
  const char *c;
  printf("    {...\n");
  a = mem->get32(addr+0x48, DN_MEM_PEEK|DN_MEM_VIRT);
  c = symbols->get(a);
  printf("      fn1: %08x %s\n", a, c?c:"<unknown>");
  a = mem->get32(addr+0x4c, DN_MEM_PEEK|DN_MEM_VIRT);
  c = symbols->get(a);
  printf("      fn2: %08x %s\n", a, c?c:"<unknown>");
  a = mem->get32(addr+28, DN_MEM_PEEK|DN_MEM_VIRT);
  c = symbols->get(a);
  printf("      fn3: %08x %s\n", a, c?c:"<unknown>");
  a = htonl(mem->get32(addr+0x84, DN_MEM_PEEK|DN_MEM_VIRT));
  printf("      id:  [%.4s]\n", &a);
  printf("    ...}");
}



void print_putc(unsigned int addr) {
  printf("    '%c' (%08x)\n", cpu->state->Reg[0], cpu->state->Reg[0]);
}

void print_SafeShortTimerDelay(unsigned int addr) {
  printf("    delay = %d dec\n", cpu->state->Reg[0]);
}

void print_SetRect(unsigned int addr) {
  printf("    Rect %08 = {l:%d, t:%d, r:%d, b:%d}", cpu->state->Reg[0],
    cpu->state->Reg[1], cpu->state->Reg[2], cpu->state->Reg[3],
    mem->get32(cpu->state->Reg[13], DN_MEM_PEEK|DN_MEM_VIRT) );
}

void print_TScreenDriver_Blit(unsigned int addr) {
  printf("  TScreenDriver = %08x\n", cpu->state->Reg[0]);
  printf("    PixelMap = %08x\n", cpu->state->Reg[1]);
  printf("    Rect = %08x ", cpu->state->Reg[2]);
  print_Rect(cpu->state->Reg[2]); printf("\n");
  printf("    Rect = %08x\n", cpu->state->Reg[3]);
  print_Rect(cpu->state->Reg[3]); printf("\n");
  printf("    long = %08x\n", mem->get32(cpu->state->Reg[13], DN_MEM_PEEK|DN_MEM_VIRT));
}

void print_PtInDRect(unsigned int addr) {
  printf("    pos = %d, %d\n", cpu->state->Reg[0], cpu->state->Reg[1]);
  printf("    Rect = %08x ", cpu->state->Reg[2]);
  print_Rect(cpu->state->Reg[2]); printf("\n");
}

void print_TScheduler_Add(unsigned int addr) {
  //printf("    TTask = %08x\n", cpu->state->Reg[1]);
  //print_TTask(cpu->state->Reg[1]); printf("\n");
}

void print_LowLevelCopyEngine(unsigned int addr) {
  printf("    dst=%08x, src=%08x, n=%08x(%d)\n", 
    cpu->state->Reg[0], cpu->state->Reg[1], 
    cpu->state->Reg[2], cpu->state->Reg[2]);
}

void print_TTask_Init(unsigned int addr) {
  unsigned int a = cpu->state->Reg[1];
  const char *c = symbols->get(a);
  printf("    Function: %08x %s\n", a, c?c:"<unknown>");
}


/** Static variables
0c100fc8: TObjectTable * ; 
*/

/** Attempt at understanding TTask:

TTask::TTask()
TTask::FreeStack()
TTask::SetBequeathId(unsigned long)
TTask::Init(??) FIXME: Init__5TTaskFPFPvUlT2_vUlPvN32P12TEnvironment

size: 0x104 (260d) bytes

+0x0004: 0
+0x001c  (28): *function_3
+0x0048  (72): *function_1
+0x004c  (76): *function_2
+0x006c (108): 0 unsigned int flags; // if mask 02000000 is set, stack is in paged mem
+0x0074 (116): 0 TEnvironment * ; // an environment that depends on this class
+0x0078: 0
+0x007c: 0
+0x0080 (136): 0 unsigned int pagedStackID; // stack, if allocated via "AllocPagedMem"
+0x0084 (140): fourCC ID; // four character identifier
+0x008c (140): 0 void *stack; // stack if allocated with "malloc"
+0x0094 (148): TTaskQItem ; // 8 bytes
 +0x0094 (148/0): 0 ptr to this TTask???
 +0x0098 (152/4): 0
+0x00a0: 0
+0x00ac: 0
+0x00b0: 0
+0x00b4: 0
+0x00b8: 0
+0x00bc: TDoubleQItem ; // 12 bytes
 +0x00bc (188/0): TDoubleQItem *next;
 +0x00c0 (192/4): TDoubleQItem *prev; // ??
 +0x00c4 (196/8): TDoubleQContainer * ; // some Queue that we are in
+0x00c8: TDoubleQItem
 +0x00c8 (200/0): TDoubleQItem *next;
 +0x00cc (204/4): TDoubleQItem *prev; // ??
 +0x00d0 (208/8): TDoubleQContainer * ; // some Queue that we are in
+0x00d4: 0
+0x00d8: 0
+0x00dc: 0
+0x00e0: 0
+0x00e4: 0
+0x00e8: 0
+0x00ec: 0
+0x00f0 (240): 0 objectId ; // an object ID that identifies us in the globl object table
+0x00f4 (244): 0 objectId ; // another object ID in the gloabl object table
+0x00f8: 0
+0x00fc (252): 0 unsigned int bequeathId; 
+0x0100 (256): 0 
 0x0104: (size = 260 bytes)

Tasks that are initialised after a cold boot in this order:
  01a00024 MonitorEntryGlue
  01b00078 OsBoot
  01b094a8 UserBoot(void)
  01afde80 InitialKSRVTask(void)
  01bdce7c TUTaskWorld::TaskEntry(unsigned long, unsigned long)
*/


Dn_Symbols::Dn_Symbols() {
  add_detail(0x00018f08, print_SafeShortTimerDelay);
  add_detail(0x0033ffb0, print_SetRect);
  add_detail(0x0034F7E0, print_putc);
  add_detail(0x003885F8, print_TScreenDriver_Blit);
  add_detail(0x0002efa4, print_PtInDRect);
  add_detail(0x001CC564, print_TScheduler_Add);
  add_detail(0x00394090, print_LowLevelCopyEngine);
  add_detail(0x002522b0, print_TTask_Init);
}

void Dn_Symbols::tick(unsigned int addr) {
  Dn_S_Tick::iterator ti = ticks.find(addr);
  if (ti==ticks.end()) {
    ticks.insert(std::make_pair(addr, 1));
  } else {
    ti->second++;
  }
}

void Dn_Symbols::save_ticks(const char *filename) {
  FILE *f = fopen(filename, "wb");
  Dn_S_Tick::iterator ti = ticks.begin();
  for ( ; ti!=ticks.end() ; ++ti) {
    fprintf(f, "%9d 0x%08x\n", ti->second, ti->first);
  }
  fclose(f);
}

