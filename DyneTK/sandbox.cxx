//
// "$Id$"
//
// DyneTK sandbox implementation file
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

#include <FL/x.H>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Tooltip.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_XPM_Image.H>
#include <FL/Fl_Preferences.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_File_Chooser.H>

#include "fluid/Fldtk_Prefs.h"
#include "fluid/Fldtk_Proj_Settings.h"

#include "dtk/Dtk_Document_List.h"
#include "dtk/Dtk_Document.h"
#include "dtk/Dtk_Project.h"

#include "fluid/main_ui.h"
#include "fltk/Flio_Serial_Port.h"

//#include "images/toolbox_open.xpm"
//#include "images/toolbox_closed.xpm"

#include <assert.h>

extern "C" {
#include "NewtCore.h"
#include "NewtBC.h"
#include "NewtNSOF.h"
#include "NewtPkg.h"
#include "NewtPrint.h"
#include "NewtEnv.h"
}

//
// this is our little sandbox. Nothing here can be take seriously. 
// But your didn'y anyways, right?
//
unsigned char data5413[147] = {
  0x02, 0x06, 0x06, 0x07, 0x05, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x07, 0x0c, 0x69, 0x6e, 0x73, 0x74, 
  0x72, 0x75, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x07, 0x08, 0x6c, 0x69, 0x74, 0x65, 0x72, 0x61, 
  0x6c, 0x73, 0x07, 0x08, 0x61, 0x72, 0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x6e, 0x75, 
  0x6d, 0x41, 0x72, 0x67, 0x73, 0x07, 0x0c, 0x44, 0x65, 0x62, 0x75, 0x67, 0x67, 0x65, 0x72, 0x49, 
  0x6e, 0x66, 0x6f, 0x07, 0x09, 0x43, 0x6f, 0x64, 0x65, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x03, 0x04, 
  0x09, 0x02, 0x70, 0x19, 0x38, 0x02, 0x04, 0x02, 0x09, 0x03, 0x07, 0x03, 0x64, 0x74, 0x6b, 0x07, 
  0x04, 0x4f, 0x70, 0x65, 0x6e, 0x06, 0x03, 0x07, 0x0d, 0x5f, 0x6e, 0x65, 0x78, 0x74, 0x41, 0x72, 
  0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x5f, 0x70, 0x61, 0x72, 0x65, 0x6e, 0x74, 0x07, 
  0x0c, 0x5f, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x6f, 0x72, 0x0a, 0x0a, 0x0a, 
  0x00, 0x00, 0x0a, 
  // .....class..instructions..literals..argFrame..numArgs..DebuggerI
  // nfo..CodeBlock....p.8.......dtk..Open...._nextArgFrame.._parent.
  // ._implementor......
};

extern "C" int getNSO(const char *command, unsigned char **data, int *size);

unsigned char *nso;
int nNso;


/**
 * Convert the given command into a binary Newton Streaming Object,
 * ready to be sent to the Newton.
 */
int getNSO(const char *command, unsigned char **data, int *size)
{
  NEWT_DUMPBC = 1;
  newtRefVar obj = NBCCompileStr((char*)command, true);
  NewtPrintObject(stdout, obj);
  newtRefVar nsof = NsMakeNSOF(0, obj, NewtMakeInt30(2));  
  NewtPrintObject(stdout, nsof);

  if (NewtRefIsBinary(nsof)) {
    *size = NewtBinaryLength(nsof);
    *data = (unsigned char*)malloc(*size);
    memcpy(*data, NewtRefToBinary(nsof), *size);

    unsigned char *s = *data;
    int i, n = *size;
    for (i=0; i<n; i++) {
      printf("%02x ", *s);
      s++;
    }
    s = *data;
    n = *size;
    for (i=0; i<n; i++) {
      printf("%c", *s>31 ? *s : '.');
      s++;
    }

  } else {
    printf("*** ERROR *** creating NSOF\n");
  }

  return 0;
}

/*
static unsigned char recv_cnnt[] = {
  0x02, 0x04, 0x01, 0x6e, 0x65, 0x77, 0x74, 0x6e, 0x74, 0x70, 0x20, 0x63, 0x6e, 
  0x6e, 0x74, 0x00, 0x01, 0x00, 0x00, 0x00
};
*/

