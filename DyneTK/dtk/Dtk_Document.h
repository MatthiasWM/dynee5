//
// "$Id$"
//
// Dtk_Document header file for the Dyne Toolkit.
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

#ifndef DTK_DOCUMENT_H
#define DTK_DOCUMENT_H


class Fldtk_Editor;

extern "C" {
#include "NewtType.h"
}

class Dtk_Document_List;
class Dtk_Project;

/*---------------------------------------------------------------------------*/
/**
 * Base class for any kind of document that we can view, edit, or even compile.
 */
class Dtk_Document
{
public:
					Dtk_Document(Dtk_Document_List *list);
	virtual			~Dtk_Document();

	virtual int		load() { return -1; }
	virtual void	edit();
	virtual int		save() { return -1; }
	virtual int		saveAs() { return -1; }
	virtual void	close();
	virtual int		getID() { return -1; }
	virtual	newtRef	compile() { return kNewtRefNIL; }

	void			setFilename(const char *filename);
	void			setAskForFilename(bool v=true);
	const char		*name();
	bool			shown();
	bool			topMost();
	bool			isInProject() { return isInProject_; }
	newtRef			getProjectItemRef();

    Dtk_Project     * project();
    int             isDirty() { return 0; }

// don't use the following functions:
	void			setInProject(bool v) { isInProject_ = v; }

protected:
	Fldtk_Editor	* editor_;
	char			* shortname_;
	char			* filename_;
	char			* name_;
	bool			askForFilename_;
	bool			isInProject_;

    /// we must always be a member of exactly one list
    Dtk_Document_List   * list_;
};


#endif

//
// End of "$Id$".
//
