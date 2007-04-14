//
// "$Id$"
//
// DyneMP implementation file
//
// Copyright 2002-2007 by Matthias Melcher.
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
// Please report all bugs and problems to "dyne@matthiasm.com".
//

#include "main.h"

#include "fluid/dmp_main.h"

#include <FL/Fl_Tooltip.H>

extern "C" {
#include "NewtCore.h"
#include "NewtBC.h"
#include "NewtNSOF.h"
#include "NewtPkg.h"
#include "NewtPrint.h"
#include "NewtEnv.h"
}

#include <stdio.h>


Dmp_Main *dmpMain;


extern "C" void yyerror(char * s)
{
	printf("%s\n", s);
}


/*---------------------------------------------------------------------------*/
/**
 * Main entry function.
 * 
 * \param	[in]	number of arguments when launched
 * \param	[inout]	launch arguments
 * \retval	0
 */
int main(int argc, char **argv) {
  
	// initialize our GUI library
	Fl::lock();
	Fl::scheme("GTK+");
	Fl_Tooltip::size(11);

	// initialize the local interpreter and compiler
	NewtInit(argc, (const char**)argv, 0);
	// set up all the Newt stuff!
	NcSetGlobalVar(NSSYM(printLength), NSINT(20));
	NcSetGlobalVar(NSSYM(printDepth), NSINT(5));
	// load packages from a directory?
	// if we have the optional pipe argument, open an inspector connection to DTK

	// launch the application
	dmpMain = new Dmp_Main(340, 530, "DyneMP v0.0.1");
	dmpMain->show(argc, argv);
	Fl::run();

	// clean the virtual machine
	NewtCleanup();

	return 0;
}


//
// End of "$Id$".
//