unsigned char data0013a[] = { 2,4,0, 0x6e, 0x65, 0x77, 0x74, };
unsigned char data0014a[] = { 2,4,0, 0x6e, 0x74, 0x70, 0x20, };
unsigned char data0015a[] = { 2,4,0, 0x6c, 0x73, 0x63, 0x62, };
unsigned char data0016a[] = { 2,4,0, 0x00, 0x00, 0x00, 0xc2, };
unsigned char data0017a[] = {
  2,4,0, 
  0x02, 0x06, 0x06, 0x07, 0x05, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x07, 0x0c, 0x69, 0x6e, 0x73, 0x74, 
  0x72, 0x75, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x07, 0x08, 0x6c, 0x69, 0x74, 0x65, 0x72, 0x61, 
  0x6c, 0x73, 0x07, 0x08, 0x61, 0x72, 0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x6e, 0x75, 
  0x6d, 0x41, 0x72, 0x67, 0x73, 0x07, 0x0c, 0x44, 0x65, 0x62, 0x75, 0x67, 0x67, 0x65, 0x72, 0x49, 
  0x6e, 0x66, 0x6f, 0x07, 0x09, 0x43, 0x6f, 0x64, 0x65, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x03, 0x05, 
  0x09, 0x02, 0x18, 0x19, 0x1a, 0x2a, 0x02, 0x04, 0x03, 0x09, 0x03, 0x08, 0x0e, 0x00, 0x4e, 0x00, 
  0x74, 0x00, 0x6b, 0x00, 0x32, 0x00, 0x41, 0x00, 0x73, 0x00, 0x00, 0x08, 0x1c, 0x00, 0x48, 0x00, 
  0x65, 0x00, 0x6c, 0x00, 0x6c, 0x00, 0x6f, 0x00, 0x20, 0x00, 0x4e, 0x00, 0x65, 0x00, 0x77, 0x00, 
  0x74, 0x00, 0x6f, 0x00, 0x6e, 0x00, 0x2e, 0x00, 0x00, 0x07, 0x09, 0x41, 0x6c, 0x61, 0x72, 0x6d, 
  0x55, 0x73, 0x65, 0x72, 0x06, 0x03, 0x07, 0x0d, 0x5f, 0x6e, 0x65, 0x78, 0x74, 0x41, 0x72, 0x67, 
  0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x5f, 0x70, 0x61, 0x72, 0x65, 0x6e, 0x74, 0x07, 0x0c, 
  0x5f, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x6f, 0x72, 0x0a, 0x0a, 0x0a, 0x00, 
  0x00, 0x0a, 
};

unsigned char data0006a[] = {
  2,4,0, 
  0x6e, 0x65, 0x77, 0x74, 
  0x6e, 0x74, 0x70, 0x20, 
  0x63, 0x6f, 0x64, 0x65, 
  0xff, 0xff, 0xff, 0xff, 
  0x00, 0x00, 0x00, 0x94, 
  0x02, 0x06, 0x06, 0x07, 0x05, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x07, 0x0c, 0x69, 0x6e, 0x73, 0x74, 
  0x72, 0x75, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x07, 0x08, 0x6c, 0x69, 0x74, 0x65, 0x72, 0x61, 
  0x6c, 0x73, 0x07, 0x08, 0x61, 0x72, 0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x6e, 0x75, 
  0x6d, 0x41, 0x72, 0x67, 0x73, 0x07, 0x0c, 0x44, 0x65, 0x62, 0x75, 0x67, 0x67, 0x65, 0x72, 0x49, 
  0x6e, 0x66, 0x6f, 0x07, 0x09, 0x43, 0x6f, 0x64, 0x65, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x03, 0x04, 
  0x09, 0x02, 0x24, 0xa8, 0x70, 0x02, 0x04, 0x01, 0x09, 0x03, 0x07, 0x0a, 0x70, 0x72, 0x69, 0x6e, 
  0x74, 0x44, 0x65, 0x70, 0x74, 0x68, 0x06, 0x03, 0x07, 0x0d, 0x5f, 0x6e, 0x65, 0x78, 0x74, 0x41, 
  0x72, 0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x5f, 0x70, 0x61, 0x72, 0x65, 0x6e, 0x74, 
  0x07, 0x0c, 0x5f, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x6f, 0x72, 0x0a, 0x0a, 
  0x0a, 0x00, 0x00, 0x0a, 
};


extern unsigned char *nso;
extern int nNso;
extern "C" int getNSO(const char *command, unsigned char **data, int *size);

