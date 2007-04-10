//
// "$Id$"
//
// Fldtk_Editor header file for the FLMM extension to FLTK.
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

#ifndef FLDTK_EDITOR_H
#define FLDTK_EDITOR_H


#include <FL/Fl_Group.H>


class Fl_Text_Editor;


/**
 * Container for a document editor.
 */
class Fldtk_Editor : public Fl_Group
{
public:
	Fldtk_Editor(int X, int Y, int W, int H, const char *L=0L);
	Fldtk_Editor(const char *L=0L);
	void Fldtk_Editor_(const char *L=0L);
	virtual ~Fldtk_Editor();

	int		loadFile(const char *filename);
	int		saveFile(const char *filename);
	char	* getText();
	void	setName(const char *name);

protected:
	Fl_Text_Editor	*editor_;
};


#endif

//
// End of "$Id$".
//
