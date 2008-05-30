/*
*	File:		�PROJECTNAME�.h
*	
*	Version:	1.0
* 
*	Created:	�DATE�
*	
*	Copyright:  Copyright � �YEAR� �ORGANIZATIONNAME�, All Rights Reserved
* 
*	Disclaimer:	IMPORTANT:  This fltk application template  is supplied to you by F. Costantini on an "AS IS" basis.
*				USE IT AT YOUR OWN RISK. YOU CAN USE, MODIFY AND DISTRIBUTE THIS TEMPLATE 
*               provided that you keep the Copyright info in the distribution.
*
*/
/*=============================================================================
	�PROJECTNAME�.h
	
=============================================================================*/
#include <iostream>

#include <FL/Fl.H>

// �PROJECTNAME� Main User Interface include
#include "MainUI.h"

// �PROJECTNAME� main entry point
int main (int argc, char * const argv[]) {
   UserInterface ui;
    
	ui.get_window()->show();
	
	Fl::run();
	
    return 0;
}
