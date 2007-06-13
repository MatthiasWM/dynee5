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

    /** Initialize a document list
     * 
     * Documents lists ca be use in projects or stand-alone for 
     * globally available documents.
     */
                    Dtk_Document_List(Dtk_Project *proj=0L);

    /** Remove a document list and all referenced documents.
     */
                    ~Dtk_Document_List();

    /** Add any kind of existing document.
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

    /** Remove a document from the list.
     *
     * This function does not close or delete the document.
     * This also updates the browser if required.
     *
     * \param   [in] document address of doc to be removed
     *
     * \retval  0 if successful
     * \retval  -1 if the document was not in this list
     */
    int             remove(Dtk_Document *document);

    /** Find a file based on a filename and path.
     *
     * If the returned value is not the same as the calling value, the buffer 
     * must be freed by the caller.
     *
     * \todo We need this to avoid duplicate files, so please implement this.
     */
	char		    * findFile(const char *filename);

    /** Create a new document that will contain a script.
     *
     * \param   [in] filename a path and filename for the new document
     *
     * \retval  address of the new document, or NULL if the operation failed
     */
	Dtk_Document	* newScript(const char *filename);

    /** Create a new document that will contain a layout.
     *
     * \param   [in] filename a path and filename for the new document
     *
     * \retval  address of the new document, or NULL if the operation failed
     */
	Dtk_Document	* newLayout(const char *filename);

    /** Get a document by index.
     * 
     * \param   [in] i index number of document
     *
     * \retval  address of document, or NULL
     */
	Dtk_Document	* getDocument(int i);

    /** Create a Newt Array that conatins a list of all project documents.
     *
     * \retval  Newt reference to list of project items
     */
	newtRef			getProjectItemsRef();

    /** Return the address of the project that manages us.
     *
     * \retval  address of project or NULL
     */
    Dtk_Project     * project() { return project_; }

    /** Add the document to the internal list and to the browser.
     * 
     * \param doc the document that we want to add to the list
     */
    void            append(Dtk_Document *doc);

    /** A document tells us that its name changed.
     *
     * \param document the document that changed its name.
     */
    void            filenameChanged(Dtk_Document *document);

private:

    /// back reference to the project that keeps this list
    Dtk_Project     * project_;

    /// stdlib list of documents
	std::vector<Dtk_Document*>	docList_;

    /// we are in charge of keeping this browser updated and reacting to its messages
    Fldtk_Document_Browser      * browser_;

    /// this is an FLTK callback that is triggered when the browser is clicked
    static void browser_cb(Fldtk_Document_Browser *w, Dtk_Document_List *d);
};


#endif

//
// End of "$Id$".
//