void send_test(int ix) {
  if (ix==1) {
    wInspectorSerial->send_block(data0013a, sizeof(data0013a));
    wInspectorSerial->send_block(data0014a, sizeof(data0014a));
    wInspectorSerial->send_block(data0015a, sizeof(data0015a));
    wInspectorSerial->send_block(data0016a, sizeof(data0016a));
    wInspectorSerial->send_block(data0017a, sizeof(data0017a));
  } else if (ix==2) {
	InspectorSendScript("Print(\"So long and thanks for all the fish!\")");
	/*
    int err = getNSO("Print(\"So long and thanks for all the fish!\")", &nso, &nNso);
    if (nNso==0) return;
    wInspectorSerial->send_block(data0013a, sizeof(data0013a));
    wInspectorSerial->send_block(data0014a, sizeof(data0014a));
    wInspectorSerial->send_block(data0015a, sizeof(data0015a));
    unsigned char b1[7] = { nNso>>24, nNso>>16, nNso>>8, nNso };
    wInspectorSerial->send_data_block(b1, 4);
    wInspectorSerial->send_data_block(nso, nNso);
	*/
  } else if (ix==3) {
    getNSO("Print(3*0.5)", &nso, &nNso);
    if (nNso==0) return;
    wInspectorSerial->send_block(data0013a, sizeof(data0013a));
    wInspectorSerial->send_block(data0014a, sizeof(data0014a));
    wInspectorSerial->send_block(data0015a, sizeof(data0015a));
    unsigned char b1[7] = { nNso>>24, nNso>>16, nNso>>8, nNso };
    wInspectorSerial->send_data_block(b1, 4);
    wInspectorSerial->send_data_block(nso, nNso);
  } else if (ix==4) {
    if (nNso==0) return;
    wInspectorSerial->send_block(data0013a, sizeof(data0013a));
    wInspectorSerial->send_block(data0014a, sizeof(data0014a));
    wInspectorSerial->send_block(data0015a, sizeof(data0015a));
    unsigned char b1[7] = { nNso>>24, nNso>>16, nNso>>8, nNso };
    wInspectorSerial->send_data_block(b1, 4);
    wInspectorSerial->send_data_block(nso, nNso);
  }
}

void testNSOFReader(const char *filename) 
{
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
		NcSetGlobalVar(NSSYM(printDepth), NSINT(12));
		NEWT_DUMPBC = 0;
		NEWT_INDENT = -2;

		FILE *g = fopen("dump_nsof.txt", "wb");
		newtRef pkg = NewtReadNSOF(buffer, n);
		NewtPrintObject(g, pkg);
		fclose(g);

	}
}

void testPkgReader(const char *filename) 
{

	if (!filename)
		filename = fl_file_chooser("Load Package", "*.pkg", 0L);
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
		//const char *argv[] = { "DyneTK" };
		//NewtInit(1, argv, 0);

		NcSetGlobalVar(NSSYM(printLength), NSINT(9999));
		NcSetGlobalVar(NSSYM(printDepth), NSINT(20));
		NEWT_DUMPBC = 2;
    NEWT_INDENT = -2;

		newtRef pkg = NewtReadPkg(buffer, n);
		printf("Begin Package:\n");
		NewtPrintObject(stdout, pkg);
		printf("End Package\n");

    char fn[2048];
    strcpy(fn, filename);
    fl_filename_setext(fn, 2048, ".txt");
    f = fopen(fn, "wb");
		NewtPrintObject(f, pkg);
    fclose(f);

/*
{	parts: [
		icon: {
		    bits: <Binary, class "bits", length 124>, 
		    mask: <Binary, class "mask", length 124>, 
		    bounds: {left: 0, top: 0, bottom: 27, right: 24}		
*/
		/* dump the bitmap and mask
		{
			int32_t ix = NewtFindSlotIndex(pkg, NSSYM(parts));
			newtRef parts = NewtGetFrameSlot(pkg, ix);
			newtRef part = NewtGetArraySlot(parts, 0);
			ix = NewtFindSlotIndex(part, NSSYM(data));
			newtRef data = NewtGetFrameSlot(part, ix);
			ix = NewtFindSlotIndex(data, NSSYM(icon));
			newtRef icon = NewtGetFrameSlot(data, ix);

			ix = NewtFindSlotIndex(icon, NSSYM(mask));
			newtRef bits = NewtGetFrameSlot(icon, ix);
			uint32_t i, len = NewtBinaryLength(bits);
			uint8_t *src = NewtRefToBinary(bits);
			printf("uint8_t bits[] = {\n");
			for (i=0; i<len; i++) {
				printf("0x%02x, ", src[i]);
			}
			printf("\n};\n\n");

			newtRef mask = NewtGetBinarySlot(icon, NewtFindSlotIndex(icon, NSSYM(mask)));
			len = NewtBinaryLength(mask);
			src = NewtRefToBinary(mask);
			printf("uint8_t mask[] = {\n");
			for (i=0; i<len; i++) {
				printf("0x%02x, ", src[i]);
			}
			printf("\n};\n\n");
		}
		// */

		//project->package_ = pkg;
		/*
		{
			printf("Writing Package\n");
			newtRef pkg_bin = NewtWritePkg(pkg);

			uint32_t size = NewtBinaryLength(pkg_bin);
			uint8_t *data = NewtRefToBinary(pkg_bin);
			FILE *f = fopen("d:/home/matt/dev/Newton/ntk2/helloWRT.pkg", "wb");
			fwrite(data, 1, size, f);
			fclose(f);
		}

		printf("Done\n");
		*/
		//NewtCleanup();
	}
}


//
// End of "$Id$".
//
