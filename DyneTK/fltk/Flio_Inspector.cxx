//
// "$Id$"
//
// Flio_Inspector implementation for the FLIO extension to FLTK.
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
// Please report all bugs and problems to "flio@matthiasm.com".
//


#include "Flio_Inspector.h"

#include "globals.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include <FL/Fl_Button.H>
#include <FL/Fl_Image.H>

extern Fl_Button *wInspectorConnect;
extern Fl_Image *toolbox_open_pixmap;
extern Fl_Image *toolbox_closed_pixmap;
extern Fl_Window *wConnect;


Flio_Inspector::Flio_Inspector(int X, int Y, int W, int H, const char *L)
: Flio_Mnp4_Serial(X, Y, W, H, L),
  pattern_(0L),
  nPattern_(0),
  NPattern_(0),
  found_(0L),
  received_(0L),
  error_(0L),
  buffer_(0L),
  nBuffer_(0),
  NBuffer_(0),
  nReceive_(0)
{
}


Flio_Inspector::~Flio_Inspector()
{
}


int Flio_Inspector::open(const char *port, int bps)
{
  int err = Flio_Mnp4_Serial::open(port, bps);
  waitForCommand();
  return err;
}

unsigned char term1[4] = {
  0x6e, 0x65, 0x77, 0x74, 
  // newt
};
unsigned char term2[4] = {
  0x6e, 0x74, 0x70, 0x20, 
  // ntp 
};
unsigned char term3[4] = {
  0x74, 0x65, 0x72, 0x6d, 
  // term
};
unsigned char term4[4] = {
  0x00, 0x00, 0x00, 0x00, 
  // ....
};

void Flio_Inspector::close()
{
  send_data_block(term1, sizeof(term1));
  send_data_block(term2, sizeof(term2));
  send_data_block(term3, sizeof(term3));
  send_data_block(term4, sizeof(term4));
  // FIXME: actually, we should wait for the request to terminate by the Newton
  // and reply accordingly
  //Flio_Mnp4_Serial::close();
}


int Flio_Inspector::on_connect()
{
	wInspectorConnect->image(toolbox_open_pixmap);
	UpdateMainMenu();
	if (wConnect)
		wConnect->hide();
	return 1;
}


int Flio_Inspector::on_disconnect()
{
	wInspectorConnect->image(toolbox_closed_pixmap);
	wInspectorConnect->redraw();
	UpdateMainMenu();
	return 1;
}


static unsigned char send_okln1[] = { 0x6E, 0x65, 0x77, 0x74, 0x6E, 0x74, 0x70, 0x20, };
static unsigned char send_okln2[] = { 0x6F, 0x6B, 0x6C, 0x6E, };
static unsigned char send_okln3[] = { 0x00, 0x01, 0x00, 0x00, 0x00, };


int Flio_Inspector::on_receive()
{
  int n = get_block_size();
  unsigned char *buf = get_block();
  if (buf[1]==4) {
    int offset = buf[0]+1;
    buf += offset;
    n -= offset;
    /*
    while (n>0) {
      if (!consume_block(buf, n)) {
        printf("**** WARNING: unidentified block\n");
        return 0;
      }
    }*/
    while (n) {
      if (nPattern_) {
        // we are scanning for a pattern
        if (nBuffer_<nPattern_) {
          buffer_[nBuffer_++] = buf[0];
          buf++; n--;
        } else {
          memmove(buffer_, buffer_+1, nPattern_-1);
          buffer_[nBuffer_-1] = buf[0];
          buf++; n--;
        }
        if (nBuffer_==nPattern_ && memcmp(buffer_, pattern_, nPattern_)==0)
          (this->*found_)();
        else if (nBuffer_==nPattern_)
          printf("Can't sync\n");
      } else {
        if (nBuffer_<nReceive_) {
          buffer_[nBuffer_++] = buf[0];
          buf++; n--;
        }
        if (nBuffer_==nReceive_)
          (this->*received_)();
      }
    }
    return 1;
  } else {
    printf("**** ERROR: unexpected block type %d\n", buf[1]);
    return 0;
  }
}

#include "fltk/Flmm_Console.h"
extern Flmm_Console *wConsole;


unsigned int Flio_Inspector::get_uint(const unsigned char *src)
{
  return (src[0]<<24)|(src[1]<<16)|(src[2]<<8)|(src[3]);
}


