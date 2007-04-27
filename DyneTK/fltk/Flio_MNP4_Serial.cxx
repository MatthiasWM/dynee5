//
// "$Id$"
//
// Flio_Mnp4_Serial implementation for the FLIO extension to FLTK.
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



#include "Flio_MNP4_Serial.h"

#include <FL/Fl.H>

#include <stdlib.h>
#include <stdio.h>


Flio_Mnp4_Serial::Flio_Mnp4_Serial(int X, int Y, int W, int H, const char *L)
: Flio_Serial_Port(X, Y, W, H, L),
  state_(0),
  index_(0),
  rxCnt_(0),
  txCnt_(0),
  buffer_(0L),
  esc_(0x33)
{
  buffer_ = (unsigned char*)malloc(2048);
}


Flio_Mnp4_Serial::~Flio_Mnp4_Serial()
{
  stop_keep_alive_();
  if (buffer_)
    free(buffer_);
}


int Flio_Mnp4_Serial::open(const char *port, int bps)
{
  return Flio_Serial_Port::open(port, bps);
}


void Flio_Mnp4_Serial::close()
{
  stop_keep_alive_();
  // FIXME: send close block
  if (!on_disconnect() && callback())
    do_callback();
  Flio_Serial_Port::close();
}


void Flio_Mnp4_Serial::release_block()
{
  state_ = 0;
}


int Flio_Mnp4_Serial::get_block_size() 
{
  return index_;
}


unsigned char *Flio_Mnp4_Serial::get_block() 
{
  return buffer_;
}



int Flio_Mnp4_Serial::on_read_()
{
  for (;;) {
    if (state_==7) 
      break;
    unsigned char c;
    int n = read(&c, 1);
    if (!n)
      break;
    switch (state_) {
      case 0: // nothing received yet, wait for 0x16
        if (c==0x16) 
          state_++;
        break;
      case 1: // we have the 0x16, now we need the 0x10
        if (c==0x10) {
          state_++;
          index_ = 0;
        } else
          state_ = 0;
        break;
      case 2: // we have the 0x16 0x10, now we need the 0x02
        if (c==0x02) {
          state_++;
          index_ = 0;
          crc_ = 0;
        } else
          state_ = 0;
        break;
      case 3: // now here's our block data, but a 0x10 may indicate the end of the block
        if (c==0x10) {
          state_++;
          break;
        }
block_data:
        buffer_[index_++] = c;
        crc16(crc_, c);
        break;
      case 4: // if we now receive a 0x03, we reached the end of the block
        if (c==0x03) {
          state_++;
          crc16(crc_, c);
          break;
        } else {
          state_--;
          goto block_data;
        }
      case 5: // get the first checksum byte
        state_++;
        crcIn_ = c;
        break;
      case 6: // get the second checksum byte
        state_++;
        crcIn_ |= (c<<8);
        // verify the calculated and the received checksum
        if (crc_ != crcIn_) {
          printf("Checksum error! Please resend everything after last correct block.\n");
          send_LT_ack_();	// send the ack for the last block we understood
		  start_keep_alive_(); // delay the next keep-alive
		  state_ = 0;
        } else {
		  // now the block is complete and we can call the callback
		  handle_block_();
		}
        break;
    }
  }
  return 1;
}

static unsigned char LR_ack[] = {
  0x1d, 0x01, 0x02, 0x01, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x02, 0x01, 0x02, 0x03, 0x01, 
  0x08, 0x04, 0x02, 0x40, 0x00, 0x08, 0x01, 0x03, 0x0E, 0x04, 0x02, 0x04, 0x00, 0xFA,
};

void Flio_Mnp4_Serial::send_LT_ack_()
{
  unsigned char LT_ack[] = { 3, 5, 0, 8 };
  LT_ack[2] = rxCnt_;
  send_block(LT_ack, sizeof(LT_ack));
}


void Flio_Mnp4_Serial::stop_keep_alive_()
{
  Fl::remove_timeout(keep_alive_, this);
}

void Flio_Mnp4_Serial::start_keep_alive_()
{
  Fl::remove_timeout(keep_alive_, this);
  Fl::add_timeout(3.0, keep_alive_, this);
}

void Flio_Mnp4_Serial::keep_alive_(void *t)
{
  Flio_Mnp4_Serial *This = (Flio_Mnp4_Serial*)t;
  This->send_LT_ack_();
  Fl::repeat_timeout(3.0, keep_alive_, This);
}

/**
 * At this point, we know that the checksum of the block is correct.
 * We must now classify the block and react accordingly. Some blocks 
 * require handshake or acknowledge to be sent.
 *
 * Protocol based blocks will all be handled here. Data blocks will
 * be sent on to on_block_receive() and then to the callback.
 */
