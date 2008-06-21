//
// "$Id$"
//
// Flio_Inspector_Einstein implementation for the FLIO extension to FLTK.
//
// Copyright 2002-2007 by Matthias Melcher.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "flio@matthiasm.com".
//


#include "Flio_Inspector_Einstein.h"

#include "globals.h"
#include "allNewt.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <FL/Fl_Button.H>
#include <FL/Fl_Image.H>
#include <FL/filename.h>

#ifndef WIN32
# define Sleep(a) sleep((a)/1000)
//# define Sleep(a) usleep((a)*1000)
# include <unistd.h>
#else
# include <windows.h>
#endif

extern Fl_Button *wInspectorConnect;
extern Fl_Image *toolbox_open_pixmap;
extern Fl_Image *toolbox_closed_pixmap;
extern Fl_Window *wConnect;


Flio_Inspector_Einstein::Flio_Inspector_Einstein(int X, int Y, int W, int H, const char *L)
: Flio_Inspector(X, Y, W, H, L),
#ifdef __APPLE__
  cInst_(0),
#endif
  is_open_(false)
{
}


Flio_Inspector_Einstein::~Flio_Inspector_Einstein()
{
}


int Flio_Inspector_Einstein::sendScript(const char *script)
{
#ifdef __APPLE__
  return sendCmd(
    "tell application \"Einstein\"\r"
    " do newton script \"%s\"\r"
    "end tell\r",
    script);
#elif defined WIN32
  return sendCmd("dons", script);
#else
#endif
  return 0;
}
    

int Flio_Inspector_Einstein::sendCmd(const char *cmd, const char *script)
{
#ifdef __APPLE__
  if (script && cmd && cInst_) {
    int i, n = strlen(script), bs = 0;
    const char *s = script;
    for (i=0; i<n; i++) {
      char v = *s++;
      if (v=='"' || v=='\\' || v=='\r' || v=='\n') bs++;
    }
    char *esc = (char*)malloc(n+bs+1), *d = esc;
    s = script;
    for (i=0; i<n; i++) {
      char v = *s++;
      switch (v) {
        case '"':  *d++ = '\\'; *d++ = '"';  break;
        case '\\': *d++ = '\\'; *d++ = '\\'; break;
        case '\r': *d++ = '\\'; *d++ = 'r';  break;
        case '\n': *d++ = '\\'; *d++ = 'n';  break;
        default: *d++ = v;
      }
    }
    *d = 0;
    char *as = (char*)malloc(n+bs+strlen(cmd)+1);
    sprintf(as, cmd, esc);
    puts(as);
    
    AEDesc aeScript, aeResult;
    AECreateDesc(typeChar, as, strlen(as), &aeScript);
    AECreateDesc(typeChar, 0, 0, &aeResult);
    OSADoScript(cInst_, &aeScript, kOSANullScript, typeChar, 0, &aeResult);
    AEDisposeDesc(&aeResult);
    AEDisposeDesc(&aeScript);
    
    free(esc);
    free(as);
  }
#elif defined WIN32
  char inbuf[4096];
  char outbuf[4096];
  strcpy(outbuf, cmd);
  strcat(outbuf, script);
  DWORD n;
  LPTSTR name = TEXT("\\\\.\\pipe\\einstein"); 
  CallNamedPipe(
    name,
    (void*)outbuf, strlen(outbuf)+1,
    inbuf, 4096,
    &n, 5000);
  puts(inbuf);
#else
#endif
	return 0;

  /*
WIN32 possibilities:
  Scripting: no information found
  Broadcast Message: RegisterWindowMessage, BroadcastSystemMessage
  Named Pipes: complicated?
  Using Copy Data: does that work across apps?
  */
}


int Flio_Inspector_Einstein::deletePackage(const char *symbol) 
{
  char buf[2048];
  sprintf(buf, "SafeRemovePackage(GetPkgRef(\"%s\", GetStores()[0]));", symbol);
  return sendScript(buf);
}


int Flio_Inspector_Einstein::sendPackage(const char *filename) 
{
  char buf[2048];
  fl_filename_absolute(buf, 2048, filename);
#ifdef __APPLE__
  return sendCmd(
                 "tell application \"Einstein\"\r"
                 " install package \"%s\"\r"
                 "end tell\r", buf);
#else
  return sendCmd("inst", buf);
#endif
}


int Flio_Inspector_Einstein::open(const char *port, int bps)
{
#ifdef __APPLE__
  if (!cInst_)
    cInst_ = OpenDefaultComponent(kOSAComponentType, kOSAGenericScriptingComponentSubtype);
#endif
  is_open_ = true;
  on_connect();
  return 0;
}


void Flio_Inspector_Einstein::cancel()
{
  close();
}


void Flio_Inspector_Einstein::close()
{
  on_disconnect();
  is_open_ = false;
#ifdef __APPLE__
  if (cInst_) {
    CloseComponent(cInst_);
    cInst_ = 0L;
  }
#endif
}

//
// End of "$Id$".
//
