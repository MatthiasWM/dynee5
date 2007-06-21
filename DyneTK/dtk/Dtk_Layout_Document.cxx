//
// "$Id$"
//
// Dtk_Layout_Document implementation for the Dyne Toolkit.
//
// Copyright 2007 by Matthias Melcher.
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
// Please report all bugs and problems to "dtk@matthiasm.com".
//

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "dtk/Dtk_Layout_Document.h"
#include "dtk/Dtk_Template.h"
#include "dtk/Dtk_Slot.h"
#include "dtk/Dtk_Script_Writer.h"
#include "fltk/Fldtk_Layout_Editor.h"
#include "fltk/Fldtk_Layout_View.h"
#include "fltk/Fldtk_Slot_Editor_Group.h"
#include "fltk/Fldtk_Document_Tabs.h"
#include "fluid/main_ui.h"
#include "main.h"

#include <FL/filename.h>
#include <FL/fl_file_chooser.h>
#include <FL/Fl_Hold_Browser.H>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*---------------------------------------------------------------------------*/
Dtk_Layout_Document::Dtk_Layout_Document(Dtk_Document_List *list)
:   Dtk_Document(list),
    root_(0L),
    editor_(0L),
    view_(0L)
{
}


/*---------------------------------------------------------------------------*/
Dtk_Layout_Document::~Dtk_Layout_Document()
{
    editor_->templateBrowser()->callback(0L, 0L);
    editor_->slotBrowser()->callback(0L, 0L);
    delete editor_;
    delete view_;
    delete root_;
}


/*---------------------------------------------------------------------------*/
int Dtk_Layout_Document::load()
{
	if (!editor_)
		edit();
	askForFilename_ = false; // FIXME only if the document was loaded successfuly

    uint8_t *buffer;
    FILE *f = fopen(filename_, "rb");
	if (!f) {
    	SystemAlert("Can't open layout file");
		return -1;
	}
    fseek(f, 0, SEEK_END);
    int nn = ftell(f);
    fseek(f, 0, SEEK_SET);
    buffer = (uint8_t*)malloc(nn);
    int n = fread(buffer, 1, nn, f);
    fclose(f);

    if (n) {
        root_ = new Dtk_Template(this, 0L);
		newtRef obj = NewtReadNSOF(buffer, n);
		newtRef hrc = NewtGetFrameSlot(obj, NewtFindSlotIndex(obj, NSSYM(templateHierarchy)));
		if (NewtRefIsFrame(hrc)) {
            // load an MSWindow layout
			root_->load(hrc);
		} else {
            // load a Mac layout
			root_->load(obj);
		}
        // FIXME: we still need to load global layout settings, like layout size, etc.
	}
    setupEditors();
    return 0;
}


/*---------------------------------------------------------------------------*/
int Dtk_Layout_Document::edit() 
{
	if (!editor_) {
		Fl_Group::current(0L);
		editor_ = new Fldtk_Layout_Editor(this);
		dtkDocumentTabs->add(editor_);
	}
	dtkDocumentTabs->value(editor_);
    if (!view_) {
        Fl_Group::current(0L);
        view_ = new Fldtk_Layout_View(this, 320, 480);
    }
    return 0;
}

/*---------------------------------------------------------------------------*/
void Dtk_Layout_Document::editView() 
{
    view_->show();
}

/*---------------------------------------------------------------------------*/
int Dtk_Layout_Document::save()
{
	if (askForFilename_) {
		return saveAs();
	}
	// FIXME write the code to save out the text
	return 0;
}


/*---------------------------------------------------------------------------*/
int Dtk_Layout_Document::saveAs()
{
	char *filename = fl_file_chooser("Save Document As...", "*.lyt", filename_);
	if (!filename) 
		return -1;
	askForFilename_ = false;
	setFilename(filename);
	return save();
}


/*---------------------------------------------------------------------------*/
void Dtk_Layout_Document::close() 
{
	Dtk_Document::close();
}

/*---------------------------------------------------------------------------*/
void Dtk_Layout_Document::setupEditors() 
{
    if (!editor_)
        return;
    Fl_Hold_Browser *b = editor_->templateBrowser();
    b->clear();
    if (!root_)
        return;
    int indent = 0;
    int index = 1;
    layoutView()->begin();
    root_->updateBrowserLink(b, indent, index, true);
    layoutView()->end();
    b->callback((Fl_Callback*)templateBrowser_cb, this);
    slotBrowser()->callback((Fl_Callback*)slotBrowser_cb, this);
}

/*---------------------------------------------------------------------------*/
Fl_Hold_Browser *Dtk_Layout_Document::templateBrowser() {
    return editor_->templateBrowser();
}

/*---------------------------------------------------------------------------*/
Fl_Hold_Browser *Dtk_Layout_Document::slotBrowser() {
    return editor_->slotBrowser();
}

/*---------------------------------------------------------------------------*/
Fldtk_Slot_Editor_Group *Dtk_Layout_Document::slotEditor() {
    return editor_->slotEditor();
}

/*---------------------------------------------------------------------------*/
void Dtk_Layout_Document::templateBrowser_cb(Fl_Hold_Browser *browser, Dtk_Layout_Document *layout)
{
    int i = browser->value();
    if (i) {
        layout->slotEditor()->blank();
        Dtk_Template *tmpl = (Dtk_Template*)browser->data(i);
        tmpl->edit();
    } else {
        layout->slotBrowser()->clear();
        layout->slotEditor()->blank();
    }
    layout->view_->redraw();
    UpdateMainMenu();
}

/*---------------------------------------------------------------------------*/
void Dtk_Layout_Document::slotBrowser_cb(Fl_Hold_Browser *browser, Dtk_Layout_Document *layout)
{
    int i = browser->value();
    if (i) {
        Dtk_Slot *slot = (Dtk_Slot*)browser->data(i);
        slot->edit();
    } else {
        layout->slotEditor()->blank();
    }
    UpdateMainMenu();
}

/*---------------------------------------------------------------------------*/
int Dtk_Layout_Document::write(Dtk_Script_Writer &sw) 
{ 
    char buf[1024];
    sprintf(buf, "// Beginning of file %s\n", name());
    sw.put(buf);
    if (root_) {
        root_->write(sw);
        sprintf(buf, "constant |layout_%s| := %s;\n", name(), root_->scriptName());
        sw.put(buf);
    }
    sprintf(buf, "// End of file %s\n\n", name());
    sw.put(buf);
    return -1; 
}

//
// End of "$Id$".
//
