//
// "$Id$"
//
// Dtk_Document implementation for the Dyne Toolkit.
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

#include "Dtk_Document.h"
#include "Dtk_Document_List.h"
#include "Dtk_Project.h"
#include "fltk/Fldtk_Editor.h"
#include "fltk/Fldtk_Document_Tabs.h"
#include "fluid/main_ui.h"
#include "main.h"

#include <FL/filename.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

extern "C" {
#include "NewtCore.h"
#include "NewtBC.h"
#include "NewtNSOF.h"
#include "NewtPkg.h"
#include "NewtPrint.h"
#include "NewtEnv.h"
}


/*---------------------------------------------------------------------------*/
/**
 * Constructor.
 */
Dtk_Document::Dtk_Document(Dtk_Document_List *list)
:	editor_(0L),
	shortname_(0L),
	filename_(0L),
	name_(0L),
	askForFilename_(false),
    list_(list)
{
}


/*---------------------------------------------------------------------------*/
/** 
 * Destructor.
 */
Dtk_Document::~Dtk_Document()
{
    // close all GUI elements
    close();

    // remove all references to this document
    list_->remove(this);

    // remove all our resources
	if (shortname_)
		free(shortname_);
	if (filename_)
		free(filename_);
}


/*---------------------------------------------------------------------------*/
/**
 * Open the editor matching this document type.
 */
void Dtk_Document::edit() 
{
	if (!editor_) {
		Fl_Group::current(0L);
		editor_ = new Fldtk_Editor(this);
		dtkMain->document_tabs->add(editor_);
	}
	dtkMain->browsers->value(editor_);
}


/*---------------------------------------------------------------------------*/
/**
 * Return the name of the file.
 */
const char *Dtk_Document::name() 
{
	return name_;
}


/*---------------------------------------------------------------------------*/
/**
 * Set a new filename for this document.
 *
 * \todo	we must also update the file browser
 */
void Dtk_Document::setFilename(const char *filename)
{
	if (shortname_)
		free(shortname_);
	if (filename_)
		free(filename_);
	if (filename) {
		filename_ = strdup(filename);
		name_ = (char*)fl_filename_name(filename_);
		const char *ext = fl_filename_ext(name_);
		int n = ext-name_;
		shortname_ = (char*)calloc(n+1, 1);
		memcpy(shortname_, name_, n);
	}
	if (editor_)
		editor_->setName(name_);
	// documents->updateDocName(this);
}


/*---------------------------------------------------------------------------*/
/**
 * Set a flag that will make DTK ask for a filename even if we Save instead of Save As.
 */
void Dtk_Document::setAskForFilename(bool v)
{
	askForFilename_ = v;
}


/*---------------------------------------------------------------------------*/
/**
 * Return true, if the attached document editor is the current and top most one.
 */
bool Dtk_Document::topMost() 
{
	if (editor_ && editor_->visible())
		return true;
	return false;
}


/*---------------------------------------------------------------------------*/
/**
 * Remove the editor from the global view.
 */
void Dtk_Document::close() 
{
	if (editor_) {
		dtkMain->document_tabs->remove(editor_);
		delete editor_;
		editor_ = 0L;
		dtkMain->document_tabs->redraw();
	}
}


/*---------------------------------------------------------------------------*/
/**
 * Create a Frame that we can use to reference this document from within a package.
 */
newtRef Dtk_Document::getProjectItemRef()
{
	newtRefVar itemA[] = {
		NSSYM(file),			dtkProject->makeFileRef(filename_), 
		NSSYM(type),			NewtMakeInt30(getID()), 
		NSSYM(isMainLayout),	kNewtRefNIL,
	};
	newtRef item = NewtMakeFrame2(sizeof(itemA) / (sizeof(newtRefVar) * 2), itemA);
	return item;
}

Dtk_Project *Dtk_Document::project()
{
    assert(list_);
    return list_->project();
}

//
// End of "$Id$".
//
