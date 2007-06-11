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


/*
                    Dtk_Document_List(Dtk_Project *proj=0l);
                    ~Dtk_Document_List();
	char		    * findFile(const char *filename);
	Dtk_Document	* addDocument(const char *filename);
	Dtk_Document	* newScript(const char *filename);
	Dtk_Document	* newLayout(const char *filename);
	Dtk_Document	* getDocument(int i);
	newtRef			getProjectItemsRef();
	void			setBrowser(Fldtk_Document_Browser*);
*/




#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "Dtk_Document_List.h"
#include "Dtk_Document.h"
#include "Dtk_Layout_Document.h"
#include "Dtk_Script_Document.h"

#include "fltk/Fldtk_Document_Browser.h"

#include "fluid/main_ui.h"
#include "main.h"

#ifdef WIN32
# include <io.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "allNewt.h"

#include <FL/filename.H>
#include <FL/Fl_File_Chooser.H>



/*---------------------------------------------------------------------------*/
Dtk_Document_List::Dtk_Document_List(Dtk_Project *proj)
:   project_(proj),
    browser_(0L)
{
    if (project_) {
        browser_ = dtkDocumentBrowser;
        browser_->activate();
        browser_->redraw();
        //++ browser_->callback(show, this) // allow user to click on a name to pop that document up
    }
}


/*---------------------------------------------------------------------------*/
Dtk_Document_List::~Dtk_Document_List()
{
    int i, n = docList_.size();
    for (i=n-1; i>=0; --i) {
        if (browser_) {
            browser_->remove(i+1);
        }
        Dtk_Document *doc = docList_.at(i);
        delete doc;
    }
    if (project_) {
        browser_->deactivate();
        browser_->redraw();
        browser_->callback(0L, 0L);
    }
    browser_ = 0L;
    project_ = 0L;
}

/*---------------------------------------------------------------------------*/
Dtk_Document *Dtk_Document_List::add(const char *filename)
{
    // determine the file type by loading the first few bytes
	FILE *f = fopen(filename, "rb");
	int id = fgetc(f);
	fclose(f);
	Dtk_Document *doc = 0L;
	if (id==2) { 
        // its NSOF, so for now we assume it is a layout
		doc = new Dtk_Layout_Document(this);
	} else { 
        // otherwise, this is likely text
		doc = new Dtk_Script_Document(this);
	}
    // load the file
	doc->setFilename(filename);
    doc->load();
    doc->edit();
    append_(doc);
	return doc;
}

/*---------------------------------------------------------------------------*/
char *Dtk_Document_List::findFile(const char *filename)
{
	if (access(filename, 0004)==0) // R_OK
		return strdup(filename);
	const char *name = fl_filename_name(filename);
	char buf[FL_PATH_MAX];
	fl_filename_absolute(buf, FL_PATH_MAX, name);
	if (access(buf, 0004)==0) // R_OK
		return strdup(buf);
	const char *user = fl_file_chooser("File not found, please search manually", 0, buf);
	if (!user)
		return 0L;
	return strdup(user);
}

/*---------------------------------------------------------------------------*/
Dtk_Document *Dtk_Document_List::newScript(const char *filename)
{
	Dtk_Script_Document *doc = new Dtk_Script_Document(this);
	doc->setFilename(filename);
	doc->setAskForFilename();
    append_(doc);
	return doc;
}

/*---------------------------------------------------------------------------*/
Dtk_Document *Dtk_Document_List::newLayout(const char *filename)
{
	Dtk_Layout_Document *doc = new Dtk_Layout_Document(this);
	doc->setFilename(filename);
	doc->setAskForFilename();
    append_(doc);
	return doc;
}

/*---------------------------------------------------------------------------*/
newtRef Dtk_Document_List::getProjectItemsRef()
{
	int i = 0, n = docList_.size();

	newtRef items = NewtMakeArray(kNewtRefNIL, n);

    for (i=0; i<n; ++i) {
        Dtk_Document *doc = docList_.at(i);
		NewtSetArraySlot(items, i, doc->getProjectItemRef());
	}

	return items;
}

/*---------------------------------------------------------------------------*/
Dtk_Document *Dtk_Document_List::getDocument(int i)
{
    if (i<0 || i>=docList_.size())
        return 0L;
    return docList_.at(i);
}

/*---------------------------------------------------------------------------*/
void Dtk_Document_List::append_(Dtk_Document *doc)
{
    docList_.push_back(doc);
    if (browser_) {
        browser_->add(doc->name(), doc);
    }
}

/*---------------------------------------------------------------------------*/
int Dtk_Document_List::remove(Dtk_Document *doc)
{
    int i, n = docList_.size();
    // search the list for this document
    for (i=0; i<n; ++i) {
        if (docList_.at(i)==doc) {
            docList_.erase(i);
            if (browser_)
                browser_.remove(i+1);
        }
    }
    // not found
    return -1;
}




#ifdef IGNORE_ME


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
 * We will communicate directly with the browser.
 */
void Dtk_Document_Manager::setBrowser(Fldtk_Document_Browser *b)
{
	browser_ = b;
}


#endif


//
// End of "$Id$".
//
