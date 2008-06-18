//
// "$Id$"
//
// Flmm_Command_Editor header file for the FLMM extension to FLTK.
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

#ifndef FLMM_CONSOLE_H
#define FLMM_CONSOLE_H


#include <FL/Fl_Text_Editor.H>


/**
 * This is a quick shot to handle Ctrl-Enter to trigger
 * the interpreter.
 *
 * \todo Add text highlighting for errors, exceptions, replies, etc.
 * \todo Autoscroll so that all output remains visible
 * \todo Back up the contents into a file for reloading on the next launch
 */
class Flmm_Console : public Fl_Text_Editor
{
public:
  Flmm_Console(int X, int Y, int W, int H, const char *L=0L);
  virtual ~Flmm_Console();
  virtual int handle(int);
};


#endif

//
// End of "$Id$".
//