int Flio_Inspector::scan_for(unsigned char *pattern, int size, Call found, Call error, double timeout)
{
  if (NPattern_<size) {
    NPattern_ = size+32;
    pattern_ = (unsigned char*)realloc(pattern_, NPattern_);
  }
  if (NBuffer_<size) {
    NBuffer_ = size+256;
    buffer_ = (unsigned char*)realloc(buffer_, NBuffer_);
  }
  memcpy(pattern_, pattern, size);
  nPattern_ = size;
  nBuffer_ = 0;
  nReceive_ = 0;
  found_ = found;
  error_ = error;
  return 0;
}

int Flio_Inspector::wait_for(int n, Call received, Call error, double timeout)
{
  nBuffer_ = 0;
  return wait_for_more(n, received, error, timeout);
}

int Flio_Inspector::wait_for_more(int n, Call received, Call error, double timeout)
{
  int size = nBuffer_ + n;
  if (NBuffer_<size+2) { // always have space to add a trailing zero in UTF-16!
    NBuffer_ = size+256;
    buffer_ = (unsigned char*)realloc(buffer_, NBuffer_);
  }
  nPattern_ = 0;
  nReceive_ = nBuffer_ + n;
  received_ = received;
  error_ = error;
  if (n==0)
    (this->*received_)();
  return 0;
}


void Flio_Inspector::waitForCommand() {
  scan_for((unsigned char*)"newtntp ", 8, &Flio_Inspector::gotNewtNtk, &Flio_Inspector::gotErr);
}

void Flio_Inspector::gotErr() {
  printf("*** ERROR *** while waiting for data\n");
}

void Flio_Inspector::gotNewtNtk() {
  wait_for_more(4, &Flio_Inspector::gotNewtNtk4, &Flio_Inspector::gotErr);
}

void Flio_Inspector::gotNewtNtk4() {
  //printf("gotNewtNtk4 '%.4s'\n", buffer_+8);
  if (memcmp(buffer_+8, "cnnt", 4)==0)
    // cnnt: wait for a size and unknown block. Send connection ack
    wait_for_more(4, &Flio_Inspector::gotNewtNtkCnnt, &Flio_Inspector::gotErr);
  else if (memcmp(buffer_+8, "fobj", 4)==0)
    // fobj: size, NSOF
    wait_for_more(4, &Flio_Inspector::gotNewtNtkFobj, &Flio_Inspector::gotErr);
  else if (memcmp(buffer_+8, "rslt", 4)==0)
    // rslt: wait for size and a result code identifier
    wait_for_more(4, &Flio_Inspector::gotNewtNtkRslt, &Flio_Inspector::gotErr);
  else if (memcmp(buffer_+8, "text", 4)==0)
    // text: wait for size, then text block, then dump to console
    wait_for_more(4, &Flio_Inspector::gotNewtNtkText, &Flio_Inspector::gotErr);
  else if (memcmp(buffer_+8, "eerr", 4)==0)
    // eerr: exception, size, text
    wait_for_more(8, &Flio_Inspector::gotNewtNtkEerr, &Flio_Inspector::gotErr);
  else if (memcmp(buffer_+8, "eref", 4)==0)
    // eerr: size, text, text, errno, garble
    wait_for_more(4, &Flio_Inspector::gotNewtNtkEref, &Flio_Inspector::gotErr);
  else if (memcmp(buffer_+8, "fstk", 4)==0)
    // fstk: size, stack dump in NSOF format
    wait_for_more(4, &Flio_Inspector::gotNewtNtkFstk, &Flio_Inspector::gotErr);
  else if (memcmp(buffer_+8, "eext", 4)==0)
    // eext: ?? just dump
    wait_for_more(4, &Flio_Inspector::gotNewtNtkFstk, &Flio_Inspector::gotErr);
  else {
    printf("*** ERROR *** Inspector received unknown command %.4s\n", buffer_+8);
    waitForCommand();
  }
}

void Flio_Inspector::gotNewtNtkCnnt() {
  send_data_block(send_okln1, sizeof(send_okln1));
  send_data_block(send_okln2, sizeof(send_okln2));
  send_data_block(send_okln3, sizeof(send_okln3));
  unsigned int n = get_uint(buffer_+12);
  wait_for_more(n, &Flio_Inspector::waitForCommand, &Flio_Inspector::gotErr);
}

void Flio_Inspector::gotNewtNtkRslt() {
  unsigned int n = get_uint(buffer_+12);
  wait_for_more(n, &Flio_Inspector::waitForCommand, &Flio_Inspector::gotErr);
}

