//
// "$Id$"
//
// Dtk_Platform header file for the Dyne Toolkit.
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

#ifndef DTK_PLATFORM_H
#define DTK_PLATFORM_H


#include <map>

extern "C" {
#include "NewtType.h"
}

struct Fl_Menu_Item;
class Dtk_Template;
class Dtk_Script_Writer;

/** Read a platform file and provide database and GUI access.
 */
class Dtk_Platform
  {
  public:
    
    /** 
     * Initialize the platform.
     */
    Dtk_Platform(const char *ptfFilename, const char *constFilename);
    
    /** 
     * Remove a platform.
     */
    virtual         ~Dtk_Platform();
    
    /** 
     *Load a platform file from disk.
     */
    int loadPtfFile();
    
    /**
     * Load the paltform constants from "Newton21.txt"
     */
    int loadConstFile();
    void writeConstants(Dtk_Script_Writer &sw);
    
    /** 
     * Return the FLTK menu items for the template choice menu.
     */
    Fl_Menu_Item    * templateChoiceMenu();
    
    /** Return the FLTK menu items for the specific choice menu.
     */
    Fl_Menu_Item    * specificChoiceMenu(Dtk_Template *tmpl);
    
    /** Return the FLTK menu items for the "Methods" choice menu.
     */
    Fl_Menu_Item    * methodsChoiceMenu();
    
    /** Return the FLTK menu items for the "Attributes" choice menu.
     */
    Fl_Menu_Item    * attributesChoiceMenu();
    
    /** Deactivate menu items with slots that already exist.
     */
    static void     updateActivation(Fl_Menu_Item *menu, Dtk_Template *tmpl);
    
    /** Sort an FLTK menu array in alphabetic order.
     */
    static void     sort(Fl_Menu_Item *menu, int n=-1);
    
    /** Create a form with the default attributes for a template.
     */
    newtRef         newtTemplate(char *id);
    
    /** Create a newt frame containing a slot.
     */
    newtRef         getSpecificSlotDescription(Dtk_Template *tmpl, newtRefArg key);
    
    /** Create a newt frame containing a slot.
     */
    newtRef         getScriptSlotDescription(newtRefArg key);
    
    /** Create a newt frame containing a slot.
     */
    newtRef         getAttributesSlotDescription(newtRefArg key);
    
    /** Convert the template id into a proto magic pointer number.
     */
    int             findProto(const char *id);
    
    /** Return the help text for a slot in a template.
     */
    const char *getHelp(Dtk_Template *tmpl, const char *slot);
    
  protected:
    
    class CStringSort {
    public:
      bool operator()(const char*, const char*) const;
    };
    
    newtRef         platform_;
    
    Fl_Menu_Item    * templateChoiceMenu_;
    
    Fl_Menu_Item    * methodsChoiceMenu_;
    
    Fl_Menu_Item    * attributesChoiceMenu_;
    
    std::map<char*,Fl_Menu_Item*,CStringSort> specificMenuMap_;
    
    char * ptfFilename_;
    
    char * constFilename_;
  };


#endif

//
// End of "$Id$".
//
