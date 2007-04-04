//
// "$Id$"
//
// Dtk_Layout_Document header file for the Dyne Toolkit.
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

#ifndef DTK_LAYOUT_DOCUMENT_H
#define DTK_LAYOUT_DOCUMENT_H


#include <dtk/Dtk_Document.h>


/*---------------------------------------------------------------------------*/
/**
 * Base class for any kind of document that we can view, edit, or even compile.
 */
class Dtk_Layout_Document : public Dtk_Document
{
public:
					Dtk_Layout_Document();
	virtual			~Dtk_Layout_Document();

	virtual int		load();
	virtual void	edit();
	virtual int		save();
	virtual int		saveAs();
	virtual void	close();
	virtual int		getID() { return 0; }

private:
};


#endif

//
// End of "$Id$".
//