void Flio_Inspector::gotNewtNtkFobj() {
  unsigned int n = get_uint(buffer_+12);
  wait_for_more(n, &Flio_Inspector::gotNewtNtkFobjSize, &Flio_Inspector::gotErr);
}

void Flio_Inspector::gotNewtNtkFobjSize() {
  dumpBuffer();
  waitForCommand();
}

void Flio_Inspector::gotNewtNtkText() {
  unsigned int n = get_uint(buffer_+12);
  wait_for_more(n, &Flio_Inspector::gotNewtNtkTextSize, &Flio_Inspector::gotErr);
}

void Flio_Inspector::gotNewtNtkTextSize() {
  unsigned int i, n = get_uint(buffer_+12);
  unsigned char *s = buffer_+16;
  for (i=0; i<n; i++) {
    if (*s=='\r') *s = '\n';
    s++;
  }
  *s = 0;
  wConsole->insert((char*)buffer_+16);
  waitForCommand();
}

void Flio_Inspector::gotNewtNtkEerr() {
  unsigned int n = get_uint(buffer_+16);
  wait_for_more(n, &Flio_Inspector::gotNewtNtkEerrSize, &Flio_Inspector::gotErr);
}

void Flio_Inspector::gotNewtNtkEerrSize() {
  unsigned int i, n = get_uint(buffer_+16);
  unsigned int ex = get_uint(buffer_+12);
  unsigned char *s = buffer_+20;
  for (i=0; i<n; i++) {
    if (*s=='\r') *s = '\n';
    s++;
  }
  *s = 0;
  wConsole->insert((char*)buffer_+20);
  waitForCommand();
}

void Flio_Inspector::gotNewtNtkEref() {
  unsigned int n = get_uint(buffer_+12);
  wait_for_more(n+8, &Flio_Inspector::gotNewtNtkErefSize, &Flio_Inspector::gotErr);
}

void Flio_Inspector::gotNewtNtkErefSize() {
  unsigned int i;
  unsigned int n  = get_uint(buffer_+12);
  unsigned int n1 = get_uint(buffer_+16);
  unsigned int n2 = get_uint(buffer_+20+n1);
  unsigned char *s = buffer_+20;
  for (i=0; i<n1; i++) {
    if (*s=='\r') *s = '\n';
    s++;
  }
  *s = 0; // ouch!
  wConsole->insert("Exception: ");
  wConsole->insert((char*)buffer_+20);
  wConsole->insert("\n");
  // at n2, we have an NSOF containing errorCode and symbol
  waitForCommand();
}

void Flio_Inspector::gotNewtNtkFstk() {
  unsigned int n = get_uint(buffer_+12);
  wait_for_more(n, &Flio_Inspector::gotNewtNtkFstkSize, &Flio_Inspector::gotErr);
}

void Flio_Inspector::gotNewtNtkFstkSize() {
  wConsole->insert("Unhandled fstk chunk\n");
  dumpBuffer();
  waitForCommand();
}


void Flio_Inspector::dumpBuffer()
{
	int i, j, nLine = nBuffer_/16;
	for (i=0; i<nBuffer_; i+=16) {
		char buf[78];
		memset(buf, 32, 78); buf[76] = '\n'; buf[77] = 0;
		sprintf(buf, "%08x", i); buf[8] = ' ';
		for (j=i; (j<i+16) && (j<nBuffer_); j++) {
			unsigned char c = buffer_[j];
			sprintf(buf+10+3*(j-i), "%02x", c); buf[12+3*(j-i)] = ' ';
			if (isprint(c)) 
				buf[59+j-i] = c;
			else
				buf[59+j-i] = '.';
		}
		wConsole->insert(buf);
	}
}

// This happens when we press "EXIT": newtntp code
unsigned char dataX0219[150] = {
  0x02, 0x06, 0x06, 0x07, 0x05, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x07, 0x0c, 0x69, 0x6e, 0x73, 0x74, 
  0x72, 0x75, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x07, 0x08, 0x6c, 0x69, 0x74, 0x65, 0x72, 0x61, 
  0x6c, 0x73, 0x07, 0x08, 0x61, 0x72, 0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x6e, 0x75, 
  0x6d, 0x41, 0x72, 0x67, 0x73, 0x07, 0x0c, 0x44, 0x65, 0x62, 0x75, 0x67, 0x67, 0x65, 0x72, 0x49, 
  0x6e, 0x66, 0x6f, 0x07, 0x09, 0x43, 0x6f, 0x64, 0x65, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x03, 0x03, 
  0x09, 0x02, 0x18, 0x28, 0x02, 0x04, 0x01, 0x09, 0x03, 0x07, 0x0d, 0x45, 0x78, 0x69, 0x74, 0x42, 
  0x72, 0x65, 0x61, 0x6b, 0x4c, 0x6f, 0x6f, 0x70, 0x06, 0x03, 0x07, 0x0d, 0x5f, 0x6e, 0x65, 0x78, 
  0x74, 0x41, 0x72, 0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x5f, 0x70, 0x61, 0x72, 0x65, 
  0x6e, 0x74, 0x07, 0x0c, 0x5f, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x6f, 0x72, 
  0x0a, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 
  // .....class..instructions..literals..argFrame..numArgs..DebuggerI
  // nfo..CodeBlock.....(.......ExitBreakLoop...._nextArgFrame.._pare
  // nt.._implementor......
};

