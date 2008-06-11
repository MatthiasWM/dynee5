//
// "$Id$"
//
// Dtk_Template header file for the Dyne Toolkit.
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

#ifndef DTK_TEMPLATE_H
#define DTK_TEMPLATE_H


class Dtk_Layout;
class Dtk_Template_List;

extern "C" {
#include "NewtType.h"
}


class Fl_Hold_Browser;
class Fl_Group;
class Dtk_Slot_List;
class Flnt_Widget;
class Dtk_Slot;
class Dtk_Rect_Slot;
class Dtk_Value_Slot;
class Dtk_Proto_Slot;
class Dtk_Script_Writer;

/** Manage a template object in a layout.
 *
 * Templates are arranged as a tree inside a layout.
 * They contain a list of slots which make up the 
 * attributes and signal handling of a template.
 */
class Dtk_Template
{
public:
  
  /** Initialize a template.
   * 
   * \param layout back reference to our parent layout
   * \param list bvack referenc to the list that manages us; if this is 
   *        NULL, we are the root of the layout
   * \param proto a C-String naming the prototype for this template
   */
  Dtk_Template(
               Dtk_Layout *layout, 
               Dtk_Template_List *list=0L,
               char *proto=0L);
  
  /** Remove a template and all its children.
   */
  virtual         ~Dtk_Template();
  
  /**
   * Remove all child layouts and all slots.
   */
  void clear();
  
  /** Load a template tree starting at the given newtRef
   */
  int             load(newtRef node);
  
  /** Create a frame that can be saved to a layout file.
   */
  newtRefVar      save();
  
  /** Write this template as a Newt Script.
   *
   * \param sw reference to the script writer
   *
   * \retval 0 if successful
   * \retval negative if an error occured
   */     
  virtual int		write(Dtk_Script_Writer &sw);
  
  /** Recursively run through all templates in the tree.
   *
   * Upate all information needed to keep the browser information current.
   */
  void            updateBrowserLink(Fl_Hold_Browser *browser, int &indent, int &index, bool add=false);
  
  /** Return the name of the template as it will appear in the browser
   *
   * The text that is used in the browser is prepended with spaces to create 
   * a tree-style list of templates. The actual text is a composition
   * of the name, id, and prototype of the template.
   *
   * \return creates and returns the name of the template as it appear in the browser
   */
  const char      * browserName();
  
  /** Return the name of this template.
   *
   * This is a pointer into an internal buffer which may change or move at any time.
   *
   * \return pointer to the template name
   */
  const char      * getName() const { return ntName_; }
  
  /** Set a new name for this template.
   *
   * Setting the name of the Template may trigger a redraw of the 
   * template browser and the layout view.
   *
   * \param name this string will be copied into the Template class
   */
  void            setName(const char *name);
  
  /** Add a new template as a step child to this template.
   *
   * \param x, y, w, h position and size of the new template
   * \param proto prototype for the new template, or NULL to add the user-selected prototype
   * \return the newly created template or NULL
   */
  Dtk_Template    * addTemplate(int x, int y, int w, int h, char *proto=0L);
  
  /** Add a previously create slot to this template.
   */
  void            addSlot(Dtk_Slot *);
  
  /** Add a named slot using a newt script description.
   */
  Dtk_Slot        * addSlot(newtRef key, newtRef slot);
  
  /** 
   * Remove a slot from this template.
   *
   * Close any open slot editor, then remove the slot from the 
   * template and from the slot browser.
   *
   * This function does not delete the slot.
   *
   * \param[in] slot the slot that we want removed
   *
   * \todo This function should return error codes for no existing slots and for
   *       slots that must not be removed.
   */
  void removeSlot(Dtk_Slot *slot);
  
  /** 
   * Delete all slots in this template.
   */
  void deleteAllSlots();
  
  /** 
   * Delete all template children from this template.
   */
  void deleteAllChildren();
  
  /** Return the associated Layout
   */
  Dtk_Layout * layout() { return layout_; }
  
  /** This template was selected by the user for editing.
   *
   * We need to update the slot browser and the slot editor.
   */
  void edit();
  
  /** Return the coordinates of the template in the layout.
   */
  void getSize(int &t, int &l, int &b, int &r);
  
  /** Return the template alignment.
   */
  unsigned int justify();
  
  /** This template was selected in the view
   */
  void selectedInView();
  
  /** Return 1 if this template is selected
   */
  char isSelected();
  
  /** Return the parent template of this template.
   */
  Dtk_Template *parent();
  
  /** Returns the name of the template during script building.
   */
  char *scriptName() { return scriptName_; }
  
  /** Return the slot list.
   *
   * If there is no slot list, this call creates one.
   */
  Dtk_Slot_List       * slotList();
  
  /** Return the ID, the class name of this template.
   */
  char                * id() { return ntId_; }
  
  /** Set a new ID.
   */ 
  void                id(const char *id);
  
  /** Find a slot by key.
   */
  Dtk_Slot            * findSlot(const char *key);
  
  Dtk_Rect_Slot       * viewBounds() { return viewBounds_; }
  Flnt_Widget         * widget() { return widget_; }
  
  /** Create a new widget in the template view.
   */
  Flnt_Widget         * newWidget();
  
  Dtk_Template_List     * list() { return list_; }
  
  void setList(Dtk_Template_List *list) { list_ = list; }
  void setLayout(Dtk_Layout *layout) { layout_ = layout; }
  
private:
  
  /** This slot is called if the widget is dragged or resized.
   */
  void                widgetBoundsChangedSignal();
  
  /** This slot is called if the viewBounds rectangle is modified.
   */
  void                viewBoundsChangedSignal();
  
  /** This slot is called if the viewJustify is modified by the user.
   */
  void                viewJustifyChangedSignal();
  
  /// we must be part of a single layout
  Dtk_Layout * layout_;
  
  /// every template except the root is a member of exactly one list
  Dtk_Template_List   * list_;
  
  /// a template can contain a list of templates to form a tree.
  Dtk_Template_List   * tmplList_;
  
  /// a template can have any number of slots
  Dtk_Slot_List       * slotList_;
  
  /// index in the browser widget
  int                 index_;
  
  /// depth within the tree
  int                 indent_;
  
  /// the browser that lists this template
  Fl_Hold_Browser     * browser_;
  
  /// name of the template as it appears in the browser view
  char                * browserName_;
  
  /// dtk name of template
  char                * ntName_;
  
  /// dtk id of template (compareable to C++ "class")
  char                * ntId_;
  
  /// name of template during script generation
  char                * scriptName_;
  
  /// if this is true, the scriptName_ was generated by the compiler
  bool                autoScriptName_;
  
  /// an FLTK derived widget graphically representing the Newt UI element
  Flnt_Widget         * widget_;
  
  /// slot containing the coordintes of the template widget
  Dtk_Rect_Slot       * viewBounds_;
  
  /// slot containing the widget justify value
  Dtk_Value_Slot      * viewJustify_;
  
  /// return this if no viewJustify Slot is connected
  unsigned int        defaultJustify_;
  
  /// slot containing the proto magic pointer 
  Dtk_Proto_Slot      * proto_;
};


#endif

//
// End of "$Id$".
//
