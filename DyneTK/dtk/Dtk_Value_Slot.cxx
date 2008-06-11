//
// "$Id$"
//
// Dtk_Value_Slot implementation for the Dyne Toolkit.
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
// Please report all bugs and problems to "flmm@matthiasm.com".
//

/*
 A number slot can hold either an integer or a real number. 
 If you enter an integer in the range -536,870,912 to 536,870,911, 
 NTK stores it as type integer. If you enter an integer outside 
 that range or a number containing a decimal point, NTK stores 
 it as type real.
 */

#include "Dtk_Value_Slot.h"
#include "Dtk_Layout.h"
#include "Dtk_Script_Writer.h"
#include "fltk/Fldtk_Value_Slot_Editor.h"
#include "fltk/Fldtk_Format_Slot_Editor.h"
#include "fltk/Fldtk_Justify_Slot_Editor.h"
#include "fltk/Fldtk_Slot_Editor_Group.h"

#include "allNewt.h"
#include "globals.h"

#include <string.h>
#include <math.h>


/*---------------------------------------------------------------------------*/
Dtk_Value_Slot::Dtk_Value_Slot(Dtk_Slot_List *list, const char *theKey, newtRef slot)
: Dtk_Slot(list, theKey, slot),
  editor_(0L),
  value_(0.0)
{
  if (slot!=kNewtRefUnbind) {
    newtRef v = NewtGetFrameSlot(slot, NewtFindSlotIndex(slot, NSSYM(value)));
    if (NewtRefIsReal(v)) {
      value_ = NewtRefToReal(v);
    } else if (NewtRefIsInteger(v)) {
      value_ = (double)NewtRefToInteger(v);
    } else {
      printf("############################# Can't read number!\n"); // FIXME
      EnterDebugger();
    }
    newtRef dt = NewtGetFrameSlot(slot, NewtFindSlotIndex(slot, NSSYM(__ntDatatype)));
    if (NewtRefIsString(dt)) {
      datatype_ = strdup(NewtRefToString(dt));
    }
  }
  if (!datatype_)
    datatype_ = strdup("NUMB");
}


/*---------------------------------------------------------------------------*/
Dtk_Value_Slot::~Dtk_Value_Slot()
{
  close();
}


/*---------------------------------------------------------------------------*/
void Dtk_Value_Slot::edit()
{
  Fldtk_Slot_Editor_Group *container = layout()->slotEditor();
  if (!editor_) {
    container->begin();
    if (strcasecmp(key_, "viewJustify")==0)
      editor_ = new Fldtk_Justify_Slot_Editor(container, this);
    else if (strcasecmp(key_, "viewFormat")==0)
      editor_ = new Fldtk_Format_Slot_Editor(container, this);
    else
      editor_ = new Fldtk_Value_Slot_Editor(container, this);
    container->end();
    editor_->value(value_);
  }
  container->value(editor_);
}


/*---------------------------------------------------------------------------*/
void Dtk_Value_Slot::close()
{
  Fldtk_Slot_Editor_Group *container = layout()->slotEditor();
  if (editor_) {
    container->remove(editor_);
    delete editor_;
    editor_ = 0L;
  }
}


/*---------------------------------------------------------------------------*/
int Dtk_Value_Slot::write(Dtk_Script_Writer &sw)
{
  char buf[1024], val[32];
  
  double i, f = fabs(modf(value_, &i));
  if (f>0.0)
    sprintf(val, "%.f%0f", i, f);
  else
    sprintf(val, "%.f", i);
  
  sprintf(buf, "     %s: %s", key_, val);
  sw.put(buf);
  return 0;
}


/*---------------------------------------------------------------------------*/
void Dtk_Value_Slot::apply() 
{ 
  if (editor_) {
    value_ = editor_->value();
  }
  signalValueChanged(this);
}


/*---------------------------------------------------------------------------*/
void Dtk_Value_Slot::revert() 
{
  if (editor_) {
    editor_->value(value_);
  }
}


/*---------------------------------------------------------------------------*/
newtRef	Dtk_Value_Slot::save()
{
  newtRefVar slotA[] = {
    NSSYM(value),   kNewtRefNIL,
  NSSYM(__ntDataType), NewtMakeString(datatype_, false) };
  
  double i, f = fabs(modf(value_, &i));
  if (f>0.0)
    slotA[1] = NewtMakeReal(value_);
  else
    slotA[1] = NewtMakeInteger((int)value_);
  
  newtRef slot = NewtMakeFrame2(2, slotA);
  
  return slot;
}


//
// End of "$Id$".
//
