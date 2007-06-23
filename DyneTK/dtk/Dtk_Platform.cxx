//
// "$Id$"
//
// Dtk_Platform implementation for the Dyne Toolkit.
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


#include "Dtk_Platform.h"

#include "allNewt.h"

#include <FL/fl_ask.h>

#include <stdio.h>
#include <sys/stat.h>


/*---------------------------------------------------------------------------*/
Dtk_Platform::Dtk_Platform(const char *filename)
:   platform_(kNewtRefUnbind),
    templateChoiceMenu_(0L)
{
    load(filename);
}


/*---------------------------------------------------------------------------*/
Dtk_Platform::~Dtk_Platform()
{
}


/*---------------------------------------------------------------------------*/
int Dtk_Platform::load(const char *filename)
{
    struct stat st;
    if (stat(filename, &st)) {
        fl_alert(
            "Platform file \"%s\" not found.\n\n"
            "This file is required to edit DyneTK projects. It must be placed in the DyneTK start directory.\n"
            "\"%s\" is part of the Mac NTK and should be available on Unna:\n\n"
            "http://www.unna.org/view.php?/apple/development/NTK/platformfiles/Newton2.1_Platform_File.sea.hqx",
            filename, filename);
        return -1;
    }
    FILE *f = fopen(filename, "rb");
	uint8_t *pf = (uint8_t*)malloc(st.st_size);
    fread(pf, 1, st.st_size, f);
    fclose(f);

    platform_ = NewtReadNSOF(pf, st.st_size);

FILE *ff = fopen("platform1.txt", "wb");
NewtPrintObject(ff, platform_);
fclose(ff);

newtRef ta = NewtGetArraySlot(platform_, NewtFindSlotIndex(platform_, NSSYM(TemplateArray)));
if (ta==kNewtRefUnbind)
    return 0L;

ff = fopen("platform.txt", "wb");
NewtPrintObject(ff, ta);
fclose(ff);

    free(pf);

    if (platform_==kNewtRefUnbind) {
        fl_alert("Error reading platform file.");
        return -1;
    } else {
        NcSetGlobalVar(NSSYM(DyneTK_Platform), platform_);
        return 0;
    }
}


/*---------------------------------------------------------------------------*/
Fl_Menu_Item *Dtk_Platform::templateChoiceMenu()
{
    if (templateChoiceMenu_)
        return templateChoiceMenu_;
    if (platform_==kNewtRefUnbind)
        return 0L;

    newtRef ta = NewtGetArraySlot(platform_, NewtFindSlotIndex(platform_, NSSYM(TemplateArray)));
    if (ta==kNewtRefUnbind)
        return 0L;

//FILE *ff = fopen("platform.txt", "wb");
//NewtPrintObject(ff, ta);
//fclose(ff);

    int i, n = NewtArrayLength(ta);
    for (i=0; i<n; i++) {
        newtRef sym = NewtSlotsGetSlot(ta, i);
        NewtPrintObject(stdout, sym);
        if (NewtRefIsSymbol(sym)) {
            printf("SYM: %s\n", NewtSymbolGetName(sym));
        } else {
            printf("++\n");
        }
    }

    return 0L;
}


/*
void testNSOFReader(const char *filename) 
{
    struct stat st;
    stat("testing_script_writer.txt", &st);
    FILE *f = fopen("testing_script_writer.txt", "rb");
	char *script = (char*)malloc(st.st_size);
    fread(script, 1, st.st_size, f);
    fclose(f);

    
    
    if (!filename)
		filename = fl_file_chooser("Load NSOF File", "*", 0L);
	if (!filename) return;

	uint8_t *buffer;
	FILE *f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	int nn = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = (uint8_t*)malloc(nn);
	int n = fread(buffer, 1, nn, f);
	fclose(f);
	if (n) {
		NcSetGlobalVar(NSSYM(printLength), NSINT(9999));
		NcSetGlobalVar(NSSYM(printDepth), NSINT(3));
		NEWT_DUMPBC = 0;
		NEWT_INDENT = -2;

		FILE *f = fopen("dump_nsof.txt", "wb");
		newtRef pkg = NewtReadNSOF(buffer, n);
		NewtPrintObject(f, pkg);
		fclose(f);

	}
}
*/

//
// End of "$Id$".
//
