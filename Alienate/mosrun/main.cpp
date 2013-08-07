/*
  mosrun - the MacOS MPW runtime emulator
  Copyright (C) 2013  Matthias Melcher

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  The author can be contacted at mosrun AT matthiasm DOT com.
  The latest source code can be found at http://code.google.com/p/dynee5/
 */

//
// This is a proposal for possible flags that may make life easier when using
// mosrun in a Unix or DOS environment.
//
// ---help
// ---run
// ---aaa-bbb-ccc-to-ddd=eee
//    aaa is "all":   all files in this command line will be treated this way
//           "allin": all input streams will be filtered
//           "allout":all output streams will be filtered
//           "next":  only the next file will be treated this way, overrides all, default
//           "regex": all files which match the following pattern, default if eee is set
//           "stdin": apply to stdin stream (abbreviate as "in")
//           "stdout":apply to stdout stream
//           "stderr":apply to stderr stream
//           "conout":apply to stdout and stderr stream
//    bbb is "name":  apply a filter to the file name only
//           "data":  apply a filter to the data in the stream or file only
//           "file":  apply filters to the file name and file data, (default)
//    ccc is "host":  convert from host format, default
//           "unix":  convert from utf-8, '\n', '/'
//           "mac":   convert from MacRom, '\r', ':'
//           "dos":   convert from MS-DOS format, CodePage, '\r\n', '\'
//    to is  "to":    indicates format conversion
//           "keep":  overrides default filters with absolutely no conversion
//    ddd    as ccc   convert ot format
//    eee    "..."    optional name pattern for "regex" attribute
//                    if regex is set, eee must be filled
//                    if eee is set, regex is implied and no other option must be chosen
//
// Implied rules are ---stdin-to-mac ---conout-mac-to-host
//
// example: ARM6asm ---unix-to-mac test.s -o ---keep test.s.o
//

//
// 4/Aug/2013:
//    ARM6asm runs and creates object files
//    ARM6c runs and creates object files
//    ARMCFront misses traps
//      ERROR: unimplemented trap 0x0000A994: _CurResFile
//      ERROR: unimplemented trap 0x0000A9A4: _HomeResFile
//      ERROR: unimplemented trap 0x0000A80D: _Count1Resources
//      ERROR: unimplemented trap 0x0000A80E: _Get1IxResource
//      ERROR: unimplemented trap 0x0000A04A: _HNoPurge
//    ARMCpp runs and creates object files
//    ARMLink runs, probably missing some files to link and generate an executable
//    DumpAIF seems to run
//    DumpAOF seems to run
//    Makemake seems to run but misses traps:
//      ERROR: unimplemented trap 0x0000A9ED: _Pack6
//      ERROR: unimplemented trap 0x0000A00C: _GetFileInfo
//    Rex seems to work
//


//
// Goal: compile the ROM Extension for the Newton Emulator "Einstein" on OS X
//
// Tools needed:
//    ProtocolGen     -  Script: what do we do?
//    ProtocolGenTool -  seems to be working
//    ARMCpp          -  seems to be working
//    ARM6Asm         -  seems to be working
//    Rex             -  seems to be working
//    ARMLink         -  seems to be working
//    Packer          -  may be working, help page is rendered
//    SetFile         ok native OS X tool
//



// Inlcude all the required system headers

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/xattr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

// Include our own interfaces

#include "main.h"
#include "names.h"
#include "log.h"
#include "rsrc.h"
#include "memory.h"
#include "filename.h"
#include "fileio.h"
#include "resourcefork.h"
#include "breakpoints.h"
#include "traps.h"
#include "cpu.h"
#include "systemram.h"

// Inlcude Musahi's m68k emulator

extern "C" {
  #include "musashi331/m68k.h"
}


// application global variables

byte *theApp = 0;
unsigned int theAppSize = 0;
byte *theRsrc = 0;
unsigned int theRsrcSize = 0;
byte *theJumpTable = 0;

const unsigned int kMosApplLimit      = 0x0130;  // Application heap limit
const unsigned int kMosApplZone       = 0x02aa;  // Address of application heap zone
const unsigned int kMosHeapEnd        = 0x0114;  // Address of end of application heap zone
const unsigned int kMosMemTop         = 0x0108;  // Address of end of RAM (on Macintosh XL, end of RAM available to applications)
const unsigned int kMosSysZone        = 0x02a6;  // Address of system heap zone
const unsigned int kMosBufPtr         = 0x010c;  // Address of end of jump table
const unsigned int kMosCurrentA5      = 0x0904;  // Address of boundary between application globals and application parameters
const unsigned int kMosCurJTOffset    = 0x0934;  // Offset to jump table from location pointed to by A5 (word)
const unsigned int kMosCurStackBase   = 0x0908;  // Address of base of stack; start of application globals

