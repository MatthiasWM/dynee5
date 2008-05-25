//
// "$Id$"
//
// Dtk_Script_Document header file for the Dyne Toolkit.
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

#ifndef DTK_SCRIPT_DOCUMENT_H
#define DTK_SCRIPT_DOCUMENT_H


#include <dtk/Dtk_Document.h>


class Fldtk_Script_Editor;


/*---------------------------------------------------------------------------*/
/**
 * This class manages documents containing scripts.
 */
class Dtk_Script_Document : public Dtk_Document
{
public:
    /** Create a document container for Newt Scripts.
     *
     * \param list this is the list that we will be hooked into
     */
					Dtk_Script_Document(Dtk_Document_List *list);
    
    /** Remove all links and resources and delete this document.
     */
	virtual			~Dtk_Script_Document();

    /** Load this document from disk.
     *
     * Load the document file using the file and path referenced
     * via getFilename().
     *
     * \retval 0 if successful
     * \retval negative if an error occured
     */
	virtual int		load();

    /** Open the appropritate editor for this document.
     *
     * If this document is editable, theis call will create and
     * open the editor window. Most editor window are linked into the 
     * decument tabs group. If the editor is already open, it
     * will still be raised and made the urrent one.
     */
	virtual int     edit();

    /** Save the document using the given filename.
     *
     * Save the doc without popping up a file chooser. If the 
     * AskForFilename is set, this function will call saveAs()
     * instead.
     *
     * \retval 0 if successful
     * \retval negative if an error occured
     */
	virtual int		save();

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
    virtual int		saveAs();

	virtual void	close();

    /** Write this document type as a Newt Script.
     *
     * The Script Document is copied to the destination without 
     * changes. 
     *
     * \retval 0 if successful
     * \retval negative if an error occured
     */     
	virtual int		write(Dtk_Script_Writer &sw);

	virtual int		getID() { return 5; }

	virtual	newtRef	compile();

private:
    Fldtk_Script_Editor *editor_;
};


#endif

//
// End of "$Id$".
//
