//
// "$Id$"
//
// Dtk_Project_UI implementation for the Dyne Toolkit.
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

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

/*-- class header --*/
#include "ui/Dtk_Project_UI.h"

/*-- reference to project --*/
#include "dtk/Dtk_Project.h"

/*-- reference to UI --*/
#include "fluid/Fldtk_Proj_Settings.h"
#include "fluid/main_ui.h"

/*-- other headers --*/


/*----------------------------------------------------------------------------*/
Dtk_Project_UI::Dtk_Project_UI(Dtk_Project *p)
: project(p),
  window(dtkMain),
  browserTabs(dtkBrowserTabs)
{
  project->setGuiManager(this);
  projectCreated();
}


/*----------------------------------------------------------------------------*/
void Dtk_Project_UI::projectCreated()
{
  projectRenamed();
  browserTabs->activate();
}


/*----------------------------------------------------------------------------*/
void Dtk_Project_UI::projectRemoved()
{
  window->label("DyneTK");
  browserTabs->deactivate();
}


/*----------------------------------------------------------------------------*/
void Dtk_Project_UI::projectRenamed()
{
  const char *name = project->name();
  if (name) {
    char buffer[2048];
    sprintf(buffer, "DyneTK: %s", project->name());
    window->copy_label(buffer);
  } else {
    window->label("DyneTK: <unnamed project>");
  }
}

//
// End of "$Id$".
//
