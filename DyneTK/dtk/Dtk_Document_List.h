//
// "$Id$"
//
// Dtk_Document_List header file for the Dyne Toolkit.
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

#ifndef DTK_DOCUMENT_LIST_H
#define DTK_DOCUMENT_LIST_H


#include <vector>

extern "C" {
#include "NewtType.h"
}

class Dtk_Project;
class Dtk_Document;

class Fldtk_Document_Browser;


/** Keep a list of documents.
 *
 * If the "project" member is set, these documents are associated with a 
 * project. Otherwise, the documents in the list are global and not used 
 * by any project.
 *
 * The Dtk_Document_List is the one and only class responsibel for the
 * document browser that is assigned to it.
 */
class Dtk_Document_List
  {
  public:
    
    /**
     * /defgroup constructor Constructor and destructor
     */
    //@{
    
    /** 
     * Initialize a document list
     * 
     * Documents lists ca be use in projects or stand-alone for 
     * globally available documents.
     */
    Dtk_Document_List(Dtk_Project *proj=0L);
    
    /** 
     * Remove a document list and all referenced documents.
     */
    ~Dtk_Document_List();
    
    //@}
    
    
    /**
     * /defgroup listmanagement Basic List Management
     */
    //@{
    
    /** 
     * Append the document to the internal list and to the browser.
     * 
     * This function will create a link to the given document and ask the
     * document to link back here. It will also create a corresponding entry
     * in the document browser if that exists.
     *
     * It will not open the document or creata a vie in the tab group.
     *
     * \param doc the document that we want to append to the end of the list.
     */
    void            append(Dtk_Document *doc);

    /** 
     * Remove a document from the list.
     *
     * This function removes all links from this list to the given document.
     * It does not close, clear, or delete the document.
     * This also updates the browser if required.
     *
     * \param   [in] document address of doc to be removed
     *
     * \retval  0 if successful
     * \retval  -1 if the document was not in this list
     */
    int             remove(Dtk_Document *document);
    
    /**
     * Delete all documents in this list.
     *
     * Go backwards through the list and tell all members to clear their 
     * dependents, the remove the member from this list, and finally delete 
     * the member.
     *
     * This function takes care of all UI links as well.
     */
    void clear();
    
    /** 
     * Return the number of documents in this list.
     */
    int             size() { return docList_.size(); }
    
    /**
     * Return a pointer to the document at index i.
     */
    Dtk_Document    * at(int i);
    
    //@}
    
    
    /** 
     * Add any kind of existing document.
     * 
     * We use the filename and the first bytes of the file to determine
     * the document typ, then create a new document container, load the
     * document from disk, and update the GUI for editing.
     *
     * \param   [in] filename of document that we will add
     *
     * \retval  0 if document was added successfully
     * \retval  negative, if any part of the operation failed
     *
     * \todo The file type check is currently only minimal.
     */
    Dtk_Document	* add(const char *filename);
        
    /** 
     * Find a file based on a filename and path.
     *
     * If the returned value is not the same as the calling value, the buffer 
     * must be freed by the caller.
     *
     * \todo We need this to avoid duplicate files, so please implement this.
     */
    char		    * findFile(const char *filename);
    
    /**
     * Create a new document that will contain a script.
     *
     * \param   [in] filename a path and filename for the new document
     *
     * \retval  address of the new document, or NULL if the operation failed
     */
    Dtk_Document	* newScript(const char *filename);
    
    /**
     * Create a new document that will contain a layout.
     *
     * \param   [in] filename a path and filename for the new document
     *
     * \retval  address of the new document, or NULL if the operation failed
     */
    Dtk_Document	* newLayout(const char *filename);
    
    /** 
     * Create a Newt Array that conatins a list of all project documents.
     *
     * \retval  Newt reference to list of project items
     */
    newtRef			getProjectItemsRef();
    
    /**
     * Return the address of the project that manages us.
     *
     * \retval  address of project or NULL
     */
    Dtk_Project     * project() { return project_; }
    
    /** 
     * A document tells us that its name changed.
     *
     * \param document the document that changed its name.
     */
    void            documentNameChanged(Dtk_Document *document);
    
    /** 
     * Return the main document.
     */
    Dtk_Document    * getMainDocument() { return main_; }
    
    /** 
     * Change the main document.
     */
    void            setMainDocument(Dtk_Document *doc);
    
    
  private:
    
    /// back reference to the project that keeps this list
    Dtk_Project     * project_;
    
    /// stdlib list of documents
    std::vector<Dtk_Document*>	docList_;
    
    /// every project has a main document which describes the top level layout
    Dtk_Document * main_;
    
    /// we are solely responsible for the document browser
    Fldtk_Document_Browser *wDocumentBrowser_;
    
    /// this is an FLTK callback that is triggered when the browser is clicked
    static void wDocumentBrowser_cb(Fldtk_Document_Browser *w, Dtk_Document_List *d);
  };


#endif

//
// End of "$Id$".
//