// changing the stack trace: newtntp code
unsigned char dataX0179[147] = {
  0x02, 0x06, 0x06, 0x07, 0x05, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x07, 0x0c, 0x69, 0x6e, 0x73, 0x74, 
  0x72, 0x75, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x07, 0x08, 0x6c, 0x69, 0x74, 0x65, 0x72, 0x61, 
  0x6c, 0x73, 0x07, 0x08, 0x61, 0x72, 0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x6e, 0x75, 
  0x6d, 0x41, 0x72, 0x67, 0x73, 0x07, 0x0c, 0x44, 0x65, 0x62, 0x75, 0x67, 0x67, 0x65, 0x72, 0x49, 
  0x6e, 0x66, 0x6f, 0x07, 0x09, 0x43, 0x6f, 0x64, 0x65, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x03, 0x03, 
  0x09, 0x02, 0x18, 0x28, 0x02, 0x04, 0x01, 0x09, 0x03, 0x07, 0x0a, 0x53, 0x74, 0x61, 0x63, 0x6b, 
  0x54, 0x72, 0x61, 0x63, 0x65, 0x06, 0x03, 0x07, 0x0d, 0x5f, 0x6e, 0x65, 0x78, 0x74, 0x41, 0x72, 
  0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x5f, 0x70, 0x61, 0x72, 0x65, 0x6e, 0x74, 0x07, 
  0x0c, 0x5f, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x6f, 0x72, 0x0a, 0x0a, 0x0a, 
  0x00, 0x00, 0x0a, 
  // .....class..instructions..literals..argFrame..numArgs..DebuggerI
  // nfo..CodeBlock.....(.......StackTrace...._nextArgFrame.._parent.
  // ._implementor......
};

// changing the print depth: newtntp code
unsigned char dataX0141[148] = {
  0x02, 0x06, 0x06, 0x07, 0x05, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x07, 0x0c, 0x69, 0x6e, 0x73, 0x74, 
  0x72, 0x75, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x07, 0x08, 0x6c, 0x69, 0x74, 0x65, 0x72, 0x61, 
  0x6c, 0x73, 0x07, 0x08, 0x61, 0x72, 0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x6e, 0x75, 
  0x6d, 0x41, 0x72, 0x67, 0x73, 0x07, 0x0c, 0x44, 0x65, 0x62, 0x75, 0x67, 0x67, 0x65, 0x72, 0x49, 
  0x6e, 0x66, 0x6f, 0x07, 0x09, 0x43, 0x6f, 0x64, 0x65, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x03, 0x04, 
  0x09, 0x02, 0x20, 0xa8, 0x70, 0x02, 0x04, 0x01, 0x09, 0x03, 0x07, 0x0a, 0x70, 0x72, 0x69, 0x6e, 
  0x74, 0x44, 0x65, 0x70, 0x74, 0x68, 0x06, 0x03, 0x07, 0x0d, 0x5f, 0x6e, 0x65, 0x78, 0x74, 0x41, 
  0x72, 0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x5f, 0x70, 0x61, 0x72, 0x65, 0x6e, 0x74, 
  0x07, 0x0c, 0x5f, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x6f, 0x72, 0x0a, 0x0a, 
  0x0a, 0x00, 0x00, 0x0a, 
  // .....class..instructions..literals..argFrame..numArgs..DebuggerI
  // nfo..CodeBlock.... ®p.......printDepth...._nextArgFrame.._parent
  // .._implementor......
};

