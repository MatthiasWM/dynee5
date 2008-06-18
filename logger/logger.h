
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LOGGER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LOGGER_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef LOGGER_EXPORTS
#define LOGGER_API __declspec(dllexport)
#else
#define LOGGER_API __declspec(dllimport)
#endif

#define API_ __cdecl
#define API_FP __stdcall

extern "C" {
LOGGER_API void API_ MnpClose();
LOGGER_API void *API_ MnpConnect(void*, void*, void*, void*, void*, void*);
LOGGER_API void API_ MnpDisconnect();
LOGGER_API void *API_ MnpDump(void*, void*, void*, void*, void*, void*);
LOGGER_API void *API_ MnpFlushInput(void*, void*, void*, void*, void*, void*);
LOGGER_API void *API_ MnpFlushOutput(void*, void*, void*, void*, void*, void*);
LOGGER_API void *API_ MnpGetError(void*, void*, void*, void*, void*, void*);
LOGGER_API void API_ MnpListen();
LOGGER_API void API_ MnpOpen();
LOGGER_API void *API_ MnpReceive(void*, void*, void*, void*, void*, void*);
LOGGER_API void API_ MnpReceiveXtd();
LOGGER_API void *API_ MnpSend(void*, void*, void*, void*, void*, void*);
LOGGER_API void API_ MnpSendXtd();
LOGGER_API void *API_ MnpSetDTR(void*, void*, void*, void*, void*, void*);
LOGGER_API void *API_ MnpStart(void*, void*, void*, void*, void*, void*);
LOGGER_API void *API_ MnpTrace(void*, void*, void*, void*, void*, void*);
}