//
// "$Id$"
//
// Dtk_Document_Manager implementation for the Dyne Toolkit.
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
// Please report all bugs and problems to "flmm@matthiasm.com".
//

// FIXME	Make sure that we can't add the same document twice. If we open the same
//			document, just bring it to the front.

// FIXME	For new documents, come up with a new name if the old one is taken.
//			Increment some number before the extension.


#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "Dtk_Document_Manager.h"
#include "Dtk_Document.h"
#include "Dtk_Layout_Document.h"
#include "Dtk_Script_Document.h"

#include "fltk/Fldtk_Document_Browser.h"

#include "fluid/main_ui.h"
#include "main.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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
Dtk_Document_Manager::Dtk_Document_Manager()
:	browser_(0L)
{
}


/*---------------------------------------------------------------------------*/
/**
 * Destructor.
 */
Dtk_Document_Manager::~Dtk_Document_Manager()
{
	if (browser_) {
		browser_->clear();
		browser_->deactivate();
	}
	project_docs.clear();
	visible_docs.clear();
}


/*---------------------------------------------------------------------------*/
/**
 * Create a document of unknown type.
 * \todo return a setup that show that this type is not editable (and unsupported)
 */
Dtk_Document *Dtk_Document_Manager::newDocument(const char *filename)
{
	Dtk_Script_Document *doc = new Dtk_Script_Document();
	doc->setFilename(uniqueFilename(filename));
	doc->setAskForFilename();
	addToVisible(doc);
	return doc;
}


/*---------------------------------------------------------------------------*/
/**
 * Create a new document that will contain scripts.
 */
Dtk_Document *Dtk_Document_Manager::newScript(const char *filename)
{
	Dtk_Script_Document *doc = new Dtk_Script_Document();
	doc->setFilename(uniqueFilename(filename));
	doc->setAskForFilename();
	addToVisible(doc);
	return doc;
}


/*---------------------------------------------------------------------------*/
/** 
 * Create a new Layout document.
 */
Dtk_Document *Dtk_Document_Manager::newLayout(const char *filename)
{
	Dtk_Layout_Document *doc = new Dtk_Layout_Document();
	doc->setFilename(uniqueFilename(filename));
	doc->setAskForFilename();
	addToVisible(doc);
	return doc;
}


/*---------------------------------------------------------------------------*/
/** 
 * Return the currently active document.
 */
Dtk_Document *Dtk_Document_Manager::getCurrentDoc()
{
	std::list<Dtk_Document*>::iterator it = visible_docs.begin();
	while (it!=visible_docs.end()) {
		if ((*it)->topMost())
			return (*it);
		++it;
	}
	return 0L;
}


/*---------------------------------------------------------------------------*/
/**
 * Add a document to the manager.
 */
void Dtk_Document_Manager::addToVisible(Dtk_Document *doc)
{
	assert(doc);
	visible_docs.push_back(doc);
}


/*---------------------------------------------------------------------------*/
/**
 * Add a document to the manager.
 */
void Dtk_Document_Manager::addToProject(Dtk_Document *doc)
{
	assert(doc);
	doc->setInProject(true);
	project_docs.push_back(doc);
	if (browser_) 
		browser_->add(doc->getName(), doc);
}


/*---------------------------------------------------------------------------*/
/**
 * Update the name of a document in the browser.
 */
void Dtk_Document_Manager::updateDocName(Dtk_Document *doc)
{
	int i = inProject(doc);
	if (i>=0)
		browser_->text(i+1, doc->getName());
}


/*---------------------------------------------------------------------------*/
/**
 * Remove a document from the manager.
 */
void Dtk_Document_Manager::removeVisibleDoc(Dtk_Document *doc)
{
	assert(doc);
	doc->close();
	visible_docs.remove(doc);
}


/*---------------------------------------------------------------------------*/
/**
 * Return the index of a document within the project, or -1.
 */
int Dtk_Document_Manager::inProject(Dtk_Document *doc)
{
	int i = 0;
	std::list<Dtk_Document*>::iterator it = project_docs.begin();
	for ( ; it!=project_docs.end(); ++it, ++i) {
		if ((*it)==doc) {
			return i;
		}
	}
	return -1;
}


/*---------------------------------------------------------------------------*/
/**
 * Remove a document from the manager.
 */
void Dtk_Document_Manager::removeProjectDoc(Dtk_Document *doc)
{
	assert(doc);
	int i = inProject(doc);
	if (i>=0)
		browser_->remove(i+1);
	doc->setInProject(false);
	project_docs.remove(doc);
}


/*---------------------------------------------------------------------------*/
/**
 * Create a filename that is unique within the scope of the document manager.
 */
const char *Dtk_Document_Manager::uniqueFilename(const char *filename)
{
	return filename;
}


/*---------------------------------------------------------------------------*/
/**
 * Return the number of documents that are part of the current project.
 */
int Dtk_Document_Manager::numVisibleDocs()
{
	return visible_docs.size();
}


/*---------------------------------------------------------------------------*/
/**
 * Return the number of documents that are part of the current project.
 */
int Dtk_Document_Manager::numProjectDocs()
{
	return project_docs.size();
}


/*---------------------------------------------------------------------------*/
/**
 * Get a document that is part of the project by index.
 */
Dtk_Document *Dtk_Document_Manager::getProjectDoc(int i)
{
	std::list<Dtk_Document*>::iterator it = project_docs.begin();
	for ( ; i>0; i--) ++it;
	return *(it);
}


/*---------------------------------------------------------------------------*/
/**
 * Create a Newt Array that conatins a list of all project documents.
 */
newtRef Dtk_Document_Manager::getProjectItemsRef()
{
	int i = 0, n = project_docs.size();

	newtRef items = NewtMakeArray(kNewtRefNIL, n);

	std::list<Dtk_Document*>::iterator it = project_docs.begin();
	while (it!=project_docs.end()) {
		NewtSetArraySlot(items, i++, (*it)->getProjectItemRef());
		++it;
	}

	return items;
}


/*---------------------------------------------------------------------------*/
/**
 * We will communicate directly with the browser.
 */
void Dtk_Document_Manager::setBrowser(Fldtk_Document_Browser *b)
{
	browser_ = b;
}

//
// End of "$Id$".
//
