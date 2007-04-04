//
// "$Id$"
//
// Dtk_Document_Manager header file for the Dyne Toolkit.
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

#ifndef DTK_DOCUMENT_MANAGER_H
#define DTK_DOCUMENT_MANAGER_H


#include <list>

extern "C" {
#include "NewtType.h"
}

class Dtk_Document;
class Fldtk_Document_Browser;


/*---------------------------------------------------------------------------*/
/**
 * Keep track of all open documents
 */
class Dtk_Document_Manager
{
public:
					Dtk_Document_Manager();
					~Dtk_Document_Manager();

	void			setBrowser(Fldtk_Document_Browser*);

	Dtk_Document	* newDocument(const char *filename);
	Dtk_Document	* newScript(const char *filename);
	Dtk_Document	* newLayout(const char *filename);

	// visible docs
	Dtk_Document	* getCurrentDoc();
	void			removeVisibleDoc(Dtk_Document*);
	int				numVisibleDocs();
	void			addToVisible(Dtk_Document*);

	// project docs
	Dtk_Document	* getProjectDoc(int i);
	void			removeProjectDoc(Dtk_Document*);
	int				numProjectDocs();
	void			addToProject(Dtk_Document*);
	void			clearProjectDocs();
	void			updateDocName(Dtk_Document*);
	int				inProject(Dtk_Document*);

	const char		*uniqueFilename(const char *filename);

	newtRef			getProjectItemsRef();

private:
	std::list<Dtk_Document*>	project_docs;
	std::list<Dtk_Document*>	visible_docs;
	Fldtk_Document_Browser		* browser_;
};


#endif

//
// End of "$Id$".
//
