//
// "$Id$"
//
// Dtk_Project_UI header file for the Dyne Toolkit.
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

#ifndef DTK_PROJECT_UI_H
#define DTK_PROJECT_UI_H

class Dtk_Project;

class Fl_Window;
class Fl_Tabs;


/*---------------------------------------------------------------------------*/
/** Manage the connection betwen FLTK and Dtk_Project.
 */
class Dtk_Project_UI
{
public:
              Dtk_Project_UI(Dtk_Project*);
              ~Dtk_Project_UI();
  void        projectCreated();
  void        projectRemoved();
  void        projectRenamed();
  
private:
  Dtk_Project * project;
  Fl_Window   * window;
  Fl_Tabs     * browserTabs;
};
    

#endif

//
// End of "$Id$".
//
