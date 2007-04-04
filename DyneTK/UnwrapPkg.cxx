//
// "$Id$"
//
// Code to unwrap a package file and put a visual representation into a GUI.
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
// Please report all bugs and problems to "flmm@matthiasm.com".
//


#include <FL/Fl.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Scroll.H>
#include "Flmm_Flex_Pack.H"
#include "Flmm_Flex_Group.H"
#include "Flmm_Flex_Box.H"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "Packages/PackageParts.h"
#include "Toolbox/CrossPlatform.h"


PackageDirectory *dir;


static void addInfoRefText(const char *label, InfoRef &value, uchar *data=0L) {
  char buf[512];
  sprintf(buf, "%s: offset=%d length=%d\n    --> ", label, value.offset, value.length);
  if (data) {
    int i, n = value.length; if (n>80) n=80;
    char *d = buf + strlen(buf);
    for (i=0; i<n; i++) {
      uchar c = data[value.offset+i];
      if (c>=32 && c<127) *d++ = c; else *d++ = '.';
    }
    new Flmm_Flex_Box(0, 0, 300, 28, strdup(buf));
  } else {
    new Flmm_Flex_Box(0, 0, 300, 14, strdup(buf));
  }
}


static void addInfoRef(const char *label, InfoRef &value, uchar *data=0L) {
  char buf[128];
  sprintf(buf, "%s: offset=%d length=%d", label, value.offset, value.length);
  new Flmm_Flex_Box(0, 0, 300, 14, strdup(buf));
  if (data) {
    int i;
    for (i=0; i<value.length; i++) {
      printf("%02x ", data[value.offset+i]);
    }
    printf("\n");
    for (i=0; i<value.length; i++) {
      uchar c = data[value.offset+i];
      if (c>=32 && c<127) putchar(c); else putchar('.');
    }
    printf("\n");
  }
}


static void addULongHex(const char *label, ULong value) {
  char buf[128];
  sprintf(buf, "%s: 0x%08lx", label, value);
  new Flmm_Flex_Box(0, 0, 300, 14, strdup(buf));
}


static void addULongDec(const char *label, ULong value) {
  char buf[128];
  sprintf(buf, "%s: %ld", label, value);
  new Flmm_Flex_Box(0, 0, 300, 14, strdup(buf));
}


static void addULongStr(const char *label, ULong value) {
  char buf[128];
  value = htonl(value);
  sprintf(buf, "%s: '%.4s'", label, (char*)(&value));
  new Flmm_Flex_Box(0, 0, 300, 14, strdup(buf));
}


void addPartEntry(PartEntry *p, uchar *varData) {
  fixEndianness(*p);
  addULongDec("offset", p->offset);
  addULongDec("size", p->size);
  addULongStr("type", p->type);
  addULongHex("flags", p->flags);
  addInfoRefText("info", p->info, varData);

  /*
ULong		offset;				//	offset to part, longword aligned
ULong		size;					//	size of part
ULong		size2;				//	= size
ULong		type;					//	'form', 'book', 'auto' etc.
ULong		reserved1;
ULong		flags;				//	defined below
InfoRef	info;					//	data passed to part when activated
InfoRef	reserved2;
   */
}


void unwrap_pkg(const char *filename) 
{
  char buf[128];
  
  FILE *f = fopen(filename, "rb");
  if (!f) {
    fl_message("Can't open package\n\"%s\"", filename);
    return;
  }
  struct stat pstat;
  stat(filename, &pstat);
  SizeT n = pstat.st_size;
  if (n<=0 || n>4*1024*1024) {
    fl_message("Unsupported file size: %ld bytes", n);
    return;
  }
  uchar *p = (uchar*)malloc(n), *head = p;
  fread(p, n, 1, f);
  fclose(f);
  
  if (strncmp((char*)p, "package", 7)!=0) {
    fl_message("Unsupported file format\n\"%s\"", filename);
    return;
  }
  
  Fl_Window *win = new Fl_Window(300, 500, filename);
  Fl_Scroll *scr = new Fl_Scroll(5, 5, win->w()-10, win->h()-10);
  scr->box(FL_DOWN_BOX);
  Flmm_Flex_Group *pkg = new Flmm_Flex_Group(7, 7, 300, 30);
  scr->resizable(pkg);
  win->resizable(scr);
  
  dir = (PackageDirectory*)head; 
  head += 52;
  fixEndianness(*dir);
  uchar *varData = head + /* relocation table + */ dir->numParts*sizeof(PartEntry);
  
  // -- add the directory member
  Flmm_Flex_Group *wDir = new Flmm_Flex_Group(0, 0, 300, 14, "Directory");
  sprintf(buf, "signature: %.8s", dir->signature);
  new Flmm_Flex_Box(0, 0, 300, 14, strdup(buf));
  addULongHex("id", dir->id);
  addULongHex("flags", dir->flags);
  // FIXME: we could list the symbolic names of the flags here...
  addULongHex("version", dir->version);
  addInfoRefText("copyright", dir->copyright, varData);
  addInfoRefText("name", dir->name, varData);
  addULongDec("size", dir->size);
  //Date		creationDate;
  //ULong		modifyDate;
  addULongDec("directorySize", dir->directorySize);
  addULongDec("numParts", dir->numParts);
  //PartEntry parts[];
  //char		variableLengthData[];
  wDir->end();

  // FIXME: there may be a relocation table here!
	if ((dir->signature[7] == '1') && (dir->flags & kRelocationFlag)) {
    new Flmm_Flex_Box(0, 0, 300, 14, "ERROR: unsupported relocation header: all following offsets are wrong!");
	}
  
  // -- add all parts
  for (unsigned pn=0; pn<dir->numParts; pn++) {
    PartEntry *pe = (PartEntry*)head;
    sprintf(buf, "Part %d", pn);
    Flmm_Flex_Group *block = new Flmm_Flex_Group(0, 0, 300, 14, strdup(buf));
    addPartEntry(pe, varData);
    head += sizeof(PartEntry);
    int n = pe->size;
    uchar *b = head + pe->offset;
    while (n>0) {
      ObjHeader *obj = (ObjHeader*)b;
      fixEndiannessSmart(*obj);
      Flmm_Flex_Group *wObj = new Flmm_Flex_Group(0, 0, 300, 14, "Object");
      addULongDec("size", obj->size);
      wObj->end();
      int nn = (obj->size+3)&~7;
      n -= nn; b += nn;
      n = 0;
    }
    block->end();
  }

  
  
  win->end();
  win->show();
}


//
// End of "$Id:$".
//
