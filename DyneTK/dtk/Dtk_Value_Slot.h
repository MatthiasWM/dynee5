//
// "$Id$"
//
// Dtk_Value_Slot header file for the Dyne Toolkit.
//
// Copyright 2007 by Matthias Melcher.
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
// Please report all bugs and problems to "dtk@matthiasm.com".
//

#ifndef DTK_VALUE_SLOT_H
#define DTK_VALUE_SLOT_H


#include "Dtk_Slot.h"
#include "fltk/Flmm_Signal.h"

class Fldtk_Value_Slot_Editor;


/** 
 * Manage a value slot inside a template.
 */
class Dtk_Value_Slot : public Dtk_Slot
{
public:
  
  /** 
   * Initialize a value slot
   */
  Dtk_Value_Slot(Dtk_Slot_List *list, const char *key, newtRef slot);
  
  /** 
   * Destroy a value slot
   */
  virtual         ~Dtk_Value_Slot();
  
  /** 
   * Pop up the value slot editor.
   */
  virtual void    edit();

  /**
   * Close the value slot editor.
   */
  virtual void close();
  
  /** 
   * Return the value
   */
  double          value() { return value_; }

  /**
   * Set the value.
   */
  void            value(double v) { value_ = v; }
  
  /** 
   * Write this slot as a Newt Script.
   *
   * \retval 0 if successful
   * \retval negative if an error occured
   */     
  virtual int		write(Dtk_Script_Writer &sw);
  
  /**
   * Apply the changes in the editor to the slot.
   */
  virtual void    apply();
  
  /** 
   * Revert the changes in the editor to the current slot sttings.
   */
  virtual void    revert();
  
  /** 
   * This signal will be called whenever the user applies changes to the value.
   */
  Flmm_Signal     signalValueChanged;
  
  /**
   * Create a frame that can be saved to a layout file.
   */
  virtual newtRefVar save();
  
private:
  
  /// this is the editor that we are using
  Fldtk_Value_Slot_Editor    * editor_;
  
  /// the rectangle iteslf
  double          value_;
};


#endif

//
// End of "$Id$".
//
