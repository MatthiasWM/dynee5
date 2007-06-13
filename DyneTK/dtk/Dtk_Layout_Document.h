//
// "$Id$"
//
// Dtk_Layout_Document header file for the Dyne Toolkit.
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

#ifndef DTK_LAYOUT_DOCUMENT_H
#define DTK_LAYOUT_DOCUMENT_H


#include <dtk/Dtk_Document.h>


class Dtk_Template;
class Fldtk_Layout_Editor;
class Fl_Hold_Browser;

/*---------------------------------------------------------------------------*/
/**
 * Base class for any kind of document that we can view, edit, or even compile.
 */
class Dtk_Layout_Document : public Dtk_Document
{
public:
					Dtk_Layout_Document(Dtk_Document_List *list);
	virtual			~Dtk_Layout_Document();

	virtual int		load();
	virtual int     edit();
	virtual int		save();
	virtual int		saveAs();
	virtual void	close();
	virtual int		getID() { return 0; }

    Fl_Hold_Browser * templateBrowser();
    Fl_Hold_Browser * slotBrowser();

private:
    void            rebuildTemplateBrowser();

    /// The root element of the tree of all templates inside this layout.
    Dtk_Template    * root_;

    /// Editor in the document tabs.
    Fldtk_Layout_Editor * editor_;

    /// Visual layout editor.
    // Fldtk_Visual_Layout * visualEditor_;

    /// GUI callback whenever a new template is selected
    static void templateBrowser_cb(Fl_Hold_Browser*, Dtk_Layout_Document*);
};


#endif

//
// End of "$Id$".
//
