//
// "$Id$"
//
// Dtk_Project header file for the Dyne Toolkit.
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

#ifndef DTK_PROJECT_H
#define DTK_PROJECT_H

extern "C" {
#include "NewtType.h"
}

class Dtk_Document_List;
class Dtk_Script_Writer;
class Fldtk_Main_Window;
class Fl_Tabs;

/*---------------------------------------------------------------------------*/
/**
 * Manage multiple documents that make up a project and generate and application.
 */
class Dtk_Project
{
public:
  
  /** 
   * Create an empty project.
   */
  Dtk_Project();
  
  /** 
   * Delete the project and all associated resources.
   */
  ~Dtk_Project();
  
  /**
   * Return true if the project was modified and needs to be saved.
   *
   * \todo implement this function
   */
  int isDirty();

  /**
   * Set all project settings to their default value.
   */
	void          setDefaults();
  
  /**
   * Load any kind of project file.
   */
	int           load();
  
  /**
   * Load a Mac style project file.
   */
	int           loadMac();
  
  /**
   * Load a Windows style project file.
   */
	int           loadWin();
  
  /**
   * Save a project in DyneTK format, which is derived from the MSWindows format.
   */
	int           save();

  /**
   * Create NewtonScript form this project.
   */
	int           write(Dtk_Script_Writer &sw);

  /**
   * Save all dirty parts of the project.
   */
  int           saveAll();

  /** 
   * Close the project and delete all dependent.
   *
   * \todo Implement me!
   */
	void          close();

  /**
   * Build a NewtonScript package from this project.
   */
	int           buildPackage();
  
  /**
   * Save the package that we just created.
   */
	int           savePackage();

  /**
   * Return a pointer to a buffer containing the filename and path of the package.
   */
	char          * getPackageName();
  
  /**
   * Change the filename of the project and all associated names.
   */
	void          setFilename(const char *filename);
  
  /**
   * Create a Newt Frame referencing a file relative to the path of the project.
   */
	newtRef       makeFileRef(const char *filename);

  /**
   * Return a pointer to a buffer containing project file path with a trailing slash.
   */
  char          * dos_pathname();
  char          * posix_pathname();

  /** 
   * Return the name of the project.
   */
  const char    * name() { return name_; }
  
  /**
   * Return a pointer to a buffer containing project file path and name.
   */
  const char    * filename() { return filename_; }
  
  /**
   * Return the list of documents in tis project.
   */
  Dtk_Document_List * documentList() { return documentList_; }

private:
  
  /**
   * Remember the current directory.
   *
   * Stack depth is one!
   */
	void          pushDir();
  
  /**
   * Restore the previous directory.
   */
	void          popDir();

  /// path and name of package
	char          * packagename_;
  
  /// just the name of the project without file extension
	char          * shortname_;
  
  /// path and name of the project
	char          * filename_;
  
  /// name of the project
	char          * name_;
  
  /// the directory which we were launched from
	char          * startdir_;
  
  /// stack for pushing and popping a path
	char          * dos_pathname_;
	char          * posix_pathname_;
  
  /// package as a Newton Database
	newtRef       package_;

  /// Keep a list of all documents in the project.
  Dtk_Document_List * documentList_;  
  
  /// we are responsible for the application window
  Fldtk_Main_Window * wMainWindow;
  
  /// the tab group that contains all project related browsers
  Fl_Tabs *wBrowserTabs;
};

    

#endif

//
// End of "$Id$".
//
