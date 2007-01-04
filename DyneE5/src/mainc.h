
#ifndef MAINC_H
#define MAINC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "armulator/armdefs.h"

extern void dn_set8(unsigned int a, unsigned int d);
extern void dn_set16(unsigned int a, unsigned int d);
extern void dn_set32(unsigned int a, unsigned int d);

extern unsigned int dn_get8(unsigned int a);
extern unsigned int dn_get16(unsigned int a);
extern unsigned int dn_get32(unsigned int a);

extern void dn_begin_instr_fetch();
extern void dn_end_instr_fetch();

extern void dnCallNative(struct ARMul_State *state, unsigned int instr);

extern const char *dn_get_symbol(unsigned int a);

extern void err_msg(const char *a, ...);
extern void d_msg(const char *a, ...);

#ifdef __cplusplus
}
#endif

#endif


