//
// "$Id$"
//
// Flio_Inspector header file for the FLIO extension to FLTK.
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

#ifndef FLIO_INSPECTOR
#define FLIO_INSPECTOR

#include "Flio_MNP4_Serial.h"


/**
 * Handle the Inspector protocol.
 *
 * This is a rough but working implementation of the most
 * common parts of the protocol between the NTK Inspector and
 * the Newton Toolbox.
 *
 * \todo According to the docs, *all* communication starts with "newtntd xxxxyyyy"
 *     where xxx is the command fourcc and yyyy is the length of the command
 *     <b>which must be padded to multiples of four</b>!
 * \todo This class has far too much knowledge about the rest of the 
 *     application. We must come up with a better interface!
 * \todo This class is still quite messy and should be separated
 *     into a Flio_Serializer and the Flio_Inspector. 
 * \todo It should not be derived from Flio_MNP4_Serial, but 
 * instead link to any block-oriented device.
 * \todo 'fobj' - streamed object following
 * \todo 'eerr', execption error (a=??, b=size of text, 
 * \todo 'eref', a, b, text, c, fobj - execption error (a=??, b=size of text, 
 *        text=name of exception, c=size of fobj, fobj=streamed
 *        object containing error code and symbol information
 * \todo 'code', 0xffffffff, len - ??
 */
class Flio_Inspector : public Flio_Mnp4_Serial
{
  typedef void (Flio_Inspector::*Call)();
public:

  /**
   * Create a standard widget.
   */
  Flio_Inspector(int X, int Y, int W, int H, const char *L=0L);

  /**
   * Close connection and return allocated resources.
   */
  ~Flio_Inspector();

  /**
   * Open a connection.
   *
   * All connections are opened with 8N1 for simplicity.
   *
   * \param[in] OS-specific port name, for example "\\.\COM1", or "/dev/ttyS0"
   * \param[in] transfer rate in bits per second (38400bps)
   * \return 0 for success, -1 if failed
   */
  int open(const char *port, int bps);

  /**
   * Close the serial connection.
   */
  void close();

protected:

  /**
   * Wait for the "newtntp " signature that precedes every block.
   */
  void waitForCommand();
  void gotErr();
  void gotNewtNtk();

  /**
   * Dispatch the block accoring to the four character code received.
   */
  void gotNewtNtk4();

  /**
   * 'cnnt' connection request is answered with 'okln'.
   */
  void gotNewtNtkCnnt();

  /**
   * 'rslt' result code received.
   */
  void gotNewtNtkRslt();

  /**
   * 'fobj' we received a NSOF object.
   */
  void gotNewtNtkFobj();
  void gotNewtNtkText();

  /**
   * 'text' ASCII text that goes right away to the terminal.
   */
  void gotNewtNtkTextSize();
  void gotNewtNtkEerr();

  /**
   * 'eerr' not entirely implemented yet.
   */
  void gotNewtNtkEerrSize();
  void gotNewtNtkEref();

  /**
   * 'eref' exception, not entirely implemented yet.
   */
  void gotNewtNtkErefSize();

  int scan_for(unsigned char *pattern, int size, Call found, Call error, double timeout=0.0);
  int wait_for(int size, Call received, Call error, double timeout=0.0);
  int wait_for_more(int size, Call received, Call error, double timeout=0.0);

  virtual int on_connect();
  virtual int on_disconnect();
  virtual int on_receive();

private:
  int consume_block(unsigned char *&buf, int &n);
  unsigned int get_uint(const unsigned char *src);

  unsigned char *pattern_;
  int nPattern_;
  int NPattern_;
  Call found_;
  Call received_;
  Call error_;
  unsigned char *buffer_;
  int nBuffer_;
  int NBuffer_;
  int nReceive_;
};

#endif

//
// End of "$Id$".
//
