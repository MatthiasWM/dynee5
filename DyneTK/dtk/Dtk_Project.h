//
// "$Id$"
//
// Dtk_Project header file for the Dyne Toolkit.
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

#ifndef DTK_PROJECT_H
#define DTK_PROJECT_H

extern "C" {
#include "NewtType.h"
}


/*---------------------------------------------------------------------------*/
/**
 * Manage multiple documents that make up a project and generate and application.
 */
class Dtk_Project
{
public:
	Dtk_Project();
	~Dtk_Project();

	void		setDefaults();
	int			load();
	int			save();
	void		close() {}

	int			buildPackage();
	int			savePackage();

	char		* getPackageName();
	void		setFilename(const char *filename);
	newtRef		makeFileRef(const char *filename);

private:
	char		* packagename_;
	char		* shortname_;
	char		* filename_;
	char		* name_;
	newtRef		package_;
};


#endif

//
// End of "$Id$".
//
