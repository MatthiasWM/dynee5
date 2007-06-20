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


class Dtk_Document_List;
class Dtk_Script_Writer;


/*---------------------------------------------------------------------------*/
/**
 * Manage multiple documents that make up a project and generate and application.
 */
class Dtk_Project
{
public:
	Dtk_Project();
	~Dtk_Project();

    /** Check if the project has changed and needs to be saved.
     * \todo Implement me!
     */
    int         isDirty() { return 0; }

	void		setDefaults();
	int			load();
	int			loadMac();
	int			loadWin();
	int			save();

	int			write(Dtk_Script_Writer &sw);

    /** Save all dirty parts of the project.
     */
    int			saveAll();

    /** Close and delete all dependent.
     * \todo Implement me!
     */
	void		close() {}

	int			buildPackage();
	int			savePackage();

	char		* getPackageName();
	void		setFilename(const char *filename);
	newtRef		makeFileRef(const char *filename);

    const char  * name() { return name_; }
    const char  * filename() { return filename_; }
    Dtk_Document_List   * documentList() { return documentList_; }

protected:
	void		pushDir();
	void		popDir();

private:
	char		* packagename_;
	char		* shortname_;
	char		* filename_;
	char		* name_;
	char		* startdir_;
	newtRef		package_;

    /// Keep a list of all documents in the project.
    Dtk_Document_List   * documentList_;
};


#endif

//
// End of "$Id$".
//
