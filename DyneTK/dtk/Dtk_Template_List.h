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
class Dtk_Layout_Document;


/** Keep a list of templates.
 */
class Dtk_Template_List
{
public:

    /** Initialize a template list
     */
                    Dtk_Template_List(Dtk_Template *parent);

    /** Remove a template list and all its members.
     */
                    ~Dtk_Template_List();

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
