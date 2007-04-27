//
// "$Id$"
//
// Fldtk_Layout_Drop_Box implementation for the FLMM extension to FLTK.
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

#include "Fldtk_Layout_Drop_Box.h"
#include "globals.h"

#include <FL/Fl.H>
#include <FL/names.h>
#include <FL/fl_file_chooser.h>

extern "C" {
#include "NewtCore.h"
#include "NewtBC.h"
#include "NewtNSOF.h"
#include "NewtPkg.h"
#include "NewtPrint.h"
#include "NewtEnv.h"
}

#include <stdio.h>
#include <stdlib.h>


Fldtk_Layout_Drop_Box::Fldtk_Layout_Drop_Box(int X, int Y, int W, int H, const char *L)
: Fl_Button(X, Y, W, H, L)
{
	win = 0;
}


Fldtk_Layout_Drop_Box::~Fldtk_Layout_Drop_Box()
{
	delete win;
}


int Fldtk_Layout_Drop_Box::handle(int event) 
{
	//printf("Event %s\n", fl_eventnames[event]);
	switch (event) {
	case FL_DND_ENTER:
	case FL_DND_DRAG:
	case FL_DND_RELEASE:
		return 1;
	case FL_PASTE:
		open(Fl::event_text());
		return 1;
	}
	return Fl_Button::handle(event);
}


void Fldtk_Layout_Drop_Box::open(const char *filename) 
{
//	NcSetGlobalVar(NSSYM(printLength), NSINT(9999));
	NcSetGlobalVar(NSSYM(printDepth), NSINT(10));
	NEWT_INDENT = 1;
    uint8_t *buffer;
    FILE *f = fopen(filename, "rb");
	if (!f) {
		filename = fl_file_chooser("Choose layout file to load", "", filename);
		if (!filename) 
			return;
		f = fopen(filename, "rb");
		if (!f) {
			SystemAlert("Can't open file");
			return;
		}
	}
    fseek(f, 0, SEEK_END);
    int nn = ftell(f);
    fseek(f, 0, SEEK_SET);
    buffer = (uint8_t*)malloc(nn);
    int n = fread(buffer, 1, nn, f);
    fclose(f);

	if (win) {
		delete win;
		win = 0L;
	}

    if (n) {
		newtRef obj = NewtReadNSOF(buffer, n);
		NewtPrintObject(stdout, obj);

		win = new Fl_Window(320, 480, "DyneTK Layout");
		Nt_Group *g = new Nt_Group(0, 0, 320, 480, 0);

		// try the MSWindows version of the layout file
		newtRef hrc = NewtGetFrameSlot(obj, NewtFindSlotIndex(obj, NSSYM(templateHierarchy)));
		if (NewtRefIsFrame(hrc)) {
			add_widget(hrc, g);
		} else { // try the MAC version
			add_widget(obj, g);
		}
		
		win->non_modal();
		win->show();
    } else {
		SystemAlert("Can't read file");
	}


}


void Fldtk_Layout_Drop_Box::add_widget(newtRef r, Nt_Group *w)
{
	newtRef v = NewtGetFrameSlot(r, NewtFindSlotIndex(r, NSSYM(value)));
	if (!NewtRefIsFrame(v))
		return;

	int32_t wl, wr, wt, wb;
	uint32_t justify = 0;
	char *text = 0L;
	{	// read the view bounds
		newtRef bnds = NewtGetFrameSlot(v, NewtFindSlotIndex(v, NSSYM(viewBounds)));
		if (NewtRefIsFrame(bnds)) {
			newtRef v = NewtGetFrameSlot(bnds, NewtFindSlotIndex(bnds, NSSYM(value)));
			if (NewtRefIsFrame(v)) {
				 newtRef i;
				 i = NewtGetFrameSlot(v, NewtFindSlotIndex(v, NSSYM(top)));
				 if (NewtRefIsInteger(i)) wt = NewtRefToInteger(i);
				 i = NewtGetFrameSlot(v, NewtFindSlotIndex(v, NSSYM(left)));
				 if (NewtRefIsInteger(i)) wl = NewtRefToInteger(i);
				 i = NewtGetFrameSlot(v, NewtFindSlotIndex(v, NSSYM(bottom)));
				 if (NewtRefIsInteger(i)) wb = NewtRefToInteger(i);
				 i = NewtGetFrameSlot(v, NewtFindSlotIndex(v, NSSYM(right)));
				 if (NewtRefIsInteger(i)) wr = NewtRefToInteger(i);
			}
		}
	}
	{	// read the view justify flags
		newtRef jst = NewtGetFrameSlot(v, NewtFindSlotIndex(v, NSSYM(viewJustify)));
		if (NewtRefIsFrame(jst)) {
			newtRef v = NewtGetFrameSlot(jst, NewtFindSlotIndex(jst, NSSYM(value)));
			if (NewtRefIsInteger(v)) justify = NewtRefToInteger(v);
		}
	}
	{	// read the label 
		newtRef txt = NewtGetFrameSlot(v, NewtFindSlotIndex(v, NSSYM(text)));
		if (NewtRefIsFrame(txt)) {
			newtRef v = NewtGetFrameSlot(txt, NewtFindSlotIndex(txt, NSSYM(value)));
			if (NewtRefIsString(v)) text = NewtRefToString(v);
		}
		if (!text) {
			newtRef v = NewtGetFrameSlot(r, NewtFindSlotIndex(r, NSSYM(__ntName)));
			if (NewtRefIsString(v)) text = NewtRefToString(v);
		}
	}


	Nt_Group *g = new Nt_Group(wl, wt, wr, wb, justify, text);
	g->align(FL_ALIGN_INSIDE | FL_ALIGN_WRAP);
	g->labelsize(9);

	
	{	// read the children
		newtRef c = NewtGetFrameSlot(v, NewtFindSlotIndex(v, NSSYM(stepChildren)));
		if (NewtRefIsFrame(c)) {
			newtRef a = NewtGetFrameSlot(c, NewtFindSlotIndex(c, NSSYM(value)));
			if (NewtRefIsArray(a)) {
				int i, n = NewtArrayLength(a);
				for (i=0; i<n; i++) {
					newtRef ai = NewtGetArraySlot(a, i); 
					add_widget(ai, g);
				}
			}
		}
	}

	g->end();
}


//
// End of "$Id$".
//
