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


/**
 * Container for a document editor.
 */
class Fldtk_Script_Editor : public Fldtk_Editor
{
public:
	Fldtk_Script_Editor(Dtk_Document *doc);
	virtual ~Fldtk_Script_Editor();
	virtual int loadFile(const char*);
};


#endif

//
// End of "$Id$".
//
