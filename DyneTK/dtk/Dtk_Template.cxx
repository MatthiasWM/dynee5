//
// "$Id$"
//
// Dtk_Template implementation for the Dyne Toolkit.
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



#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "Dtk_Template_List.h"
#include "Dtk_Template.h"


/*---------------------------------------------------------------------------*/
Dtk_Template::Dtk_Template(Dtk_Layout_Document *layout, Dtk_Template_List *list)
:   layout_(layout),
    list_(list),
    tmplList_(0L)
{
}


/*---------------------------------------------------------------------------*/
Dtk_Template::~Dtk_Template()
{
    delete tmplList_;
}



//
// End of "$Id$".
//