int Flio_Mnp4_Serial::handle_block_()
{
  if (index_<2) // minimum header length
    return -1;
  switch (buffer_[1]) {
    case 1: // LR - Link Request
      release_block();
      // Newton wants to connect. Reply how *we* want to connect.
      send_block(LR_ack, sizeof(LR_ack));
      rxCnt_ = txCnt_ = 0;
      esc_ = 0x33;
      on_connect();
      start_keep_alive_();
      break;
    case 2: // LD - Link Disconect
      release_block();
      stop_keep_alive_();
      //send_block(LD_ack, sizeof(LD_ack));
      //Sleep(300);
      on_disconnect();
      Flio_Mnp4_Serial::close();
      break;
    case 4: // LT - Link Transfer
//printf("rxcnt vs rxblock: %d %d\n", rxCnt_, buffer_[2]);
      if ( ((rxCnt_+1)&0xff)!=buffer_[2]) {
		// we received an out-of-order block! 
		// Tell Newton which last correct block we received.
printf("ASK FOR A RESEND OF %d\n", rxCnt_+1);
		send_LT_ack_();
	  } else {
        rxCnt_ = buffer_[2];
        send_LT_ack_();
        start_keep_alive_(); // restart the timer
        if (!on_receive() && callback())
          do_callback();
	  }
      release_block();
      break;
    case 5: // LA - Link Acknowledge
      // FIXME we will not catch any transfer errors this way!
      //do_callback();
      release_block();
      break;
    case 6: // LN - Link Attention
    case 7: // LNA - Link Attention Acknowledge
    default:
      release_block();
      printf("Flio_Mnp4_Serial: unsupported block type %d\n", buffer_[1]);
      return -1;
  }
  return 0;
}


void Flio_Mnp4_Serial::send_block(unsigned char *data, int size)
{
  static unsigned char hdr[] = { 0x16, 0x10, 0x02 };
  static unsigned char ftr[] = { 0x10, 0x03 };

  unsigned char buf[1024];
  unsigned char *d = buf;

  int i;
  *d++ = hdr[0];
  *d++ = hdr[1];
  *d++ = hdr[2];
  unsigned short crc = 0;
  for (i=0; i<size; i++) {
    unsigned char c = data[i];
    if (i==2 && data[1]==4) 
      c = ++txCnt_;
    crc16(crc, c);
    *d++ = c;
    if (c==0x10) 
      *d++ = c;
    if (c==esc_) {
      *d++ = 1;
      crc16(crc, 1);
	  printf("--- CONTROL BLOCK *NOT* changing esc from 0x%02x to 0x%02x\n", esc_, esc_+51);
      //esc_ += 51;
    }
  }
  *d++ = ftr[0];
  *d++ = ftr[1];
  crc16(crc, ftr[1]);
  *d++ = crc;
  *d++ = crc>>8;
  write(buf, d-buf);
}


void Flio_Mnp4_Serial::send_data_block(unsigned char *data, int size)
{
  const int max_block = 250;
  int i;
  unsigned char *src = data;
  while (size>max_block) {
    send_data_block2(src, max_block);
    src += max_block;
    size -= max_block;
  }
  send_data_block2(src, size);
}


void Flio_Mnp4_Serial::send_data_block2(unsigned char *data, int size)
{
  unsigned char buf[1024];
  unsigned char *d = buf;
  unsigned short crc = 0;
  int i;

  *d++ = 0x16;
  *d++ = 0x10;
  *d++ = 0x02;
  *d++ = 0x02; crc16(crc, 0x02);
  *d++ = 0x04; crc16(crc, 0x04);
  ++txCnt_;
  *d++ = txCnt_; crc16(crc, txCnt_);
  if (txCnt_==0x10) 
    *d++ = txCnt_;
  for (i=0; i<size; i++) {
    unsigned char c = data[i];
    crc16(crc, c);
    *d++ = c;
    if (c==0x10) 
      *d++ = c;
    if (c==esc_) {
      *d++ = 1;
      crc16(crc, 1);
	  printf("--- DATA BLOCK: changing esc from 0x%02x to 0x%02x\n", esc_, esc_+51);
      esc_ += 51;
    }
  }
  *d++ = 0x10;
  *d++ = 0x03;
  crc16(crc, 0x03);
  *d++ = crc;
  *d++ = crc>>8;
  write(buf, d-buf);
}


void Flio_Mnp4_Serial::crc16(unsigned short &crc, unsigned char d) 
{
  static unsigned short crctab[256] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
  };

  crc = ((crc>>8)&0x00ff)^crctab[(crc&0xff)^d];
}

//
// End of "$Id$".
//
