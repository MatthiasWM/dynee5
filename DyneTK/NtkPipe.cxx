/**
 * This file is obsolete with the introduction of NEWT/0
 */


#include "NtkPipe.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


NtkFilePipe::NtkFilePipe(const char *filename)
: file_(0L),
  filename_(0L)
{
  if (filename && *filename)
    filename_ = strdup(filename);
  char buf[1024];
  if (filename_)
    file_ = fopen(filename_, "rb");
}


NtkFilePipe::~NtkFilePipe()
{
  if (file_)
    fclose(file_);
  if (filename_)
    free(filename_);
}


char NtkFilePipe::isOpen()
{
  return (file_!=0L);
}


unsigned char NtkFilePipe::getc()
{
  unsigned char c = (unsigned char)fgetc(file_);
  return c;
}


int NtkFilePipe::getPos()
{
  return ftell(file_);
}


int NtkFilePipe::read(void *dst, int n)
{
  int r = fread(dst, n, 1, file_);
  return r;
}


NtkMemPipe::NtkMemPipe(unsigned char *start, int n)
: start_(start),
  curr_(start),
  n_(n)
{
}


NtkMemPipe::~NtkMemPipe()
{
}


char NtkMemPipe::isOpen()
{
  return (start_!=0L);
}


unsigned char NtkMemPipe::getc()
{
  unsigned char c = *curr_;
  curr_++;
  return c;
}


int NtkMemPipe::getPos()
{
  return curr_ - start_;
}


int NtkMemPipe::read(void *dst, int n)
{
  memcpy(dst, curr_, n);
  curr_ += n;
  return n;
}


NtkFileWritePipe::NtkFileWritePipe(const char *filename)
: file_(0L),
  filename_(0L),
  indent_(0)
{
  if (filename && *filename)
    filename_ = strdup(filename);
  char buf[1024];
  if (filename_)
    file_ = fopen(filename_, "wb");
  if (file_) 
    fprintf(file_, ";;\n;; This file was generated from a Root object\n;;\n\n");
}


NtkFileWritePipe::~NtkFileWritePipe()
{
  if (file_) {
    fprintf(file_, "\n;;\n;; End of generated file\n;;\n");
    fclose(file_);
  }
  if (filename_)
    free(filename_);
}


char NtkFileWritePipe::isOpen()
{
  return (file_!=0L);
}


int NtkFileWritePipe::getPos()
{
  return ftell(file_);
}

const char *NtkFileWritePipe::indent() 
{
  static char space[] = 
    "                                        "
    "                                        ";
  return space+sizeof(space)-2*indent_-1;
}

const char *NtkFileWritePipe::incrIndent()
{
  const char *ret = indent();
  indent_+=2;
  return ret;
}

const char *NtkFileWritePipe::decrIndent()
{
  indent_-=2;
  return indent();
}

int NtkFileWritePipe::printf(const char *format, ...) 
{
  va_list vl;
  va_start(vl, format);
  int ret = vfprintf(file_, format, vl);
  va_end(vl);
  return ret;
}