// pressing the "trace" button: newtntp code
unsigned char dataX0068[143] = {
  0x02, 0x06, 0x06, 0x07, 0x05, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x07, 0x0c, 0x69, 0x6e, 0x73, 0x74, 
  0x72, 0x75, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x07, 0x08, 0x6c, 0x69, 0x74, 0x65, 0x72, 0x61, 
  0x6c, 0x73, 0x07, 0x08, 0x61, 0x72, 0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x6e, 0x75, 
  0x6d, 0x41, 0x72, 0x67, 0x73, 0x07, 0x0c, 0x44, 0x65, 0x62, 0x75, 0x67, 0x67, 0x65, 0x72, 0x49, 
  0x6e, 0x66, 0x6f, 0x07, 0x09, 0x43, 0x6f, 0x64, 0x65, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x03, 0x04, 
  0x09, 0x02, 0x22, 0xa8, 0x70, 0x02, 0x04, 0x01, 0x09, 0x03, 0x07, 0x05, 0x74, 0x72, 0x61, 0x63, 
  0x65, 0x06, 0x03, 0x07, 0x0d, 0x5f, 0x6e, 0x65, 0x78, 0x74, 0x41, 0x72, 0x67, 0x46, 0x72, 0x61, 
  0x6d, 0x65, 0x07, 0x07, 0x5f, 0x70, 0x61, 0x72, 0x65, 0x6e, 0x74, 0x07, 0x0c, 0x5f, 0x69, 0x6d, 
  0x70, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 0x6f, 0x72, 0x0a, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 
  // .....class..instructions..literals..argFrame..numArgs..DebuggerI
  // nfo..CodeBlock...."®p.......trace...._nextArgFrame.._parent.._im
  // plementor......
};

// pressing "stop":
unsigned char dataX0018[153] = {
  0x02, 0x06, 0x06, 0x07, 0x05, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x07, 0x0c, 0x69, 0x6e, 0x73, 0x74, 
  0x72, 0x75, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x07, 0x08, 0x6c, 0x69, 0x74, 0x65, 0x72, 0x61, 
  0x6c, 0x73, 0x07, 0x08, 0x61, 0x72, 0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x6e, 0x75, 
  0x6d, 0x41, 0x72, 0x67, 0x73, 0x07, 0x0c, 0x44, 0x65, 0x62, 0x75, 0x67, 0x67, 0x65, 0x72, 0x49, 
  0x6e, 0x66, 0x6f, 0x07, 0x09, 0x43, 0x6f, 0x64, 0x65, 0x42, 0x6c, 0x6f, 0x63, 0x6b, 0x03, 0x06, 
  0x09, 0x02, 0x27, 0x00, 0x1a, 0xa8, 0x70, 0x02, 0x04, 0x01, 0x09, 0x03, 0x07, 0x0d, 0x62, 0x72, 
  0x65, 0x61, 0x6b, 0x4f, 0x6e, 0x54, 0x68, 0x72, 0x6f, 0x77, 0x73, 0x06, 0x03, 0x07, 0x0d, 0x5f, 
  0x6e, 0x65, 0x78, 0x74, 0x41, 0x72, 0x67, 0x46, 0x72, 0x61, 0x6d, 0x65, 0x07, 0x07, 0x5f, 0x70, 
  0x61, 0x72, 0x65, 0x6e, 0x74, 0x07, 0x0c, 0x5f, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 
  0x74, 0x6f, 0x72, 0x0a, 0x0a, 0x0a, 0x00, 0x00, 0x0a, 
  // .....class..instructions..literals..argFrame..numArgs..DebuggerI
  // nfo..CodeBlock....'..®p.......breakOnThrows...._nextArgFrame.._p
  // arent.._implementor......
};


/*
  "pkgX"  DeletePackage
  "code"  HandleCodeBlock which takes an NSOF function frame and
interpretes it.
  "rslt"  Returns some result back to NTK
  "text"  Sends some text header to NTK
  "eext"  Enter Breakloop
  "bext"  Exit Breakloop
  "eref"  Sends Exception Data (cahr *msg, RefArg data)
  "estr"  Sends Exception Data (char *msg, char *data)
  "eerr"  Sends Exception Data (char *msg, long ErrNo)
  "fstk"  Sends a stackframe to NTK

  "okln"  Sent by NTK to ACK connection, if not Newton respods with
Error -28016
  "cnnt"  Requests an NTK connection from the desktop
  "term"  Requests disconnect (?) (Not quite sure about yet)
  "pkg "  LoadPackage
  "stou"  Set timeout
  "dpkg"  ?? Probably request for Package Download
  "fobj"  Sends any arbitrary Newton Object as NSOF to NTK
  */

//
// End of "$Id$".
//
