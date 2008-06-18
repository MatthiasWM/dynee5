//
// "$Id$"
//
// Flmm_Set_Check_Button header file for the FLMM extension to FLTK.
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

#ifndef FLMM_SET_CHECK_BUTTON_H
#define FLMM_SET_CHECK_BUTTON_H


#include <FL/Fl_Check_Button.H>


/**
 * The regular Fl_Check_Button plus some functions to make this easily usable
 * in a Settings or Preferences dialog.
 */
class Flmm_Set_Check_Button : public Fl_Check_Button
{
public:
				Flmm_Set_Check_Button(int X, int Y, int W, int H, const char *L=0L);
	virtual		~Flmm_Set_Check_Button();
	int			get();
	void		set(int);
	void		update_data();
	void		update_widget();
private:
	int			data_;
};


#endif

//
// End of "$Id$".
//
