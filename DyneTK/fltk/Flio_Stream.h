//
// "$Id$"
//
// Flio_Stream header file for the FLIO extension to FLTK.
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

#ifndef FLIO_STREAM
#define FLIO_STREAM


#include <FL/x.H>
#include <FL/Fl_Box.H>


/**
 * A streaming data communication widget for FLTK1.
 */
class Flio_Stream : public Fl_Box
{
public:

  /**
   * Standard widget interface constructor.
   */
  Flio_Stream(int X, int Y, int W, int H, const char *L=0L);

  /**
   * Constructor for superwidgets.
   */
  Flio_Stream(Flio_Stream *super);

  /**
   * The destructor closes any open connections.
   */
  virtual ~Flio_Stream();

  /**
   * Open a connection.
   *
   * \param[in] OS-specific port name, for example "\\.\COM1", or "/dev/ttyS0"
   * \param[in] transfer rate in bits per second (38400bps)
   * \return 0 for success, -1 if failed
   */
  virtual int open(const char *port, int bps);

  /**
   * Write a block of binary data.
   *
   * This function returns immediatly.
   *
   * \param data[in] address of data block to send
   * \param n[in] size of data block
   * \return -1 if failed
   */
  virtual int write(const unsigned char *data, int n);

  /**
   * Return the number of bytes available.
   *
   * \return number of bytes available in the buffer
   */
  virtual int available();

  /**
   * Read bytes from the buffer.
   * 
   * This function reads at most n bytes from its internal
   * buffer and makes room for new bytes to receive.
   *
   * \param dest[in] address of memory destination
   * \param n[in] number of bytes to read at max
   * \return number of bytes actually read
   */
  virtual int read(unsigned char *dest, int n);

  /**
   * Close the connection.
   */
  virtual void close();

  /**
   * Check if the connection is open.
   *
   * \return 0 if the connection is closed
   */
  virtual int is_open();

  /**
   * This will be called whenever data arrives.
   *
   * Alternative interface to callbacks. Available bytes
   * are returned by available() and can then be read using
   * read().
   *
   * \return return 1, if callback should not be called anymore
   */
  virtual int on_read();

	virtual void draw() { Fl_Box::draw(); }

protected:

	Flio_Stream *super_;

};


#endif

//
// End of "$Id$".
//
