//
// "$Id$"
//
// Dtk_Script_Document implementation for the Dyne Toolkit.
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

#include "Dtk_Script_Document.H"
#include "fltk/Fldtk_Script_Editor.H"
#include "fltk/Fldtk_Document_Tabs.H"
#include "fluid/main_ui.h"
#include "main.h"

#include <FL/filename.h>
#include <FL/fl_file_chooser.h>

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
/**
 * Constructor.
 */
Dtk_Script_Document::Dtk_Script_Document()
:	Dtk_Document()
{
}


/*---------------------------------------------------------------------------*/
/**
 * Destructor.
 */
Dtk_Script_Document::~Dtk_Script_Document()
{
}


/*---------------------------------------------------------------------------*/
/**
 * Load a script from disk.
 */
int Dtk_Script_Document::load()
{
	if (!editor_)
		edit();
	askForFilename_ = false; // FIXME only if the document was loaded successfuly
	return editor_->loadFile(filename_);
}


/*---------------------------------------------------------------------------*/
/**
 * Create an editor for the script file and show it.
 */
void Dtk_Script_Document::edit() 
{
	if (!editor_) {
		Fl_Group::current(0L);
		editor_ = new Fldtk_Script_Editor(fl_filename_name(filename_));
		dtkMain->document_tabs->add(editor_);
	}
	dtkMain->document_tabs->value(editor_);
}


/*---------------------------------------------------------------------------*/
/** 
 * Save the script to disk.
 */
int Dtk_Script_Document::save()
{
	if (askForFilename_) {
		saveAs();
	}
	return editor_->saveFile(filename_);
	return 0;
}


/*---------------------------------------------------------------------------*/
/**
 * Ask for a new filename, then save the script.
 */
int Dtk_Script_Document::saveAs()
{
	char *filename = fl_file_chooser("Save Document As...", "*.txt", filename_);
	if (!filename) 
		return -1;
	askForFilename_ = false;
	setFilename(filename);
	return save();
}


/*---------------------------------------------------------------------------*/
/**
 * Remove the editor for the script.
 */
void Dtk_Script_Document::close() 
{
	Dtk_Document::close();
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
newtRef Dtk_Script_Document::compile()
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
	if (NewtRefIsFrame(form)) {
	} else {
		printf("***** Syntax error!\n");
	}
#endif

	return form;
}

//
// End of "$Id$".
//
