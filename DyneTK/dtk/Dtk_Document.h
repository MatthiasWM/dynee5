//
// "$Id$"
//
// Dtk_Document header file for the Dyne Toolkit.
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

#ifndef DTK_DOCUMENT_H
#define DTK_DOCUMENT_H


#include "allNewt.h"


class Dtk_Document_List;
class Dtk_Document_UI;
class Dtk_Project;
class Dtk_Script_Writer;


/**
 * Base class for any kind of document that we can view, edit, or even compile.
 *
 * The DyneTK browser differs from NTK and shows a lot less information. We will
 * provide a dialog on right-click that will show additional data, including:
 *  * sequence number
 *  * name and path information
 *  * file type details
 *  * size
 *  * modification data
 *  * main layout flag
 *
 */
class Dtk_Document
{
public:
    /** Create a new document.
     * 
     * Do not call this constructor, but one of the derived classes.
     * 
     * \param list every document is member of exactly one list
     */
					Dtk_Document(Dtk_Document_List *list);

    /** Unlink and destroy this document.
     * 
     * This function returns all resources and then removes the class 
     * from the GUI and from the list.
     */
	virtual			~Dtk_Document();

    /** Load this document from disk.
     *
     * Load the document file using the file and path referenced
     * via getFilename().
     *
     * \retval 0 if successful
     * \retval negative if an error occured
     */
	virtual int		load() { return -1; }

    /** Save the document using the given filename.
     *
     * Save the doc without popping up a file chooser. If the 
     * AskForFilename is set, this function will call saveAs()
     * instead.
     *
     * \retval 0 if successful
     * \retval negative if an error occured
     */
	virtual int		save() { return -1; }

    /** Save the document under a new filename.
     *
     * Calling this function will pop up a file chooser set to the 
     * current filename. The user can then leave things alone, or 
     * change the filename, or cancel the operation.
     *
     * \retval 0 if successful
     * \retval negative if an error occured
     * \retval -2 if the user canceled
     */
	virtual int		saveAs() { return -1; }

    /** Write this document type as a Newt Script.
     *
     * \retval 0 if successful
     * \retval negative if an error occured
     */     
	virtual int		write(Dtk_Script_Writer &sw) { return -1; }

    /** Write a script line that will make the main form public.
     *
     * \retval 0 if successful
     * \retval negative if an error occured
     */     
	virtual int		writeTheForm(Dtk_Script_Writer &sw) { return -1; }

    /** Return 1 if the document should be saved.
     *
     * \retval 0 if the document is save
     * \retval 1 if the document was changed and should be saved to disk
     */
    int             isDirty() { return 0; }

    /** Open the appropritate editor for this document.
     *
     * If this document is editable, theis call will create and
     * open the editor window. Most editor window are linked into the 
     * decument tabs group. If the editor is already open, it
     * will still be raised and made the urrent one.
     */
    virtual int     edit() { return -1; }

    /** Close the editor window.
     *
     * This function closes any open editor window related to this doc 
     * and then unlinks GUI ties. The document will remain in memory
     * if it is part of a project.
     * 
     * To remove a document from a project, just call the documents
     * destructor.
     */
   	virtual void	close() { }

    /** Set a new filename for this document.
     *
     * This function will also create a name and a short name mebmber
     * for our convinience.
     *
     * \param filename name and path of file that holds the document data
     *
     * \todo Tell the list that my filename changed.
     */
	virtual void	setFilename(const char *filename);

    /** Special care for automatically generated filenames.
     *
     * This flag should be set if a filename was created without user 
     * interaction. It will ensure that saving document will pop up a 
     * file chooser for the user to correct the filename.
     *
     * \param v defaults to true, but can be ste to false to clear this flag
     */
	void			setAskForFilename(bool v=true);

    /** Return a pointer to the name part of the filename.
     *
     * \retval pointer to the name with original file extension.
     */
	const char		* name();

    /** Return a pointer to the name as it appears in the browser.
     *
     * \retval pointer to the name with original file extension and a 'main' indicator.
     */
    const char		* browserName() { return browserName_; }

    /** Update the browser name and tell the list about it.
     */
    void            updateBrowserName(bool tellTheList=true);

    /** Return the project that holds this document.
     *
     * \retval pointer to the project, or NULL if not part of a project
     */
    Dtk_Project     * project();

    /** Update the list back pointer.
     *
     * After attaching the document to a project, we must set 
     * the list backpointer correctly.
     */
    void            setList(Dtk_Document_List *list);

    /** Return true, if this document is a layout or derived from a layout.
     */
    virtual bool isLayout() { return false; }

    /// replace me
    virtual int		getID() { return -1; }

    /// replace me
	virtual	newtRef	compile() { return kNewtRefNIL; }

    /// replace me
	newtRef			getProjectItemRef();

    /** Make myself the main document in the project.
     */
    void            setMain();

  /**
   * Create the UI manager and have it create the UI.
   */
  void          createUI();

protected:

    /// this is the name of the file without path or extension
	char			* shortname_;

    /// this is the complete filename including the full path
	char			* filename_;

    /// this is the name of the file without path, but with extension
	char			* name_;

    /// the name of the file plus a tag indicating the main file
	char			* browserName_;

    /// if this flag is set, the "save" function will behave like "save as"
	bool			askForFilename_;

    /// we must always be a member of exactly one list
    Dtk_Document_List   * list_;
  
  /// link to our usr interface manager
  Dtk_Document_UI *ui;
};


#endif

//
// End of "$Id$".
//
