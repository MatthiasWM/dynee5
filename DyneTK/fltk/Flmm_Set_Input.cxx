//
// "$Id$"
//
// Flmm_Set_Input implementation for the FLMM extension to FLTK.
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

#ifdef WIN32
#pragma warning(disable : 4996)
#endif


#include "Flmm_Set_Input.H"


#include <stdlib.h>
#include <string.h>


Flmm_Set_Input::Flmm_Set_Input(int X, int Y, int W, int H, const char *L)
:	Fl_Input(X, Y, W, H, L),
	data_(0L)
{
}


Flmm_Set_Input::~Flmm_Set_Input()
{
	if (data_)
		free(data_);
}


const char *Flmm_Set_Input::get()
{
	return data_;
}


void Flmm_Set_Input::set(const char *d)
{
	if (data_) {
		free(data_);
		data_ = 0L;
	}
	if (d) {
		data_ = strdup(d);
	}
}


void Flmm_Set_Input::update_data()
{
	set(value());
}


void Flmm_Set_Input::update_widget()
{
	value(get());
}



//
// End of "$Id$".
//
