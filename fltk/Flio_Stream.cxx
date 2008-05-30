//
// "$Id$"
//
// Flio_Serial_Port implementation for the FLIO extension to FLTK.
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
// Please report all bugs and problems to "flio@matthiasm.com".
//


#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif


#include "Flio_Stream.h"

#include <FL/Fl.H>
#include <FL/fl_draw.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


Flio_Stream::Flio_Stream(int X, int Y, int W, int H, const char *L)
: Fl_Box(X, Y, W, H, L),
  super_(0L)
{
}


Flio_Stream::Flio_Stream(Flio_Stream *super)
: Fl_Box(0, 0, 0, 0),
  super_(super)
{
}


Flio_Stream::~Flio_Stream()
{
}

int Flio_Stream::open(const char *port, int bps)
{
	return -1;
}

int Flio_Stream::write(const unsigned char *data, int n)
{
	return -1;
}

int Flio_Stream::available() 
{
	return -1;
}

int Flio_Stream::read(unsigned char *dest, int n)
{
	return -1;
}

void Flio_Stream::close()
{
}

int Flio_Stream::is_open()
{
	return 0;
}

int Flio_Stream::on_read()
{
	return 0;
}

//
// End of "$Id$".
//
