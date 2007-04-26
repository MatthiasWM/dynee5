//
// "$Id$"
//
// Flmm_Message header file for the FLMM extension to FLTK.
//
// Copyright 2002-2004 by Matthias Melcher.
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
// Please report all bugs and problems to "flmm@matthiasm.com".
//

#ifndef _FLMM_MESSAGE_H_
#define _FLMM_MESSAGE_H_

#include <stdarg.h>

// declared
class Flmm_Message;

// referenced
class Fl_Output;
class Fl_Window;
class Fl_Box;
class Fl_Browser;
class Fl_Button;
class Fl_Image;


class Flmm_Message {

public:
  /* show message boxes */
  static void alert(const char *msg=0L, ...);
  static void alert(unsigned int, const char *msg=0L, ...);
  static void message(const char *msg=0L, ...);
  static void message(unsigned int, const char *msg=0L, ...);
  static int  ask(const char *msg=0L, ...);
  static int  ask(unsigned int, const char *msg=0L, ...);
  static int  choice(const char*, const char*, const char*, const char*, ...);
  static int  choice(unsigned int, const char*, const char*, const char*, const char*, ...);
  static int  show();

  /* log messages */
  static void log(const char *, ...);
  static void log(unsigned int, const char *, ...);

  /* handle the log stack */
  static int  get_log_size();
  static void clear_log();
  static void pop_log(int n=1);

  /* additional information */
  static void set_id(unsigned int);
  static void set_icon(const Fl_Image*);
  static void set_icon(char);
  static void set_title(const char*);

  static const char *system_message(int err=0xdecaff);
  static int last_error();

  /* obsolete */
  //static int choice(const char *fmt, 
  //    const char *b0, const char *b1, const char *b2, ...);

protected:
  static void va_log(const char*, va_list); 

private:
  static char buffer[2048];
  static char windowLabel[128];
  static void make_form();
  static void resize_form();
  static int  show_form();
  static char isAsk, isMore, isChoice;
  static char * status_;
  static char next_icon;
  static unsigned int next_id;
  static const Fl_Image *next_image;
  static char *title;
  static char *sys_msg;
};

#endif

//
// End of "$Id$".
//
