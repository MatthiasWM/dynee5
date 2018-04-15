//
// "$Id$"
//
// Flio_Pipe header file for the FLIO extension to FLTK.
//
// Copyright 2002-2018 by Matthias Melcher.
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

#ifndef FLIO_PIPE
#define FLIO_PIPE


#include "fltk/Flio_Stream.h"

/**
 * A pipe communication widget for FLTK1.
 *
 * This widget manages communication over twp Unix style pipes.
 */
class Flio_Pipe : public Flio_Stream
{
public:

    /**
     * Standard widget interface constructor.
     */
    Flio_Pipe(int X, int Y, int W, int H, const char *L=0L);

    /**
     * Constructor for superwidgets.
     */
    Flio_Pipe(Flio_Stream *super);

    /**
     * The destructor closes any open connections.
     */
    virtual ~Flio_Pipe();

    /**
     * Open a connection.
     *
     * All connections are opened with 8N1 for simplicity.
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
    int write(const unsigned char *data, int n);

    /**
     * Return the number of bytes available.
     *
     * \return number of bytes available in the buffer
     */
    int available();

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
    int read(unsigned char *dest, int n);

    /**
     * Close the serial connection.
     */
    virtual void close();

    /**
     * Check if the serial line is open.
     *
     * \return 0 if the connection is closed
     */
    int is_open();

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

    //virtual int on_error();

    virtual void draw();
    void redraw();

protected:

    virtual int on_read_();
    //virtual int on_error_();

private:

    static void on_read_cb(void *);
    //static void on_error_cb(void *);
    static void lights_cb(void *);

    unsigned char *ring_ = nullptr;
    int NRing_ = 0;
    int ringHead_ = 0;
    int ringTail_ = 0;
    char *portname_ = nullptr;
    char rxActive_ = 0, pRxActive_ = 0;
    char txActive_ = 0, pTxActive_ = 0;

    int available_to_end();
    int free_to_end();

#ifdef WIN32
    void reader_thread();
    static void __cdecl reader_thread_(void*);
    HANDLE port_ = INVALID_HANDLE_VALUE;
    HANDLE event_ = INVALID_HANDLE_VALUE;
    unsigned long thread_ = -1;
    OVERLAPPED overlapped_;
#else
    int sendPort_ = -1;
    int recvPort_ = -1;
    static void reader_cb(int, void*);
    void reader();
#endif
};


#endif

//
// End of "$Id$".
//
