//
// "$Id$"
//
// Fldtk_Script_Editor header file for the FLMM extension to FLTK.
//
// Copyright 2002-2007 by Matthias Melcher.
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

#ifndef FLDTK_SCRIPT_EDITOR_H
#define FLDTK_SCRIPT_EDITOR_H


#include "fltk/Fldtk_Editor.h"


class Dtk_Script;
class Flmm_Newt_Script_Editor;


/** GUI for editing Newt Scripts.
 */
class Fldtk_Script_Editor : public Fldtk_Editor
{
public:
	                        Fldtk_Script_Editor(Dtk_Script *script);
	virtual                 ~Fldtk_Script_Editor();
    virtual Dtk_Document    * document();

	int		                loadFile(const char *filename);
	int		                saveFile(const char *filename);
	char	                * getText();
    
    /*
	virtual int loadFile(const char*);
public:
	Fldtk_Editor(Dtk_Document *doc);
	virtual ~Fldtk_Editor();

	void	setName(const char *name);
    Dtk_Document *document() { return document_; }

protected:
	Fl_Text_Editor	* editor_;
    Dtk_Document *document_;
*/
protected:
    Dtk_Script         * script_;
    Flmm_Newt_Script_Editor     * editor_;
};


#endif

//
// End of "$Id$".
//
