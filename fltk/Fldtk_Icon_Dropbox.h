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
#include <FL/Fl_RGB_Image.H>

#include "allNewt.h"


/** 
 * An FLTK widget that will accept graphic files that are dropped on it,
 * prviding a NS icon container.
 */
class Fldtk_Icon_Dropbox : public Fl_Box
{
public:
	Fldtk_Icon_Dropbox(int x, int y, int w, int h, const char *name=0L);

	virtual ~Fldtk_Icon_Dropbox();

  /**
   * Set the bit depth for this image.
   */
  void depth(int d) { bpp_ = d; }

  /**
   * Handle drag and drop for this widget.
   */
  int handle(int event);
  
  /**
   * Set the main filename and load the image.
   */
  void setImageFilename(const char *filename);

  /**
   * Get the file name for this icon image.
   */
  const char *getImageFilename() { return filename_; }

  /**
   * Load a new image from disk.
   */
  void load(const char *filename);

  /**
   * Reload the image described by filename_.
   */
  void reload();

  /**
   * Update the internal data structure with the last change to the widget.
   */
  void update_data();

  /**
   * Restore the widget from the internal data structure.
   */
  void update_widget();

  /**
   * Convert the image file into a Newton bitmap.
   *
   * \param[out] w width of bitmap
   * \param[out] h height of bitmap
   * \param[in] isMask set this if you want to generate a mask instead
   *
   * \retval binary frame containing the bitmap
   * \retval kNewtRefUnbind if there was an error
   */
  newtRef makeBitmap(int &w, int &h, bool isMask=false);

  /**
   * Convert the image file into a Newton mask.
   *
   * \param[out] w width of bitmap
   * \param[out] h height of bitmap
   *
   * \retval binary frame containing the mask
   * \retval kNewtRefUnbind if there was an error
   */
  newtRef makeMask(int &w, int &h) { return makeBitmap(w, h, true); }

  /**
   * Return the default icon bitmap (a PDA with a pen).
   *
   * \param[out] w width of bitmap
   * \param[out] h height of bitmap
   *
   * \retval binary frame containing the bitmap
   */
  static newtRef defaultBitmap(int &w, int &h);

  /**
   * Return the default icon mask (a PDA with a pen).
   *
   * \param[out] w width of bitmap
   * \param[out] h height of bitmap
   *
   * \retval binary frame containing the mask
   */
  static newtRef defaultMask(int &w, int &h);

  /**
   * Return the NTK pre 1.6.1 icon frame.
   *
   * \param[in] bits a dropbox containing the bitmap image
   * \param[in] mask a dropbox containing the mask image
   * \param mode[in] how to combine image and mask into the icon
   *
   * \retval frame containing the icon
   * \retval if the process failed, this will return teh default icon
   */
  static newtRef buildIcon(Fldtk_Icon_Dropbox *bits, Fldtk_Icon_Dropbox *mask, int mode);

  /**
   * Update the temporary filename.
   *
   * This does not load the image file.
   */
  void tmpFilename(const char *fn);

  /**
   * Update the filename.
   *
   * This does not load the image file.
   */
  void filename(const char *fn);

private:

  /// bits per pixel
  int bpp_; 

  /// icon image
  Fl_RGB_Image *image_;

  /// file and path name of final image file.
  char *filename_;
  
  /// file and path name of temporary image file.
  char *tmpFilename_;
  
};


#endif

//
// End of "$Id$".
//
