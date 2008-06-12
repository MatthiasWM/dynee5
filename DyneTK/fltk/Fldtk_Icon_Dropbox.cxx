//
// "$Id$"
//
// Fldtk_Icon_Dropbox implementation for the FLMM extension to FLTK.
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


#include "Fldtk_Icon_Dropbox.h"

#include <stdio.h>
#include <FL/Fl.H>
#include <FL/Fl_Shared_image.H>


/*---------------------------------------------------------------------------*/
Fldtk_Icon_Dropbox::Fldtk_Icon_Dropbox(int x, int y, int w, int h, const char *name)
: Fl_Box(x, y, w, h, name),
  bpp_(1),
  image_(0L)
{
}


/*---------------------------------------------------------------------------*/
int Fldtk_Icon_Dropbox::handle(int event) 
{
  switch (event) {
    case FL_DND_ENTER:
      return 1;
    case FL_DND_DRAG:
      return 1;
    case FL_DND_RELEASE:
      return 1;
    case FL_PASTE:
      load(Fl::event_text());
      printf("Filename: %s\n", Fl::event_text());
      return 1;
  }
  return Fl_Box::handle(event);
}


/*---------------------------------------------------------------------------*/
void Fldtk_Icon_Dropbox::load(const char *filename)
{
  Fl_Shared_Image *si = Fl_Shared_Image::get(filename);
  Fl_Image *im = si->copy();
  if (im->d()==3) {
    Fl_RGB_Image *rgb = (Fl_RGB_Image*)im;
    image(0L);
    delete image_;
    image_ = rgb;
    image(image_);
    label(0);
  } else {
    image(0L);
    delete image_;
    image_ = 0L;
    label("default");
  }
}


//
// End of "$Id$".
//