const unsigned int kMosRndSeed        = 0x0156;  // random number seed (long)
const unsigned int kMosTicks          = 0x016A;  // ticks since last boot (unsigned long)
const unsigned int kMosDeskHook       = 0x0A6C;  // hook for painting desktop (pointer)
const unsigned int kMosMBarHeight     = 0x0BAA;  // height of menu bar (integer)

/**
 * Find native tools in load path
 */
const char *toolPath(const char *aName)
{
  static char path[PATH_MAX];
  struct stat st;

  if (aName[0] == '/')
    return aName;

  path[0] = 0;
  if (getenv("MOSRUN_PATH") != NULL) {
    strcpy(path, getenv("MOSRUN_PATH"));
    if (path[strlen(path) - 1] != '/') strcat(path, "/");
    strcat(path, aName);
  }
  if (path[0] == 0 || stat(path, &st) != 0) {
    strcpy(path, "/usr/local/lib/mosrun/");
    strcat(path, aName);
  }
  return path;
}

/**
 * Load the executable part of a file from the resource fork.
 */
int loadApp(const char *aName)
{
  const char *path;

  path = toolPath(aName);
#if defined (__APPLE__) && defined (__MACH__)
  ssize_t size = getxattr(path, "com.apple.ResourceFork", 0L, 0, 0, 0);
  if (size==-1) {
    return 0;
  }
  theAppSize = size;
  theApp = (byte*)mosNewPtr(size);
  ssize_t ret = getxattr(path, "com.apple.ResourceFork", theApp, size, 0, 0);
  if (ret==-1) {
    return 0;
  }
  mosLog("%s has a %ld byte resource fork\n", path, size);  
#else
  FILE *f;
  struct stat st;
  f = fopen(path, "rb");
  if (f == NULL)
    return 0;
  stat(path, &st);
  theAppSize = st.st_size;
  theApp = (byte*)mosNewPtr(theAppSize);
  fread(theApp, 1, theAppSize, f);
  fclose(f);
#endif
  readResourceMap();
  mosHandle code0 = GetResource('CODE', 0);
  if (code0==0) {
    mosLog("CODE 0 not found\n");
    return 0;
  }
  gMosCurrentA5 = createA5World(code0);
  return 1;
}


/**
 * Load the executable part of a file from internal memory.
 */
int loadInternalApp()
{
  if (gAppResource) {
    theAppSize = gAppResourceSize;
    theApp = (byte*)mosNewPtr(theAppSize);
    memcpy(theApp, gAppResource, gAppResourceSize);
    readResourceMap();
    mosHandle code0 = GetResource('CODE', 0);
    if (code0==0) {
      mosLog("CODE 0 not found\n");
      return 0;
    }
    gMosCurrentA5 = createA5World(code0);
    return 1;
  } else {
    return 0;
  }
}


/**
 * Initialize the CPU and loop through each instruction using the m68k emulator.
 */
int runApp()
{
  m68k_set_cpu_type(M68K_CPU_TYPE_68020);
  m68k_pulse_reset();
  m68k_set_cpu_type(M68K_CPU_TYPE_68020);
  m68k_set_reg(M68K_REG_PC, gMosCurrentA5 + gMosCurJTOffset + 2);
  m68k_write_memory_32(gMosCurrentStackBase-4, trapExitApp); // end of app
  m68k_set_reg(M68K_REG_SP, gMosCurrentStackBase-4);
  m68k_set_reg(M68K_REG_A5, gMosCurrentA5);
  m68k_set_instr_hook_callback(m68k_instruction_hook);
  
  //MosGetResource...
  
  char done = 0;
  while(!done) {
    m68k_execute(1);
  }
  return 0;
}


/**
 * Create MPW RAM structures that will be accessed by the emulator.
 *
 * This functions also interpretes the command line arguments.
 *
 * \todo This function must be split up. It's much too long.
 */
