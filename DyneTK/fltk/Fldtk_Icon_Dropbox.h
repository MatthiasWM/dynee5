//
// "$Id$"
//
// Fldtk_Icon_Dropbox header file for the FLMM extension to FLTK.
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

#ifndef FLDTK_ICON_DROPBOX_H
#define FLDTK_ICON_DROPBOX_H


#include <FL/Fl_Box.H>


/** 
 * An FLTK widget that will accept graphic files that are dropped on it,
 * prviding a NS icon container.
 */
class Fldtk_Icon_Dropbox : public Fl_Box
{
public:
	Fldtk_Icon_Dropbox(int x, int y, int w, int h, const char *name=0L);

  /**
   * Set the bit depth for this image.
   */
  void depth(int d) { bpp_ = d; }

  /**
   * Handle drag and drop for this widget.
   */
  int handle(int event);

private:

  /// bits per pixel
  int bpp_; 

};


#endif

//
// End of "$Id$".
//
