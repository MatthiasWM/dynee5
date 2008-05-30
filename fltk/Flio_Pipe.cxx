//
// "$Id$"
//
// Flio_Pipe implementation for the FLIO extension to FLTK.
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


#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif


#include "Flio_Pipe.h"

#include <FL/Fl.H>
#include <FL/fl_draw.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>


Flio_Pipe::Flio_Pipe(int X, int Y, int W, int H, const char *L)
: Flio_Stream(X, Y, W, H, L),
  is_client_(0),
  send_fd_(-1),
  recv_fd_(-1)
{
}


Flio_Pipe::Flio_Pipe(Flio_Stream *super)
: Flio_Stream(super),
  is_client_(0),
  send_fd_(-1),
  recv_fd_(-1)
{
}


Flio_Pipe::~Flio_Pipe()
{
  close();
}

int Flio_Pipe::open(const char *port, int is_client)
{
  char c2s[128];
  char s2c[128];

  is_client_ = is_client;
  sprintf(c2s, "%s_c2s", port);
  sprintf(s2c, "%s_s2c", port);
 /* 
  mknod(FIFO_FILE, S_IFIFO|0666, 0); 
  // SIGPIPE sent, if sending, but no reader 
  // then open with O_NONBLOCK set.
  //   -- or --
  send_fd_ = mkfifo(is_client?c2s:s2c, 0002);
  recv_fd_ = mkfifo(is_client?s2c:c2s, 0004);
 */
  if (send_fd_==-1 || recv_fd_==-1) {
    close();
    return -1;
  }
  return 0;
}

int Flio_Pipe::write(const unsigned char *data, int n)
{
	return -1;
}

int Flio_Pipe::available() 
{
	return -1;
}

int Flio_Pipe::read(unsigned char *dest, int n)
{
	return -1;
}

void Flio_Pipe::close()
{
	/*
  if (send_fd_!=-1)
    ::close(send_fd_);
  if (recv_fd_!=-1)
    ::close(recv_fd_);
  */
}

int Flio_Pipe::is_open()
{
  return 0;
}

//
// End of "$Id$".
//
