//
// "$Id$"
//
// Flmm_Command_Editor implementation for the FLMM extension to FLTK.
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
// Please report all bugs and problems to "flmm@matthiasm.com".
//

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "Flmm_Console.h"

#include "main.h"
#include "fltk/Flio_Inspector.h"

#include <FL/Fl.H>
#include <FL/fl_draw.h>

#include <stdio.h>
#include <stdlib.h>


Flmm_Console::Flmm_Console(int X, int Y, int W, int H, const char *L)
: Fl_Text_Editor(X, Y, W, H, L)
{
}


Flmm_Console::~Flmm_Console()
{
}


extern "C" int getNSO(const char *command, unsigned char **data, int *size);
extern void send_test(int ix);
extern unsigned char *nso;
extern int nNso;


int Flmm_Console::handle(int event) 
{
  if (event==FL_KEYDOWN) {
    if ( (Fl::event_key()==FL_Enter && Fl::event_ctrl()) 
       ||(Fl::event_key()==FL_KP_Enter) ) {
      int p, q;
      char *cmd;
      if (buffer()->selected()) {
        buffer()->selection_position(&q, &p);
        cmd = buffer()->selection_text();
        buffer()->unselect();
      } else {
        p = insert_position();
        cmd = buffer()->line_text(p);
        char *s = cmd;
        while (*s) {
          if (*s=='\n') 
            *s = '\r';
          s++;
        }
      }
      insert_position(buffer()->line_end(p));
      insert("\n");
      if (cmd && *cmd) {
        /*
        int err = getNSO(cmd, &nso, &nNso);
        if (!err && nso) {
          send_test(4);
        } else {
          insert("ERROR\n");
        }
        free(cmd);
         */
        wInspectorSerial->sendScript(cmd);
        return 1;
      }
    }
  }
  return Fl_Text_Editor::handle(event);
}

//
// End of "$Id$".
//
