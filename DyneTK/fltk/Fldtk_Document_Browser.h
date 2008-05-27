//
// "$Id$"
//
// Fldtk_Document_Browser header file for the FLMM extension to FLTK.
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

#ifndef FLDTK_DOCUMENT_BROWSER_H
#define FLDTK_DOCUMENT_BROWSER_H


#include <FL/Fl_Hold_Browser.H>


/**
 * The regular Fl_Input plus some functions to make this easily usable
 * in a Settings or Preferences dialog.
 */
class Fldtk_Document_Browser : public Fl_Hold_Browser
{
public:
  /**
   * Create a new document browser based on a Hold Browser.
   */
  Fldtk_Document_Browser(int X, int Y, int W, int H, const char *L=0L);
  
  /**
   * Delete the Document Browser.
   */
	virtual		~Fldtk_Document_Browser();
  
  /**
   * Add a new entry with a name, data pointe, and an indicator for the main document.
   */
  void add(const char *name, void *data, bool isMain);
  
  /**
   * Change the text on an existing entry.
   */
  void text(int i, const char *name, bool isMain);
  
  /**
   * Remove an item form the browser.
   */
  void remove(int i);
  
private:
  
  // make a label from a give item name and a flag
  void makeLabel_(char *buf, const char *name, bool isMain);
  
};


#endif

//
// End of "$Id$".
//
