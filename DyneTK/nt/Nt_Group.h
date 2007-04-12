//
// "$Id$"
//
// Nt_Group header file for the FLMM extension to FLTK.
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

#ifndef NT_GROUP_H
#define NT_GROUP_H


#include <FL/Fl_Group.H>


/**
 * The regular Fl_Input plus some functions to make this easily usable
 * in a Settings or Preferences dialog.
 */
class Nt_Group : public Fl_Group
{
public:
				Nt_Group(int X, int Y, int W, int H, const char *L=0L);
				Nt_Group(int L, int T, int R, int B, int J, const char *lbl=0L);
	virtual		~Nt_Group();
	void		layout(Fl_Group *par=0L, Fl_Widget *sib=0L);
	int			left, top, right, bottom, justify;
	
};


#endif

//
// End of "$Id$".
//