int setupSystem(int argc, const char **argv, const char **envp)
{
  int runExternal = 0;
  int i;
  
  // allocate a stack
  gMosCurrentStackBase = mosNewPtr(MOS_STACK_SIZE) + MOS_STACK_SIZE;
  
  // create other memory that will be accessed by the emulation
  trapDispatchTrap = mosNewPtr(2);
  mosWrite16(trapDispatchTrap, 0xaffd);
  trapExitApp = mosNewPtr(2);
  mosWrite16(trapExitApp, 0xaffc);
  
  // create supported trap glue
  mosSetupTrapTable();
  
  // create native calls
  mosPtr tncSyFAccess = createGlue(trapSyFAccess);
  mosPtr tncSyClose   = createGlue(trapSyClose);
  mosPtr tncSyRead    = createGlue(trapSyRead);
  mosPtr tncSyWrite   = createGlue(trapSyWrite);
  mosPtr tncSyIoctl   = createGlue(trapSyIoctl);
  
  // create the IO function table
  mosPtr ioGlue = mosNewPtr(4*6*5); // only three entries are set, two entries are 0'd
  //    FSYS entry
  mosWrite32(ioGlue+0x0000+0x0000, 'FSYS');
  mosWrite32(ioGlue+0x0000+0x0004, tncSyFAccess);
  mosWrite32(ioGlue+0x0000+0x0008, tncSyClose);
  mosWrite32(ioGlue+0x0000+0x000C, tncSyRead);
  mosWrite32(ioGlue+0x0000+0x0010, tncSyWrite);
  mosWrite32(ioGlue+0x0000+0x0014, tncSyIoctl);
  //    CONS entry
  mosWrite32(ioGlue+0x0018+0x0000, 'CONS');
  mosWrite32(ioGlue+0x0018+0x0004, tncSyFAccess);
  mosWrite32(ioGlue+0x0018+0x0008, tncSyClose);
  mosWrite32(ioGlue+0x0018+0x000C, tncSyRead);
  mosWrite32(ioGlue+0x0018+0x0010, tncSyWrite);
  mosWrite32(ioGlue+0x0018+0x0014, tncSyIoctl);
  //    SYST entry
  mosWrite32(ioGlue+0x0030+0x0000, 'SYST');
  mosWrite32(ioGlue+0x0030+0x0004, tncSyFAccess);
  mosWrite32(ioGlue+0x0030+0x0008, tncSyClose);
  mosWrite32(ioGlue+0x0030+0x000C, tncSyRead);
  mosWrite32(ioGlue+0x0030+0x0010, tncSyWrite);
  mosWrite32(ioGlue+0x0030+0x0014, tncSyIoctl);
  
  // create the file descriptor entries for stdin, stdout, and stderr
  mosPtr fdEntries = mosNewPtr(5*4*3);
  // stdin
  mosWrite16(fdEntries+0x0000+0x0000, 1); // input
  mosWrite16(fdEntries+0x0000+0x0002, 0); // status OK
  mosWrite32(fdEntries+0x0000+0x0004, ioGlue+0x0000); // FSYS
  mosWrite32(fdEntries+0x0000+0x0008, (unsigned int)(&stdFiles[0])); // back to host environment
  mosWrite32(fdEntries+0x0000+0x000C, 0); // transfer size
  mosWrite32(fdEntries+0x0000+0x0010, 0); // transfer buffer address
  // stdout
  mosWrite16(fdEntries+0x0014+0x0000, 2); // output
  mosWrite16(fdEntries+0x0014+0x0002, 0); // status OK
  mosWrite32(fdEntries+0x0014+0x0004, ioGlue+0x0000); // FSYS
  mosWrite32(fdEntries+0x0014+0x0008, (unsigned int)(&stdFiles[1])); // back to host environment
  mosWrite32(fdEntries+0x0014+0x000C, 0); // transfer size
  mosWrite32(fdEntries+0x0014+0x0010, 0); // transfer buffer address
  // stderr
  mosWrite16(fdEntries+0x0028+0x0000, 2); // output
  mosWrite16(fdEntries+0x0028+0x0002, 0); // status OK
  mosWrite32(fdEntries+0x0028+0x0004, ioGlue+0x0000); // FSYS
  mosWrite32(fdEntries+0x0028+0x0008, (unsigned int)(&stdFiles[2])); // back to host environment
  mosWrite32(fdEntries+0x0028+0x000C, 0); // transfer size
  mosWrite32(fdEntries+0x0028+0x0010, 0); // transfer buffer address
  
  // create the argv array
  const char **srcArgv = argv;
  int srcArgc = argc;
  if (argv[1] && strcmp(argv[1], "---run")==0) {
    runExternal = 1;
    srcArgv+=2;
    srcArgc-=2;
  } else if (!gAppResource) {
    srcArgv++;
    srcArgc--;
  }
  
  mosPtr vArgv = mosNewPtr((srcArgc+1)*4);
  for (i=0; i<srcArgc; i++) {
    const char *arg = srcArgv[i];
    // TODO: spot tripple-dash commands and take them off the list
    // TODO: argv[0] should only be the filename (MacOS has a 32 byte limt here!
    if (i==0) {
      arg = mosFilenameName(arg);
    } else {
      if (arg[0]!='-')
        arg = mosFilenameConvertTo(arg, MOS_TYPE_MAC);
    }
    // TODO: spot path names and convert them to Mac format if they are in Unix/OSX format
    mosWrite32(vArgv+4*i, mosNewPtr(arg));
  }
  
  // TODO: envp support
  
  // create the MPW memory table that allows tools to connect back to MPW
  mosPtr mpwMem = mosNewPtr(0x0028);
  mosWrite16(mpwMem+0x0000, 0x5348);
  mosWrite32(mpwMem+0x0002, srcArgc); // argc
  mosWrite32(mpwMem+0x0006, vArgv); // argv
  mosWrite32(mpwMem+0x000A, 0); // envp
  mosWrite32(mpwMem+0x000E, 0); // NULL
  mosWrite32(mpwMem+0x0012, 0); // unknown
  mosWrite32(mpwMem+0x0016, 0); // unknown
  mosWrite16(mpwMem+0x001A, 400); // file table size
  mosWrite32(mpwMem+0x001C, fdEntries); // table of file descriptor
  mosWrite32(mpwMem+0x0020, ioGlue);    // table of file functions
  mosWrite32(mpwMem+0x0024, 0); // unknown
  
  // create the MPW master pointer
  mosPtr mpwHandle = mosNewPtr(8);
  mosWrite32(mpwHandle+0x0000, 'MPGM'); // ID
  mosWrite32(mpwHandle+0x0004, mpwMem); // data
  
  gMosMPWHandle = mpwHandle;
  
  return runExternal;
}


