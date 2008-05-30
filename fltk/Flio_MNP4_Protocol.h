//
// "$Id$"
//
// Flio_Mnp4_Protocol header file for the FLIO extension to FLTK.
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

#ifndef FLIO_MNP4_PROTOCOL
#define FLIO_MNP4_PROTOCOL


#include "Flio_Stream.h"


/**
 * Implementation of MNP error correction for serial lines.
 *
 * This class implements MNP error correction for serial line
 * communications, effectively converting a steream oriented 
 * unreliable device into a block oriented error free transmission.
 *
 * This widget does not implement the full MNP standard, but only
 * those parts needed for connecting, disconnecting, data transmission
 * and error correction. Special messages are not handled and may 
 * crash your code.
 *
 * This widget only implements the receiving side of a connection.
 *
 * \todo Implement the actual error correction (right now we only detect errors)
 */
class Flio_Mnp4_Protocol : public Flio_Stream
{
public:

  /** 
   * Standard widget constructor.
   */
  Flio_Mnp4_Protocol(int X, int Y, int W, int H, const char *L=0L);

  /**
   * Close device and return all buffers.
   */
  virtual ~Flio_Mnp4_Protocol();

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

  /**
   * Send a block of binary data.
   *
   * You are responsible for the correct block identifiers, 
   * serial number, content, and doubleing of escape bytes. 
   * The implementation will take care of the header, footer,
   * and checksum.
   *
   * This function returns immediatly.
   *
   * \param data[in] address of data block to send
   * \param n[in] size of data block
   */
  void send_block(unsigned char *data, int size);

  /**
   * Send a block of binary data.
   *
   * No knowledge of MNP is required. This function takes care 
   * of everything and wraps your precious data with everything
   * required.
   *
   * This function returns immediatly.
   *
   * \param data[in] address of data block to send
   * \param n[in] size of data block
   */
  void send_data_block(unsigned char *data, int size);

  /**
   * Get the size of the currently available data block.
   *
   * \return number of bytes in current block
   */
  int get_block_size();

  /**
   * Return a pointer to teh current block data.
   *
   * \return pointer to memory
   */
  unsigned char *get_block();

  /**
   * Return the data block to the system.
   *
   * Data blocks must be returned quickly to avoid buffer
   * overruns. It is often useful to copy the data into 
   * another place.
   */
  void release_block();

  /**
   * This will be called whenever an MNP connection is established.
   *
   * Alternative interface to callbacks.
   *
   * \return return 1, if callback should not be called anymore
   */
  virtual int on_connect() { return 0; }

  /**
   * Will be called if connection ended.
   *
   * Alternative interface to callbacks.
   *
   * \return return 1, if callback should not be called anymore
   */
  virtual int on_disconnect() { return 0; }

  /**
   * This will be called whenever a block of data is available.
   *
   * Alternative interface to callbacks. Available bytes
   * are returned by get_block_size() and can then be read using
   * get_block(). They must then be relased back to the class 
   * using release_block().
   *
   * \return return 1, if callback should not be called anymore
   */
  virtual int on_receive() { return 0; }

  virtual int write(const unsigned char *data, int n) {
		return stream_->write(data, n);
	}
  virtual int available() {
		return stream_->available();
	}
  virtual int read(unsigned char *dest, int n) {
		return stream_->read(dest, n);
	}
  virtual int is_open() {
		return stream_ ? stream_->is_open() : 0;
	}

protected:

  static void crc16(unsigned short &crc, unsigned char d);

  virtual int on_read();
  int handle_block_();
  void send_LT_ack_();
  void stop_keep_alive_();
  void start_keep_alive_();
  void send_data_block2(unsigned char *data, int size);
  static void keep_alive_(void*);
	void draw();

private:
	Flio_Stream *stream_;
  int state_;
  int index_;
  int rxCnt_;
  int txCnt_;
  unsigned char *buffer_;
  unsigned char esc_;

  unsigned short crc_, crcIn_;
};


#endif

//
// End of "$Id$".
//
