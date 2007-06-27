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

#include "main.h"
#include "allNewt.h"

#include <FL/filename.h>
#include <assert.h>


/*-v2------------------------------------------------------------------------*/
Dtk_Document::Dtk_Document(Dtk_Document_List *list)
:	shortname_(0L),
	filename_(0L),
	name_(0L),
    browserName_(0L),
	askForFilename_(false),
    list_(list)
{
}

/*-v2------------------------------------------------------------------------*/
Dtk_Document::~Dtk_Document()
{
    // close all GUI elements
    close();

    // remove all references to this document
    list_->remove(this);

    // remove all our resources
	if (browserName_)
        free(browserName_);
	if (shortname_)
		free(shortname_);
	if (filename_)
		free(filename_);
}

/*-v2------------------------------------------------------------------------*/
const char *Dtk_Document::name() 
{
	return name_;
}

/*-v2------------------------------------------------------------------------*/
void Dtk_Document::updateBrowserName(bool tellTheList) 
{
	if (browserName_)
        free(browserName_);
    char buf[128];
    buf[0] = 0;
    if (list_ && list_->getMain()==this) {
        strcpy(buf, "*");
    }
    strcat(buf, "\t");
    strcat(buf, name_);
    browserName_ = strdup(buf);
    if (list_ && tellTheList)
        list_->filenameChanged(this);
}

/*-v2------------------------------------------------------------------------*/
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
        updateBrowserName();
	}
    list_->filenameChanged(this);
}


/*-v2------------------------------------------------------------------------*/
void Dtk_Document::setAskForFilename(bool v)
{
	askForFilename_ = v;
}


/*---------------------------------------------------------------------------*/
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

/*-v2------------------------------------------------------------------------*/
Dtk_Project *Dtk_Document::project()
{
    assert(list_);
    return list_->project();
}


/*-v2------------------------------------------------------------------------*/
void Dtk_Document::setMain()
{
    assert(list_);
    list_->setMain(this);
}

/*-v2------------------------------------------------------------------------*/
void Dtk_Document::setList(Dtk_Document_List *list)
{
    list_ = list;
}

//
// End of "$Id$".
//
