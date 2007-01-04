

#include "cpu.h"
#include "memory.h"
#include "dyne.h"
#include "symbols.h"
#include "main.h"

#include <FL/Fl.H>
#include <FL/fl_ask.H>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef BIT
# undef BIT
#endif
#ifdef BITS
# undef BITS
#endif

#define BIT(v,a) ((v>>a)&1)
#define BITS(v,a,b) ((v>>a)&((1<<(b-a+1))-1))

FILE *trf = 0L;

static const char *mode_lut[] = {
  "???", "???", "???", "???", "???", "???", "???", "???", 
  "???", "???", "???", "???", "???", "???", "???", "???", 
  "usr", "fiq", "irq", "svc", "???", "???", "???", "abt", 
  "???", "???", "???", "und", "???", "???", "???", "???"
};

/**
 * Create a CPU
 */
Dn_CPU::Dn_CPU(Dn_Memory *mem) 
: pMem(mem) {
  pRunning = 0;
  pNInstr = 0;
  trace = 0;
  watchpoints = 1;

  if (sizeof(ARMword)!=4) fprintf(stderr, "ARMword has wrong size!\n");
  if (sizeof(ARMdword)!=8) fprintf(stderr, "ARMdword has wrong size!\n");

  ARMul_EmulateInit ();
  state = ARMul_NewState ();
  ARMul_SelectProcessor (state, ARM_v4_Prop); //++ some processor ID!
  state->cpu->cpu_val = state->cpu->cpu_mask = SA1100;
  //ARMul_SelectProcessor (state, SA1100); //++ some processor ID!
  ARMul_Reset(state);
  state->verbose = 1;
  state->bigendSig = HIGH;
  ARMul_MemoryInit(state, 8*1024*1024);
  ARMul_CoProInit(state);
  ARMul_OSInit(state);
  ARMul_Reset(state);

  iTraceback = 0;
  memset(traceback, 0xff, NTraceback*sizeof(unsigned int));
  traceback[0] = 0; 
  iTraceback++;

  //trf = fopen("../trace.txt", "wb");
}

unsigned int Dn_CPU::nInstr() {
  return pNInstr;
}

extern "C" int disarm(char *dst, unsigned int addr, unsigned int cmd);
extern "C" int disarm_cc(char *dst, unsigned int addr, unsigned int cmd);

/**
 * Disassemble the command at the given address into the text buffer.
 */
int Dn_CPU::dis(char *dst, unsigned int addr)
{
  if (addr&0x00000003) {
    sprintf(dst, "<odd>");
    return 1;
  }
  unsigned int op = pMem->get32(addr, DN_MEM_PEEK|DN_MEM_INSTR|DN_MEM_VIRT);
  return disarm(dst, addr, op);
}

/**
 * Disassemble some ARM code into pseudo "C"
 */
int Dn_CPU::dis_cc(char *dst, unsigned int addr)
{
  if (addr&0x00000003) {
    sprintf(dst, "<odd>");
    return 1;
  }
  unsigned int op = pMem->get32(addr, DN_MEM_PEEK|DN_MEM_INSTR|DN_MEM_VIRT);
  return disarm_cc(dst, addr, op);
}


void Dn_CPU::toggle_breakpoint() {
  if (symbols->is_breakpoint(get_pc())) {
    symbols->clear_breakpoint(get_pc());
  } else {
    symbols->set_breakpoint(get_pc());
  }
}


extern void update_gui();

static char catch_up_when_done = 0;

void Dn_CPU::idle_step(void *u) {
  Dn_CPU *t = (Dn_CPU*)u;
  int i = 40000;
  for (; i>0; i--) {
    uint addr = t->get_pc();
    if (symbols->is_breakpoint(addr)||symbols->is_tmp_breakpoint(addr)) {
      t->stop_run();
      symbols->clear_tmp_breakpoint(addr);
      if (catch_up_when_done) {
        catch_up_when_done = 0;
      }
      Fl::awake();
      return;
    }
    t->step_into();
    if (!t->pRunning) break;
  }
  update_gui();
}

void Dn_CPU::trigger_irq(unsigned int v) {
  //mem->set32(0x0f183000, v, DN_MEM_PATCH);
  state->NirqSig = 0;
  //ARMul_Abort(state, ARMul_IRQV); // ARMul_FIQV
  stop();
}

/**
 * Just run away.
 */
