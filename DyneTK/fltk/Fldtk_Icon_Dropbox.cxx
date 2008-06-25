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
#include <string.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <FL/Fl.H>
#include <FL/Fl_Shared_Image.H>


/*---------------------------------------------------------------------------*/
Fldtk_Icon_Dropbox::Fldtk_Icon_Dropbox(int x, int y, int w, int h, const char *name)
: Fl_Box(x, y, w, h, name),
  bpp_(1),
  image_(0L),
  filename_(0L),
  tmpFilename_(0L)
{
}


/*---------------------------------------------------------------------------*/
Fldtk_Icon_Dropbox::~Fldtk_Icon_Dropbox()
{
  if (tmpFilename_)
    free(tmpFilename_);
  if (filename_)
    free(filename_);
  if (image_)
    delete image_;
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
      tmpFilename(Fl::event_text());
      load(tmpFilename_);
      do_callback();
      return 1;
  }
  return Fl_Box::handle(event);
}


/*---------------------------------------------------------------------------*/
void Fldtk_Icon_Dropbox::tmpFilename(const char *filename)
{
  if (tmpFilename_)
    free(tmpFilename_);
  if (filename)
    tmpFilename_ = strdup(filename);
  else 
    tmpFilename_ = 0L;
}


/*---------------------------------------------------------------------------*/
void Fldtk_Icon_Dropbox::load(const char *filename)
{
  image(0L);
  delete image_;
  image_ = 0L;

  if (!filename || !*filename) {
    label("default");
  } else {
    if (access(filename, 4)==-1) {
      label("not\nfound");
    } else {
      Fl_Shared_Image *si = Fl_Shared_Image::get(filename);
      if (!si) {
        label("can't\nread");
      } else {
        Fl_Image *im = si->copy();
        if (im->d()!=3) {
          label("invalid\ndepth");
        } else {
          image_ = (Fl_RGB_Image*)im;
          image(image_);
          label(0);
        }
      }
    }
  }
}


/*---------------------------------------------------------------------------*/
void Fldtk_Icon_Dropbox::filename(const char *fn)
{
  if (filename_)
    free(filename_);
  if (fn)
    filename_ = strdup(fn);
  else
    filename_ = 0L;
}


/*---------------------------------------------------------------------------*/
void Fldtk_Icon_Dropbox::setImageFilename(const char *fn)
{
  filename(fn);
  load(fn);
}


/*---------------------------------------------------------------------------*/
void Fldtk_Icon_Dropbox::update_data()
{
  filename(tmpFilename_);
  load(filename_);
}


/*---------------------------------------------------------------------------*/
void Fldtk_Icon_Dropbox::update_widget()
{
  tmpFilename(filename_);
  load(filename_);
}


/*---------------------------------------------------------------------------*/
newtRef Fldtk_Icon_Dropbox::makeBitmap(int &w, int &h, bool isMask)
{
  // if there is no image, try to load it
  if (!image_) {
    load(filename_);
  }
  // if there is still no image, or the image is in the wrong format, give up
  if (!image_ || image_->d()!=3)
    return kNewtRefUnbind;
  // convert the RGB image into a bitmap
  w = image_->w(); h = image_->h();
  int i, j, bpr = (w/32+1)*4;
  int size = 16+bpr*h;
  uint8_t *bits = (uint8_t*)calloc(size, 1), *dst = bits;
  const uint8_t *src = (const uint8_t*)image_->data()[0];
  dst[5]  = bpr; // bytes per row
  dst[13] = h;   // height
  dst[15] = w;   // width
  dst += 16;
  for (i=0; i<h; i++) {
    for (j=0; j<w; j++) {
      int off = j/8, mod = j%8;
      uint8_t c = *src; src += 3;
      if (c<127)
        dst[off] |= (128>>mod);
    }
    dst += bpr;
  }
  newtRef ret = NewtMakeBinary(isMask?NSSYM(mask):NSSYM(bits), bits, size, false);
  free(bits);
  return ret;
}


