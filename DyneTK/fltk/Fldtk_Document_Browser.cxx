//
// "$Id$"
//
// Fldtk_Document_Browser implementation for the FLMM extension to FLTK.
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


#include "Fldtk_Document_Browser.h"


#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*----------------------------------------------------------------------------*/
Fldtk_Document_Browser::Fldtk_Document_Browser(int X, int Y, int W, int H, const char *L)
:	Fl_Hold_Browser(X, Y, W, H, L)
{
}


/*----------------------------------------------------------------------------*/
Fldtk_Document_Browser::~Fldtk_Document_Browser()
{
}


/*----------------------------------------------------------------------------*/
void Fldtk_Document_Browser::add(const char *name, void *data, bool isMain)
{
  char buf[2048];
  makeLabel_(buf, name, isMain);
  Fl_Hold_Browser::add(buf, data);
}


/*----------------------------------------------------------------------------*/
void Fldtk_Document_Browser::text(int i, const char *name, bool isMain)
{  
  char buf[2048];
  makeLabel_(buf, name, isMain);
  Fl_Hold_Browser::text(i, buf);
}
  
/*----------------------------------------------------------------------------*/
void Fldtk_Document_Browser::makeLabel_(char *buf, const char *name, bool isMain)
{  
  if (!name || !*name) 
    name = " - unnamed - ";
  if (isMain) {
    sprintf(buf, "*\t%s", name);
  } else {
    sprintf(buf, "\t%s", name);
  }
}

/*----------------------------------------------------------------------------*/
void Fldtk_Document_Browser::remove(int i) 
{
  Fl_Hold_Browser::remove(i);
}

//
// End of "$Id$".
//