void Dn_CPU::stop_run() {
  if (pRunning) {
    wStopRun->label("@>");
    Fl::remove_idle(idle_step, this);
  } else {
    wStopRun->label("@-2square");
    step_into();
    Fl::add_idle(idle_step, this);
  }
  pRunning = ~pRunning;
  wStopRun->redraw();
  wAsmView->redraw();
  update_gui();
}

void Dn_CPU::stop() {
  if (pRunning) {
    stop_run();
  }
}

void Dn_CPU::set_pc(const char *addr) {
  unsigned int v;
  sscanf(addr, "%x", &v);
  set_pc(v);
}

void Dn_CPU::set_pc(unsigned int x) {
  state->Reg[15] = x;
  state->pc = x;
  wStopRun->redraw();
  wAsmView->redraw();
  update_gui();
} 

void Dn_CPU::step_over_no_sim() {
  catch_up_when_done = 1;
  step_over();
}

void Dn_CPU::step_over() {
  // FIXME: if this is not a branch command, use "step" instead!
  // let Dyne run to the next command
  if (!pRunning) {
    uint pc = get_pc() + 4;
    symbols->set_tmp_breakpoint(pc);
    stop_run();
  }
}


/**
 * Execute a single instruction, following branches.
 */
void Dn_CPU::step_into() {
  pNInstr++;
  uint prev_pc;
  ARMword pc = state->Reg[15];
  

  if (trace) {
    char buf[2048];
    dis(buf, get_pc());
    printf("  %s", buf);
  }

  symbols->tick(pc);
  prev_pc = pc;

  // do the actual emulation
  state->Emulate = ONCE;
  state->NextInstr = RESUME;
  cGPIO->update_cycle();
  pc = ARMul_Emulate32(state);
  //printf("R15 %08x, pc %08x, state %d\n", state->Reg[15], pc, state->NextInstr);
  state->Reg[15] = pc; // don't ask :-(, see state->NextInstr

  traceback[iTraceback] = pc; 
  wTracebackView->base(iTraceback);
  iTraceback++; 
  iTraceback = iTraceback % NTraceback;

  const char *s = symbols->get(pc);
  //if (s) 
  //  printf("--> %08x(%08x): %s (%x,%x,%x,%x,%d)\n", pc, prev_pc, s,
  //    state->Reg[0], state->Reg[1], state->Reg[2], state->Reg[3],
  //    cpu->nInstr());
  if (s && trf) {
    static char spc[] = 
      "                                                                                "
      "                                                                                ";
    int nTab = 80-((state->Reg[13]/4)%80);
    fprintf(trf, "%08x,%02x ", state->Reg[13], state->Mode&0x1f);
    fwrite(spc, 1, nTab, trf);
    //fprintf(trf, "%08x: %s\n", pc, s);
    fprintf(trf, "%08x(%08x): %s (%x,%x,%x,%x,%d)\n", pc, prev_pc, s,
      state->Reg[0], state->Reg[1], state->Reg[2], state->Reg[3],
      cpu->nInstr());
  }
  symbols->print_details(pc);

  //if (nInstr()==23797610) stop();
  //if (nInstr()==23798600) stop();
}


/**
 * Print a text showing the contents of a register.
 */
void Dn_CPU::print_register(char *d, int i) {
  sprintf(d, "R%-2d=%08x", i, state->Reg[i]);
}

/**
 * Print a text showing the contents of all flags.
 */
void Dn_CPU::print_flags(char *d) {
  sprintf(d, "%c%c%c%c %c %c %s", 
    state->NFlag?'N':'n', state->ZFlag?'Z':'z', 
    state->CFlag?'C':'c', state->VFlag?'V':'v',
    BIT(state->IFFlags,1)?'I':'i', BIT(state->IFFlags,0)?'F':'f', 
    mode_lut[state->Mode&0x1f]);
}

unsigned int Dn_CPU::get_pc() {
  return state->pc;
}

int Dn_CPU::get_num_registers() {
  return 16;
}

void Dn_CPU::dump_registers() {
  int i;
  printf("/---------------- Register dump ----------------\n");
  for (i=0; i<16; i++) {
    printf("| R%02d = 0x%08x\n", i, state->Reg[i]);
  }
  char buf[80];
  print_flags(buf);
  printf("| Flags: %s\n", buf);
  printf("\\-----------------------------------------------\n");
}



