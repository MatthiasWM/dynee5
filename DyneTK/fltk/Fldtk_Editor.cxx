//
// "$Id$"
//
// Fldtk_Editor implementation for the FLMM extension to FLTK.
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

#include "Fldtk_Editor.h"

#include "main.h"
#include "fltk/Fldtk_Document_Tabs.h"
#include "dtk/Dtk_Document.h"

#include <FL/Fl.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>

#include <stdio.h>
#include <stdlib.h>


Fldtk_Editor::Fldtk_Editor(Dtk_Document *doc)
:   Fl_Group(dtkDocumentTabs->x(), dtkDocumentTabs->y()+25, 
             dtkDocumentTabs->w(), dtkDocumentTabs->h()-25),
    document_(doc)
{
    const char *L = 0L;
    if (doc)
        L = doc->name();
	if (L)
		copy_label(L);
	labelsize(12);

	editor_ = new Fl_Text_Editor(x()+2, y()+2, w()-4, h()-4);
	editor_->buffer(new Fl_Text_Buffer());
	editor_->textfont(FL_COURIER);
	editor_->textsize(12);
	editor_->box(FL_FLAT_BOX);
	resizable(editor_);
}


Fldtk_Editor::~Fldtk_Editor()
{
}


void Fldtk_Editor::setName(const char *name) 
{
	copy_label(name);
}


int Fldtk_Editor::loadFile(const char *filename)
{
	return editor_->buffer()->loadfile(filename);
}


int Fldtk_Editor::saveFile(const char *filename)
{
	return editor_->buffer()->savefile(filename);
}

char *Fldtk_Editor::getText() {
	return editor_->buffer()->text();
}

//
// End of "$Id$".
//
