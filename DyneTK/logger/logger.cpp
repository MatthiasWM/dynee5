// logger.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "logger.h"

#include <stdio.h>


BOOL APIENTRY DllMain( HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved
                      )
{
  switch (ul_reason_for_call)
  {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}


typedef void *(API_FP *oFn1)(void *a1);
typedef void *(API_FP *oFn4)(void *a1, void *a2, void *a3, void *a4);
typedef void *(API_FP *oFn5)(void *a1, void *a2, void *a3, void *a4, void *a5);
typedef void *(API_FP *oFn6)(void *a1, void *a2, void *a3, void *a4, void *a5, void *a6);
typedef void *(API_FP *oFn)(void *a1, void *a2, void *a3, void *a4, void *a5, void *a6);

oFn1 oMnpClose;
oFn oMnpConnect;
oFn1 oMnpDisconnect;
oFn oMnpDump;
oFn oMnpFlushInput;
oFn oMnpFlushOutput;
oFn oMnpGetError;
oFn1 oMnpListen;
oFn6 oMnpOpen;
oFn oMnpReceive;
oFn5 oMnpReceiveXtd;
oFn oMnpSend;
oFn4 oMnpSendXtd;
oFn oMnpSetDTR;
oFn oMnpStart;
oFn oMnpTrace;

FILE *o;
int ii;

void init()
{
  if (oMnpClose) 
    return;
  HMODULE hMod = LoadLibrary("oMnp32.dll");
  oMnpClose =       (oFn1)GetProcAddress(hMod, "MnpClose");
  oMnpConnect =     (oFn)GetProcAddress(hMod, "MnpConnect");
  oMnpDisconnect =  (oFn1)GetProcAddress(hMod, "MnpDisconnect");
  oMnpDump =        (oFn)GetProcAddress(hMod, "MnpDump");
  oMnpFlushInput =  (oFn)GetProcAddress(hMod, "MnpFlushInput");
  oMnpFlushOutput = (oFn)GetProcAddress(hMod, "MnpFlushOutput");
  oMnpGetError =    (oFn)GetProcAddress(hMod, "MnpGetError");
  oMnpListen =      (oFn1)GetProcAddress(hMod, "MnpListen");
  oMnpOpen =        (oFn6)GetProcAddress(hMod, "MnpOpen");
  oMnpReceive =     (oFn)GetProcAddress(hMod, "MnpReceive");
  oMnpReceiveXtd =  (oFn5)GetProcAddress(hMod, "MnpReceiveXtd");
  oMnpSend =        (oFn)GetProcAddress(hMod, "MnpSend");
  oMnpSendXtd =     (oFn4)GetProcAddress(hMod, "MnpSendXtd");
  oMnpSetDTR =      (oFn)GetProcAddress(hMod, "MnpSetDTR");
  oMnpStart =       (oFn)GetProcAddress(hMod, "MnpStart");
  oMnpTrace =       (oFn)GetProcAddress(hMod, "MnpTrace");

  o = fopen("d:\\inspectorLog.txt", "wb");
  ii = 0;
}


void fdump(unsigned char *d, int n) {
  int i;
  fprintf(o, "unsigned char data%04d[%d] = {", ii, n);
  for (i=0; i<n; i++) {
    if ((i&15)==0) 
      fprintf(o, "\n  ");
    fprintf(o, "0x%02x, ", d[i]);
  }
  for (i=0; i<n; i++) {
    if ((i&63)==0) 
      fprintf(o, "\n  // ");
    fprintf(o, "%c", d[i]>31&&d[i]!=127 ? d[i] : '.');
  }
  fprintf(o, "\n};\n");
  ii++;
}

void *API_FP nMnpClose(void *a1)
{
  if (!oMnpClose) init();
  fprintf(o, "// Close...\n"); fflush(o);
  return oMnpClose(a1);
}
LOGGER_API __declspec(naked) void API_ MnpClose() {
  __asm jmp nMnpClose
}

LOGGER_API void *API_ MnpConnect(void *a1, void *a2, void *a3, void *a4, void *a5, void *a6)
{
  if (!oMnpClose) init();
  return oMnpConnect(a1, a2, a3, a4, a5, a6);
}

void *API_FP nMnpDisconnect(void *a1)
{
  if (!oMnpClose) init();
  fprintf(o, "// Disconnect...\n"); fflush(o);
  return oMnpDisconnect(a1);
}
LOGGER_API __declspec(naked) void API_ MnpDisconnect() {
  __asm jmp nMnpDisconnect
}

LOGGER_API void *API_ MnpDump(void *a1, void *a2, void *a3, void *a4, void *a5, void *a6)
{
  if (!oMnpClose) init();
  return oMnpDump(a1, a2, a3, a4, a5, a6);
}

LOGGER_API void *API_ MnpFlushInput(void *a1, void *a2, void *a3, void *a4, void *a5, void *a6)
{
  if (!oMnpClose) init();
  return oMnpFlushInput(a1, a2, a3, a4, a5, a6);
}

LOGGER_API void *API_ MnpFlushOutput(void *a1, void *a2, void *a3, void *a4, void *a5, void *a6)
{
  if (!oMnpClose) init();
  return oMnpFlushOutput(a1, a2, a3, a4, a5, a6);
}

LOGGER_API void *API_ MnpGetError(void *a1, void *a2, void *a3, void *a4, void *a5, void *a6)
{
  if (!oMnpClose) init();
  return oMnpGetError(a1, a2, a3, a4, a5, a6);
}

void *API_FP nMnpListen(void *a1)
{
  if (!oMnpClose) init();
  fprintf(o, "// Listen...\n"); fflush(o);
  return oMnpListen(a1);
}
LOGGER_API __declspec(naked) void API_ MnpListen() {
  __asm jmp nMnpListen
}

void *API_FP nMnpOpen(void *a1, void *a2, void *a3, void *a4, void *a5, void *a6)
{
  if (!oMnpClose) init();
  fprintf(o, "// Open...\n"); fflush(o);
  void *ret = oMnpOpen(a1, a2, a3, a4, a5, a6);
  return ret;
}
LOGGER_API __declspec(naked) void API_ MnpOpen() {
  __asm jmp nMnpOpen
}

LOGGER_API void *API_ MnpReceive(void *a1, void *a2, void *a3, void *a4, void *a5, void *a6)
{
  if (!oMnpClose) init();
  return oMnpReceive(a1, a2, a3, a4, a5, a6);
}

void *API_FP nMnpReceiveXtd(void *a1, void *a2, void *a3, void *a4, void *a5)
{
  if (!oMnpClose) init();
  void *ret = oMnpReceiveXtd(a1, a2, a3, a4, a5);
  int n = *(int*)a5;
  if (n) {
    fprintf(o, "// Received...\n"); 
    fdump((unsigned char*)a2, n);
    fflush(o);
  }
  return ret;
}
LOGGER_API __declspec(naked) void API_ MnpReceiveXtd() {
  __asm jmp nMnpReceiveXtd
}

LOGGER_API void *API_ MnpSend(void *a1, void *a2, void *a3, void *a4, void *a5, void *a6)
{
  if (!oMnpClose) init();
  return oMnpSend(a1, a2, a3, a4, a5, a6);
}

void *API_FP nMnpSendXtd(void *a1, void *a2, void *a3, void *a4)
{
  if (!oMnpClose) init();
  fprintf(o, "// Send...\n");
  fdump((unsigned char*)a2, (int)a3);
  return oMnpSendXtd(a1, a2, a3, a4);
}
LOGGER_API __declspec(naked) void API_ MnpSendXtd() {
  __asm jmp nMnpSendXtd
}

LOGGER_API void *API_ MnpSetDTR(void *a1, void *a2, void *a3, void *a4, void *a5, void *a6)
{
  if (!oMnpClose) init();
  return oMnpSetDTR(a1, a2, a3, a4, a5, a6);
}

LOGGER_API void *API_ MnpStart(void *a1, void *a2, void *a3, void *a4, void *a5, void *a6)
{
  if (!oMnpClose) init();
  return oMnpStart(a1, a2, a3, a4, a5, a6);
}

LOGGER_API void *API_ MnpTrace(void *a1, void *a2, void *a3, void *a4, void *a5, void *a6)
{
  if (!oMnpClose) init();
  return oMnpTrace(a1, a2, a3, a4, a5, a6);
}

