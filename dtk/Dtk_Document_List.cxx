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

#include "Dtk_Document_List.h"
#include "Dtk_Document.h"
#include "Dtk_Layout.h"
#include "Dtk_Script.h"

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
: project_(proj),
main_(0L),
wDocumentBrowser_(0L)
{
  if (proj) {
    wDocumentBrowser_ = dtkDocumentBrowser;
    wDocumentBrowser_->callback((Fl_Callback*)wDocumentBrowser_cb, this);
  }
}


/*---------------------------------------------------------------------------*/
Dtk_Document_List::~Dtk_Document_List()
{
  clear();
  project_ = 0L;
}


/*---------------------------------------------------------------------------*/
void Dtk_Document_List::append(Dtk_Document *doc)
{
  docList_.push_back(doc);
  doc->setList(this);
  if (wDocumentBrowser_)
    wDocumentBrowser_->add(doc->name(), doc, doc==main_);
}


/*---------------------------------------------------------------------------*/
int Dtk_Document_List::remove(Dtk_Document *doc)
{
  // search the list for this document
  int i, n = docList_.size();
  for (i=n-1; i>=0; --i) {
    if (docList_.at(i)==doc) {
      // if found, make sure we do not reference this item anymore
      if (main_==doc)
        setMainDocument(0L);
      // now remove it from the browser
      if (wDocumentBrowser_) {
        if (wDocumentBrowser_->value()==i+1)
          wDocumentBrowser_->value(0);
        wDocumentBrowser_->remove(i+1);
      }
      // take it out of the list
      docList_.erase(docList_.begin()+i);
      // and remove the documents link back to us
      doc->setList(0L);
      return 0;
    }
  }
  // not found
  return -1;
}


/*---------------------------------------------------------------------------*/
void Dtk_Document_List::clear()
{
  int i, n = docList_.size();
  for (i=n-1; i>=0; --i) {
    Dtk_Document *doc = docList_.at(i);
    doc->clear();
    remove(doc);
    delete doc;
  }
}





/*---------------------------------------------------------------------------*/
Dtk_Document *Dtk_Document_List::add(const char *filename)
{
  // determine the file type by loading the first few bytes
	FILE *f = fopen(filename, "rb");
  if (!f)
    return 0L;
	int id = fgetc(f);
	fclose(f);
	Dtk_Document *doc = 0L;
	if (id==2) { 
    // its NSOF, so for now we assume it is a layout
		doc = new Dtk_Layout();
	} else { 
    // otherwise, this is likely text
		doc = new Dtk_Script();
	}
  // link the document to this doc list
  append(doc);
	doc->setFilename(filename);
  doc->load();
  doc->edit();
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
	Dtk_Script *doc = new Dtk_Script();
  append(doc);
	doc->setFilename(filename);
	doc->setAskForFilename();
	return doc;
}

/*---------------------------------------------------------------------------*/
Dtk_Document *Dtk_Document_List::newLayout(const char *filename)
{
	Dtk_Layout *doc = new Dtk_Layout();
  append(doc);
	doc->setFilename(filename);
	doc->setAskForFilename();
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
Dtk_Document *Dtk_Document_List::at(int i)
{
  if (i<0 || i>=(int)docList_.size())
    return 0L;
  return docList_.at(i);
}


/*-v2------------------------------------------------------------------------*/
void Dtk_Document_List::wDocumentBrowser_cb(Fldtk_Document_Browser *w, Dtk_Document_List *d)
{
  int it = w->value();
  if (it==0)
    return;
  Dtk_Document *doc = (Dtk_Document*)w->data(it);
  if (doc)
    doc->edit();
  UpdateMainMenu();
}

/*-v2------------------------------------------------------------------------*/
void Dtk_Document_List::documentNameChanged(Dtk_Document *document)
{
  // if we have no browser, we don't care
  int i, n = docList_.size();
  // search the list for this document
  for (i=0; i<n; ++i) {
    if (docList_.at(i)==document) {
      if (wDocumentBrowser_)
        wDocumentBrowser_->text(i+1, document->name(), (document==main_));
      return;
    }
  }
}

/*-v2------------------------------------------------------------------------*/
void Dtk_Document_List::setMainDocument(Dtk_Document *document)
{
  if (main_==document)
    return;
  if (main_) {
    Dtk_Document *doc = main_;
    main_ = 0L;
    documentNameChanged(doc);
  }
  main_ = document;
  if (main_) {
    documentNameChanged(main_);
  }
}


#ifdef IGNORE_ME

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
 * Create a filename that is unique within the scope of the document manager.
 */
const char *Dtk_Document_Manager::uniqueFilename(const char *filename)
{
	return filename;
}

#endif


//
// End of "$Id$".
//