/*---------------------------------------------------------------------------*/
newtRef Fldtk_Icon_Dropbox::defaultBitmap(int &w, int &h)
{
  static uint8_t bits[] = {
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x18,
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x3f, 0x00, 
	  0x00, 0x00, 0x33, 0x00, 0x1f, 0xff, 0x7b, 0x00, 0x3f, 0xff, 0x6e, 0x00, 0x30, 0x00, 0xce, 0x00, 
	  0x37, 0xfc, 0xcc, 0x00, 0x37, 0xfd, 0x9c, 0x00, 0x34, 0x05, 0x98, 0x00, 0x35, 0x53, 0x38, 0x00, 
	  0x34, 0x03, 0x30, 0x00, 0x35, 0x56, 0x70, 0x00, 0x34, 0x06, 0x60, 0x00, 0x35, 0x54, 0xe0, 0x00, 
	  0x34, 0x0c, 0xc0, 0x00, 0x35, 0x4f, 0xc0, 0x00, 0x34, 0x0b, 0x80, 0x00, 0x36, 0x0f, 0x00, 0x00, 
	  0x37, 0xfe, 0x80, 0x00, 0x37, 0xfd, 0x80, 0x00, 0x30, 0x0b, 0x80, 0x00, 0x18, 0x03, 0x00, 0x00, 
	  0x1f, 0xff, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };
  w = 24; h = 27;
  return NewtMakeBinary(NSSYM(bits), bits, sizeof(bits), false);
}


/*---------------------------------------------------------------------------*/
newtRef Fldtk_Icon_Dropbox::defaultMask(int &w, int &h)
{
  static uint8_t mask[] = {
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x18, 
	  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x3f, 0x00, 
	  0x00, 0x00, 0x3f, 0x00, 0x1f, 0xff, 0x7f, 0x00, 0x3f, 0xff, 0x7e, 0x00, 0x3f, 0xff, 0xfe, 0x00, 
	  0x3f, 0xff, 0xfc, 0x00, 0x3f, 0xff, 0xfc, 0x00, 0x3f, 0xff, 0xf8, 0x00, 0x3f, 0xff, 0xf8, 0x00, 
	  0x3f, 0xff, 0xf0, 0x00, 0x3f, 0xff, 0xf0, 0x00, 0x3f, 0xff, 0xe0, 0x00, 0x3f, 0xff, 0xe0, 0x00,
	  0x3f, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0xc0, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x3f, 0xff, 0x00, 0x00, 
	  0x3f, 0xff, 0x80, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x3f, 0xff, 0x80, 0x00, 0x1f, 0xff, 0x00, 0x00, 
	  0x1f, 0xff, 0x00, 0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };
  w = 24; h = 27;
  return NewtMakeBinary(NSSYM(mask), mask, sizeof(mask), false);
}


/*---------------------------------------------------------------------------*/
newtRef Fldtk_Icon_Dropbox::buildIcon(Fldtk_Icon_Dropbox *bits, Fldtk_Icon_Dropbox *mask, int mode)
{
  /// \todo \i mode is ignored for now
  newtRefVar bBits = kNewtRefUnbind;
  newtRefVar bMask = kNewtRefUnbind;
  int bw=0, bh=0, mw=0, mh=0;

  // load the bitmap
  bBits = bits->makeBitmap(bw, bh);
  if (bBits==kNewtRefUnbind) {
    bBits = defaultBitmap(bw, bh);
  }

  // load or create the mask
  if (mask) {
    bMask = mask->makeBitmap(mw, mh);
  }
  if (bMask == kNewtRefUnbind || bw!=mw || bh!=mh) {
    bMask = bits->makeMask(mw, mh);
  }
  if (bMask == kNewtRefUnbind || bw!=mw || bh!=mh) {
    bMask = defaultMask(mw, mh);
  }

  // create the remaining bits for the icon frame
	newtRefVar iconBoundsA[] = {
		NSSYM(left),			NewtMakeInt30(0),
		NSSYM(top),				NewtMakeInt30(0),
		NSSYM(bottom),		NewtMakeInt30(bh),
		NSSYM(right),			NewtMakeInt30(bw)
	};
	newtRef iconBounds = NewtMakeFrame2(sizeof(iconBoundsA) / (sizeof(newtRefVar) * 2), iconBoundsA);
	
	newtRefVar iconA[] = {
		NSSYM(bits),			bBits,
		NSSYM(mask),			bMask,
		NSSYM(bounds),		iconBounds,
	};
  newtRef icon = NewtMakeFrame2(sizeof(iconA) / (sizeof(newtRefVar) * 2), iconA);

  return icon;
}


//
// End of "$Id$".
//
