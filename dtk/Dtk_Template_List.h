//
// "$Id$"
//
// Dtk_Template_List header file for the Dyne Toolkit.
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

#ifndef DTK_TEMPLATE_LIST_H
#define DTK_TEMPLATE_LIST_H


#include <vector>


class Dtk_Template;
class Dtk_Layout;


/** 
 * Keep a list of templates.
 */
class Dtk_Template_List
  {
  public:
    
    /** 
     * Initialize a template list
     */
    Dtk_Template_List(Dtk_Template *parent);
    
    /** 
     * Delete a template list and all its members.
     */
    ~Dtk_Template_List();
    
    
    /**
     * \name List Management
     */
    //@{
        
    /** 
     * Append an existing template to this list of templates.
     * 
     * This function will create a link to the given template and ask the template to link back here. 
     *
     * \todo It will also create a corresponding entry in the template browser if that exists.
     *
     * It will not open or show the template.
     *
     * \param[in] tmpl the templates that we want to append to the end of the list.
     */
    void append(Dtk_Template *tmpl);
    
    /** 
     * Remove a template from this list.
     *
     * This function removes all links from this list to the given template.
     * It also updates the browser if required.
     * It does not close, clear, or delete the document.
     *
     * \param   [in] tmpl address of template to be removed
     *
     * \todo FIXME: In the current version, this function *does* delete the template which causes a crash!
     */
    void remove(Dtk_Template *tmpl);
    
    /**
     * Remove all templates from the list.
     *
     * Go backwards through the list and tell all members to clear their 
     * dependents, then remove the member from this list, and finally delete 
     * the member.
     *
     * This function takes care of all UI links as well.
     *
     * \todo FIXME: Verify that this is really what this function does!
     */
    void clear();
    
    /** 
     * Return the number of templates in the list.
     */
    int size() { return tmplList_.size(); }
    
    /** 
     * Return the template pointer using an index.
     */
    Dtk_Template *at(int index) { return tmplList_.at(index); }
    
    //@}
    
    /** Return the the owner of this template list.
     */
    Dtk_Template *parent() { return parent_; }
    
  private:
    
    /// back reference to the template that keeps this list
    Dtk_Template    * parent_;
    
    /// stdlib list of documents
    std::vector<Dtk_Template*>  tmplList_;
    
  };


#endif

//
// End of "$Id$".
//
