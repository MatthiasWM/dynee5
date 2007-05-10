//
// "$Id: Flmp_Image.H 4650 2005-11-18 10:08:13Z matt $"
//
// Quicktime Player support for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2005 by Bill Spitzak and others.
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
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

#ifndef _FLMP_IMAGE_
#define _FLMP_IMAGE_

#include <FL/Fl.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Button.H>


class Flmp_Image : public Fl_RGB_Image
{
public:
  char save_jpeg(const char *name, int quality);
  Fl_Image *scale(int w, int h);
  Fl_Image *scale_to_fit(int w, int h);
  Fl_Image *scale_and_clip(int w, int h);
  void compose(Fl_Image *m, int dx, int dy, int dw, int dh, 
                              int sx, int sy, int sw, int sh);
  void compose_frame(Fl_Image *m, int x1, int y1, int x2, int y2);
  Fl_Image *rotate90();
  Fl_Image *rotate180();
  Fl_Image *rotate270();
  unsigned int get_pixel(int x, int y);
  void set_pixel(int x, int y, unsigned int color);
  void set_pixel_a(int x, int y, unsigned int color);
};


class Flmp_Image_Target : public Fl_Button 
{
protected:
  char *pFilename;
  Fl_Image *pImage;
  Fl_Image *pScaledImage;
  int handle(int event);
  void draw();
public:
  Flmp_Image_Target(int x, int y, int w, int h, const char *l=0L)
    : Fl_Button(x, y, w, h, l) { 
    pFilename = 0L;
    pImage = 0L;
    pScaledImage = 0L;
  }
  char load_image(const char *filename);
  const char *get_filename();
  void clear_image();
  const Fl_Image *get_image();
  void set_image(const Fl_Image*);
};

#endif

//
// End of "$Id: Flmp_Image.H 4650 2005-11-18 10:08:13Z matt $".
//