/**
 * This is a collection of breakpoints that I used to understand ARM6asm.
 *
 * This function will go away entirely. It's sepcific to the MPW tool that
 * is being debugged.
 */
void setBreakpoints()
{
  // Breakpoints for ARM6asm only
  //  addBreakpoint(2, 0x0000003C, "SADEV: _coWrite");
  //  addBreakpoint(2, 0x000000B2, "SADEV: _fsWrite");
  //  addBreakpoint(2, 0x00000116, "SADEV: _syWrite");
  //  addBreakpoint(4, 0x00000185, "STDIO print");
  //  addBreakpoint(1, 0x0001DC14, "RTInit");
  //  addBreakpoint(8, 0x000002AE, "Patched LoadSeg");
  //  addBreakpoint(8, 0x0000032A, "Just before Load_Code");
  //  addBreakpoint(8, 0x00000004);
  
  //  addBreakpoint(7, 0x000007E6, "_faccess()");
  //  addBreakpoint(7, 0x00000000, "faccess()");
  //  addBreakpoint(7, 0x000004BA, "Pre BlockMove in _initIOPtable");
  //  addBreakpoint(7, 0x000004F2, "_initIOPtable");
  //  addBreakpoint(7, 0x00000838, "_faccess");
  //  addBreakpoint(7, 0x00000026, "open");
  //  addBreakpoint(7, 0x0000003C, "open2");
  //  addBreakpoint(7, 0x0000005A, "open3");
  
  //  addBreakpoint(1, 0x00003E70, "Usage");
  //  addBreakpoint(7, 0x000000DA, "Exit");

  //addBreakpoint(8, 0x00000000, "Launch");
}


/**
 * Main entry point.
 *
 * \todo run stdout through a Mac-to-Unix filter
 * \todo TripleDash arguments?   ARM6asm ---unix2mac myFile.s -o ---mac2unix myFile.o ---stdout2unix ---stderr2unix
 */
int main(int argc, const char **argv, const char **envp)
{
#ifdef MOS_UNITTESTS
  mosMemoryUnittests();
#else
  const char *appName = NULL;

  FILE *logFile = 0L;
  // logFile = fopen("/Users/matt/dev/Alienate/log.txt", "wb");
  // logFile = stdout;
  if (logFile)
    mosLogTo(logFile);
  
  setBreakpoints();
  
  int runExternal = setupSystem(argc, argv, envp);
  int runAsAlias = strcmp(mosFilenameNameUnix(argv[0]), "mosrun");
  int appLoaded = 0;
  
  if (runExternal || runAsAlias) {
    if (runAsAlias)
      appName = mosFilenameNameUnix(argv[0]);
    else
      appName = argv[2];

    appLoaded = loadApp(appName);
  }
  
  if (!appLoaded) {
    mosError("Can't find application data\n");
    exit(9);
  }
  
  runApp();

  if (logFile)
    fclose(logFile);
#endif
  return 0;
}




