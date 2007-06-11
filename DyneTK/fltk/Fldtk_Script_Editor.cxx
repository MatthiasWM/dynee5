//
// "$Id$"
//
// Fldtk_Script_Editor implementation for the FLMM extension to FLTK.
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

#include "Fldtk_Script_Editor.h"

#include <FL/Fl.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>

#include <stdio.h>
#include <stdlib.h>


Fldtk_Script_Editor::Fldtk_Script_Editor(const char *L)
: Fldtk_Editor(L)
{
}


Fldtk_Script_Editor::~Fldtk_Script_Editor()
{
}

int Fldtk_Script_Editor::loadFile(const char *filename) 
{
	// read the text
	FILE *in = fopen(filename, "rb");
	fseek(in, 0, SEEK_END);
	int n = ftell(in);
	rewind(in);
	char *txt = (char*)malloc(n+1);
	fread(txt, 1, n, in);
	txt[n] = 0;
	fclose(in);
	// convert file endings into '\n'
	char *src = txt, *dst = txt;
	for (;;) {
		char c = *src++;
		if (!c) break;
		if (c=='\r') {
			if (*src=='\n') {
				*dst++ = *src++;
			} else {
				*dst++ = '\n';
			}
		} else {
			*dst++ = c;
		}
	}
	*dst = 0;
	editor_->buffer()->text(txt);
	free(txt);
    return 0;
}


//
// End of "$Id$".
//
