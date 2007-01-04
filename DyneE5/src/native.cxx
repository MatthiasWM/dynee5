

#include "memory.h"
#include "dyne.h"
#include "symbols.h"
#include "main.h"
#include "mainc.h"

#include "cpu.h"


typedef void (*NativeCall)(ARMul_State *);

extern "C" void WriteR15(ARMul_State *, ARMword);


void natNop(ARMul_State*) {
}

void natSafeShortTimerDelay(ARMul_State *state) {
  ARMword dest = state->Reg[14];
//  state->Reg[15] = state->Reg[14];
  WriteR15(state, dest);
}

/*
 * This function is called when all we do is wait for in interrupt.
 * After the interrupt was handled, we go back to our nap. So instead
 * of having the emulator busy-wait, we do a timout on the host
 * until the first int would be triggered.
 * (since we are currently sooo slooow, we trigger it right away)
 */
void natSleepTask(ARMul_State *state) {
  unsigned int 
    t0 = mem->get32(0x0f182000), 
    t1 = mem->get32(0x0f182400), 
    t2 = mem->get32(0x0f182800), 
    t3 = mem->get32(0x0f182c00), 
    tt = mem->get32(0x0f181800);
  unsigned int td = 0xffffffff;
  if (t0>tt && t0<td) td = t0;
  if (t1>tt && t1<td) td = t1;
  if (t2>tt && t2<td) td = t2;
  if (t3>tt && t3<td) td = t3;
  td = ((td-tt)/delta_t)-delta_t;
  cpu->pNInstr += td;
  WriteR15(state, 0x001cc55c);
}

/** This function fills RAM with a pattern, then overwrites
 * the pattern twice to verify that all RAM cells are working.
 * In the emulation, we skip this test and just fill the RAM.
 */
void natTestRAM(ARMul_State *state) {
  unsigned int dst = state->Reg[0];
  unsigned int n = state->Reg[1];
  for ( ; n>0; n-- ) {
    mem->set32(dst, 0xb6db6db6, 0); dst+=4;
    mem->set32(dst, 0xdb6db6db, 0); dst+=4;
    mem->set32(dst, 0x6db6db6d, 0); dst+=4;
  }
  state->Reg[0] = 0;
  state->Reg[1] = 0;
  state->Reg[2] = 0xb6db6db6;
  state->Reg[3] = 0xdb6db6db;
  state->Reg[4] = 0;
  state->Reg[5] = 0xb6db6db6;
  state->Reg[6] = 0xb6db6db6;
  state->Reg[7] = 0;
  WriteR15(state, 0x00019b30);
}

/** 000db0d8: TGeoPortDebugLink::BeaconDetect(long)
 Just return "no beacon" (=0?) and return (db0d0)*/
void natBeaconDetect(ARMul_State *state) {
  state->Reg[0] = 0;
  WriteR15(state, 0x000db0d0);
}

/** 1a7530: calculation of the checksums takes forever! 
 001a7250 to 001a7268 */
void natChecksum(ARMul_State *state) {
  do {
    state->Reg[2] = mem->get32(state->Reg[0], DN_MEM_VIRT);
    state->Reg[0] += 4;
    state->Reg[3] = (state->Reg[2] << 16);
    state->Reg[7] += (state->Reg[3] >> 16);
    state->Reg[9] += (state->Reg[2] >> 16);
    state->Reg[1] -= 1;
  } while (state->Reg[1]!=0);
  WriteR15(state, 0x001a726c);
  // R7 = 18710bd6, R9 = 4afd2193
}

NativeCall nativeLUT[] = {
  /*   0...4   */ natNop, natSafeShortTimerDelay, natSleepTask, natTestRAM, natBeaconDetect,
  /*   5...5   */ natChecksum
};

void dnCallNative(struct ARMul_State *state, unsigned int instr) {
  int n = (instr&0xf)|((instr>>1)&0x70)|((instr>>5)&0x7f80);
  nativeLUT[n](state);
}

