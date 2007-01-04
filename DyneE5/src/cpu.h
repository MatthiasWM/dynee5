

#ifndef DN_CPU_H
#define DN_CPU_H

#include <stdio.h>


extern "C" {
#include "armulator/armdefs.h"
#include "armulator/armemu.h"
}


class Dn_Memory;

typedef unsigned char flag;
typedef signed char byte;
typedef unsigned char ubyte;
typedef unsigned short ushort;
typedef unsigned int uint;


const ubyte modeUser  = 0x10;
const ubyte modeFiq   = 0x11;
const ubyte modeIrq   = 0x12;
const ubyte modeSuper = 0x13;
const ubyte modeAbort = 0x17;
const ubyte modeUndef = 0x1b;


const int NTraceback = 16384;

class Dn_CPU {
  Dn_Memory *pMem;
  char trace;
  ubyte pRunning;
  
  static void idle_step(void*);

public: // public, but shouldn't be
  struct ARMul_State *state;
  char watchpoints;
  ARMdword pNInstr;
  unsigned int traceback[NTraceback];
  int iTraceback;

public:
  Dn_CPU(Dn_Memory*);
  int dis(char *dst, unsigned int addr);
  int dis_cc(char *dst, unsigned int addr);
  void step_into();
  void step_over();
  void step_over_no_sim();
  void step_out();
  void stop_run();
  void stop();
  void toggle_breakpoint();
  void toggle_trace() { trace = !trace; }
  unsigned int get_pc();
  void set_pc(const char *);
  void set_pc(unsigned int);
  void trigger_irq(unsigned int);
  int get_num_registers();
  void print_register(char *dst, int n);
  void print_flags(char *dst);
  void enable_watchpoints(char en=1) { watchpoints = en; }

  void reset();
  unsigned int nInstr();
  void dump_registers();
};


#endif

