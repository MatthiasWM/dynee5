/**
 * This file is obsolete with the introduction of NEWT/0
 */

#ifndef NTK_PIPE
#define NTK_PIPE

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include <stdio.h>


/**
 * A simple pipe class that moves information from A to B.
 *
 * This class must be derived to do anything meaningful.
 *
 * \todo There is no error checking implemented at this point.
 * \todo There is no end-of-file chaecking.
 */
class NtkPipe
{

public:

  /** 
   * Create and open a pipe.
   *
   * There is no function to open or close a pipe later. All
   * allocations are done by the constructor and all cleanup
   * is done in the destructor.
   */
  NtkPipe() { }

  /**
   * Close an open pipe and free all resources.
   *
   * There is no function to close a pipe. Instead it should
   * simply be deleted.
   */
  virtual ~NtkPipe() { }

  /**
   * Let us know if a pipe was actually opened.
   *
   * \return 0 if we were not able to open this pipe in the constructor
   */
  virtual char isOpen() = 0;

  /**
   * Read a single character from the pipe.
   *
   * \return the current character and increment the reader.
   */
  virtual unsigned char getc() { return 0; }

  /**
   * Return the current position of the reader.
   *
   * \return the absolute index of the next byte available.
   */
  virtual int getPos() = 0;

  /**
   * Read multiple bytes into a buffer.
   *
   * \param[out] buffer pointer to a data buffer of sufficient size
   * \param[in] n number of bytes to read
   * \return number of bytes actually read
   */
  virtual int read(void *buffer, int n) { return 0; }

  /**
   * Write a formatted string to the pipe.
   */
  virtual int printf(const char *format, ...) { return 0; }
};


/**
 * Pipe bytes from a named file.
 *
 * This class uses ANSI calls to pipe data from a named
 * file to our application.
 *
 * \todo There is no error checking implemented at this point.
 * \todo There is no end-of-file chaecking.
 */
class NtkFilePipe : public NtkPipe
{
  FILE *file_;
  char *filename_;

public:

  /**
   * Create a pipe by opening a file.
   *
   * \param[in] filename name with optional path of file to open
   */
  NtkFilePipe(const char *filename);

  virtual ~NtkFilePipe();
  virtual char isOpen();
  virtual unsigned char getc();
  virtual int getPos();
  virtual int read(void*, int);
};


/**
 * Pipe bytes to a named file.
 *
 * This class uses ANSI calls to pipe data to a named
 * file from our application.
 *
 * \todo There is no error checking implemented at this point.
 * \todo There is no end-of-file chaecking.
 */
class NtkFileWritePipe : public NtkPipe
{
  FILE *file_;
  char *filename_;
  int indent_;

public:

  /**
   * Create a pipe by opening a file.
   *
   * \param[in] filename name with optional path of file to open
   */
  NtkFileWritePipe(const char *filename);

  virtual ~NtkFileWritePipe();
  virtual char isOpen();
  virtual int getPos();
  virtual int printf(const char *format, ...);
  const char *indent();
  const char *incrIndent();
  const char *decrIndent();
};


/**
 * Pipe bytes from memory.
 *
 * This class pipes bytes from a block of memory.
 *
 * \todo There is no error checking implemented at this point.
 * \todo There is no end-of-file chaecking.
 */
class NtkMemPipe : public NtkPipe
{
  unsigned char *start_;
  unsigned char *curr_;
  int n_;

public:

  /**
   * Create a pipe from a memory block.
   *
   * \param[in] start address of memory block
   * \param[in] n size of memry block
   */
  NtkMemPipe(unsigned char *start, int n);

  virtual ~NtkMemPipe();
  virtual char isOpen();
  virtual unsigned char getc();
  virtual int getPos();
  virtual int read(void*, int);
};


#endif

