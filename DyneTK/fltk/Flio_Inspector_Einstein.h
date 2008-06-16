//
// "$Id$"
//
// Flio_Inspector_Einstein header file for the FLIO extension to FLTK.
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

#ifndef FLIO_INSPECTOR_EINSTEIN
#define FLIO_INSPECTOR_EINSTEIN

#include "Flio_Inspector.h"


/**
 * Handle a subste of the Inspector protocol when dealing with Einsten.
 */
class Flio_Inspector_Einstein : public Flio_Inspector
{
public:

  Flio_Inspector_Einstein(int X, int Y, int W, int H, const char *L=0L);
  virtual ~Flio_Inspector_Einstein();
  virtual int open(const char *port, int bps);
  virtual void close();
  virtual void cancel();
  virtual int is_open() { return is_open_; }
  virtual int sendScript(const char *script);
  virtual int deletePackage(const char *symbol);
  virtual int sendPackage(const char *filename);
  virtual int is_serial() { return 0; }
  virtual int is_einstein() { return 1; }
  
private:

  virtual int sendCmd(const char *cmd, const char *script);

  bool is_open_;
  ComponentInstance cInst_;
};


#endif

//
// End of "$Id$".
//
