//
// "$Id$"
//
// Dtk_Script implementation for the Dyne Toolkit.
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

#include "Dtk_Script.h"
#include "Dtk_Script_Writer.h"
#include "Dtk_Error.h"
#include "fltk/Fldtk_Script_Editor.h"
#include "fltk/Fldtk_Document_Tabs.h"
#include "fluid/main_ui.h"
#include "main.h"

#include <FL/filename.H>
#include <FL/Fl_File_Chooser.H>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include "NewtCore.h"
#include "NewtBC.h"
#include "NewtNSOF.h"
#include "NewtPkg.h"
#include "NewtPrint.h"
#include "NewtEnv.h"
#include "NewtVM.h"
}


/*---------------------------------------------------------------------------*/
Dtk_Script::Dtk_Script() 
: Dtk_Document(),
  editor_(0L)
{
}

/*---------------------------------------------------------------------------*/
Dtk_Script::~Dtk_Script()
{
    delete editor_;
}

/*---------------------------------------------------------------------------*/
int Dtk_Script::load()
{
	if (!editor_)
		edit();
	askForFilename_ = false; // FIXME only if the document was loaded successfully
	return editor_->loadFile(filename_);
}

/*---------------------------------------------------------------------------*/
void Dtk_Script::close() 
{
	if (editor_) {
		dtkDocumentTabs->remove(editor_);
		dtkDocumentTabs->redraw();
		delete editor_;
		editor_ = 0L;
	}
}

/*---------------------------------------------------------------------------*/
int Dtk_Script::edit() 
{
	if (!editor_) {
		Fl_Group::current(0L);
		editor_ = new Fldtk_Script_Editor(this);
		dtkDocumentTabs->add(editor_);
	}
	dtkDocumentTabs->value(editor_);
    return 0;
}


/*---------------------------------------------------------------------------*/
int Dtk_Script::save()
{
	if (askForFilename_) {
		return saveAs();
	}
	return editor_->saveFile(filename_);
	return 0;
}


/*---------------------------------------------------------------------------*/
int Dtk_Script::saveAs()
{
	char *filename = fl_file_chooser("Save Document As...", "*.txt", filename_);
	if (!filename) 
		return -1;
  char buf[2048];
  const char *ext = fl_filename_ext(filename);
  if (ext==0L || *ext==0) {
    strcpy(buf, filename);
    fl_filename_setext(buf, 2047, ".txt");
    filename = buf;
  }
	askForFilename_ = false;
	setFilename(filename);
	return save();
}


/*---------------------------------------------------------------------------*/
/*
	newtRefVar theForm, theFormCode = NBCCompileStr(
		"'{\r"
		"	title: \"UNGLAUBLICH!!!\", \r"
		"	viewBounds: { left: 2, top: 44, bottom: 340, right: 278}, \r"
		"	viewFormat: 83951953, \r"
		"	_proto: @157, \r"
		"	debug: \"helloBase\", \r"
		"	appSymbol: |Hello:SIG|\r"
		"};\r", true);

*/
/**
 * Compile the give script, and return a reference to the first literal.
 *
 * This is a temporary and ugly way to generate something, *anything*,
 * that will half-way run on a Newton device. The code above is probably 
 * the absolute minimum to get a window displayed.
 *
 * Nevertheless, this is a neat start for something big!
 */
newtRef Dtk_Script::compile()
{
	char *script = editor_->getText();
#if 0
	newtRef code = NBCCompileStr(script, true);
	newtRef form = kNewtRefNIL;
	if (NewtRefIsFrame(code)) {
		int32_t ix = NewtFindSlotIndex(code, NSSYM(literals));
		if (ix>=0) {
			newtRef literals = NewtGetFrameSlot(code, ix);
			form = NewtGetArraySlot(literals, 0);
		}
	} else {
		printf("***** Syntax error!\n");
	}
#else
  newtErr	err;
  newtRef form = NVMInterpretStr(script, &err);
	//NewtPrintObject(stdout, form);
	if (form==kNewtRefUnbind) {
		printf("**** ERROR while compiling or interpreting\n");
    if (err)
      printf("**** %s: %s\n", newt_error_class(err), newt_error(err));
    else 
      printf("**** unknow error\n");
		return kNewtRefUnbind;
	} else {
	}
#endif
  
	return form;
}

/*---------------------------------------------------------------------------*/
int Dtk_Script::write(Dtk_Script_Writer &sw) 
{ 
    char buf[1024];
    sprintf(buf, "// Beginning of file %s\n", name());
    sw.put(buf);
    char *script = editor_->getText();
    sw.put(script);
    free(script);
    sprintf(buf, "\n// End of file %s\n\n", name());
    sw.put(buf);
    return -1; 
}

//
// End of "$Id$".
//
