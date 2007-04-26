//
// "$Id$"
//
// Flmm_Filename_Input implementation for the FLMM extension to FLTK.
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

/*
 *  Log minor error messages onto a stack. When the application
 *  finally fails, we offer a message box and the option to get
 *  more information, i.e. the previous messages
 */

/** \class Flmm_Message
 *  
 * Flmm_Message gives easy access to a logging and message dialog system
 * 
 * Log minor error messages onto a stack. When the application
 * finally fails, we offer a message box and the option to get
 * more information, i.e. the previous messages
 *
 * \todo I want a 'copy to clipboard' button!
 * \todo There is currently no support for message ID's.
 * \todo No support for image icons or either.
 * \todo There should be a function to add a log entry to the bottom
 *       of the stack as well.
 */

#include "../FL/Flmm_Message.H"
#include "../FL/Flmm.H"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#ifndef WIN32
# include <errno.h>
#endif

#include <FL/x.H>
#include <FL/Fl.H>
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Scroll.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

const char *label_more_information = "@-1> more information";
const char *label_less_information = "@-12> less information";

char Flmm_Message::buffer[];
char Flmm_Message::windowLabel[];

char *Flmm_Message::status_ = 0L;
char Flmm_Message::isAsk = 0;
char Flmm_Message::isChoice = 0;
char Flmm_Message::isMore = 0;

static Fl_Window *message_form;
static Fl_Box *message_box, *browser_box;
static Fl_Box *icon;
static Fl_Button *button[3];
static Fl_Button *more;
static Fl_Input *input;
static Fl_Scroll *browser;

static const char *choice_label[3];

char Flmm_Message::next_icon = 0;
unsigned int Flmm_Message::next_id = 0;
const Fl_Image *Flmm_Message::next_image = 0;
char *Flmm_Message::title = 0;
char *Flmm_Message::sys_msg = 0L;

// ----------------------------------------------------------------------------

/*
 * This local class manages log entries
 */
class LogEntry {
public:
  LogEntry(char, unsigned int, const Fl_Image*, const char*);
  ~LogEntry();
  static void clear(int n=-1);
  char icon;
  unsigned int id;
  const Fl_Image *image;
  const char *text;
  static int n, N;
  static LogEntry **list;
};

int LogEntry::n = 0;
int LogEntry::N = 0;
LogEntry **LogEntry::list = 0L;

LogEntry::LogEntry(char Icon, unsigned int Id, const Fl_Image *Image, const char *Text) 
: icon(Icon),
  id(Id),
  image(Image),
  text(Text)
{
  if (text)
    text = strdup(text);
  if (n==N) {
    N += 20;
    list = (LogEntry**)realloc(list, N*sizeof(LogEntry**));
  }
  list[n] = this;
  n++;
}

LogEntry::~LogEntry() 
{
  if (text)
    free(const_cast<char*>(text));
}

void LogEntry::clear(int nc) {
  if (nc<0 || nc>n) 
    nc = n;
  while (nc>0) {
    n--; nc--;
    delete list[n];
    list[n] = 0;
  }
}

// ----------------------------------------------------------------------------


/**
 * Show an alert dialog.
 *
 * \param msg is a printf style format string followed by any number of parameters
 */
void Flmm_Message::alert(const char *msg, ...) 
{
  if (msg) {
    if (next_image==0 && next_icon==0)
      set_icon('!');
    va_list args;
    va_start(args, msg);
    va_log(msg, args);
    va_end(args);
  }
  show();
}

/**
 * Show an alert dialog with an ID.
 *
 * \param id (not yet supported)
 * \param msg is a printf style format string followed by any number of parameters
 */
void Flmm_Message::alert(unsigned int id, const char *msg, ...) 
{
  if (msg) {
    set_id(id);
    if (next_image==0 && next_icon==0)
      set_icon('!');
    va_list args;
    va_start(args, msg);
    va_log(msg, args);
    va_end(args);
  }
  show();
}

/**
 * Show an information dialog.
 *
 * \param msg is a printf style format string followed by any number of parameters
 */
void Flmm_Message::message(const char *msg, ...) 
{
  if (msg) {
    if (next_image==0 && next_icon==0)
      set_icon('i');
    va_list args;
    va_start(args, msg);
    va_log(msg, args);
    va_end(args);
  }
  show();
}

/**
 * Show an information dialog with an ID.
 *
 * \param id (not yet supported)
 * \param msg is a printf style format string followed by any number of parameters
 */
void Flmm_Message::message(unsigned int id, const char *msg, ...) 
{
  if (msg) {
    set_id(id);
    if (next_image==0 && next_icon==0)
      set_icon('i');
    va_list args;
    va_start(args, msg);
    va_log(msg, args);
    va_end(args);
  }
  show();
}

/**
 * Show an question dialog.
 *
 * \param msg is a printf style format string followed by any number of parameters
 */
int Flmm_Message::ask(const char *msg, ...) 
{
  if (msg) {
    if (next_image==0 && next_icon==0)
      set_icon('?');
    va_list args;
    va_start(args, msg);
    va_log(msg, args);
    va_end(args);
  }
  isAsk = 1;
  isChoice = 0;
  return show();
}

/**
 * Show an question dialog with an ID.
 *
 * \param id (not yet supported)
 * \param msg is a printf style format string followed by any number of parameters
 */
int Flmm_Message::ask(unsigned int id, const char *msg, ...) 
{
  if (msg) {
    set_id(id);
    if (next_image==0 && next_icon==0)
      set_icon('?');
    va_list args;
    va_start(args, msg);
    va_log(msg, args);
    va_end(args);
  }
  isAsk = 1;
  isChoice = 0;
  return show();
}

/**
 * Show a choice dialog with three possible replies.
 *
 * \param msg is a printf style format string followed by any number of parameters
 */
int Flmm_Message::choice(const char *msg, const char *b0, const char *b1, const char *b2, ...) 
{
  if (msg) {
    if (next_image==0 && next_icon==0)
      set_icon('?');
    va_list args;
    va_start(args, b2);
    va_log(msg, args);
    va_end(args);
  }
  isAsk = 0;
  isChoice = 1;
  choice_label[0] = b0;
  choice_label[1] = b1;
  choice_label[2] = b2;
  return show();
}

/**
 * Show a choice dialog with three possible replies and an ID.
 *
 * \param id (not yet supported)
 * \param msg is a printf style format string followed by any number of parameters
 */
int Flmm_Message::choice(unsigned int id, const char *msg, const char *b0, const char *b1, const char *b2, ...) 
{
  if (msg) {
    set_id(id);
    if (next_image==0 && next_icon==0)
      set_icon('?');
    va_list args;
    va_start(args, b2);
    va_log(msg, args);
    va_end(args);
  }
  isAsk = 0;
  isChoice = 1;
  choice_label[0] = b0;
  choice_label[1] = b1;
  choice_label[2] = b2;
  return show();
}

/**
 * Show a dialog using the current top log entry as the message text.
 */
int Flmm_Message::show()
{
  if (!LogEntry::n)
    return -1;
  make_form();
  //sprintf(windowLabel, "Robowerk information [%06x]", id);
  /// \todo I want custom messages here!
  sprintf(windowLabel, "Flmm_Message");
  message_form->label(windowLabel);
  LogEntry *log = LogEntry::list[LogEntry::n-1];
  message_box->label(log->text);
  char b[2] = { log->icon, 0 }; 
  icon->label(b);
  return show_form();
}

/**
 * Set a unique ID for the next log message.
 * \param id set the ID for the next log item
 * \todo id's are not supported yet
 */
void Flmm_Message::set_id(unsigned int id) {
  next_id = id;
}

/**
 * Set the icon that is to be used for the next log entry.
 * This icon can be any image. 
 * \param image the box size is 40x40 pixel
 */
void Flmm_Message::set_icon(const Fl_Image *image) {
  next_icon = 0;
  next_image = image;
}

/**
 * Set the icon that is to be used for the next log entry.
 *
 * \param i common icons are '!' for alerts, 'i' for information, and '?' 
 *        for questions.
 */
void Flmm_Message::set_icon(char i) {
  next_icon = i;
  next_image = 0;
}

/**
 * Set the title bar text for all following dialog boxes.
 * \param text a short text, usually name and version of 
 *        the application
 */
void Flmm_Message::set_title(const char *text) 
{
  if (title)
    free(title);
  title = strdup(text);
}

/**
 * Add an entry to the log stack.
 *
 * \param msg is a printf style format string followed by any number of parameters
 */
void Flmm_Message::log(const char *msg, ...) 
{
  va_list args;
  va_start(args, msg);
  va_log(msg, args);
  va_end(args);
}

/**
 * Add an entry to the log stack.
 *
 * \param id (not yet supported)
 * \param msg is a printf style format string followed by any number of parameters
 */
void Flmm_Message::log(unsigned int id, const char *msg, ...) 
{
  set_id(id);
  va_list args;
  va_start(args, msg);
  va_log(msg, args);
  va_end(args);
}

/**
 * Add an entry to the log stack.
 *
 * \param msg is a printf style format string followed by any number of parameters
 * \param args variable argument list
 */
void Flmm_Message::va_log(const char *msg, va_list args) 
{
  /// \todo use vsnprintf and expand the buffer as needed
  vsprintf(buffer, msg, args);
  new LogEntry(next_icon, next_id, next_image, buffer);
}

/**
 * Create the message dialog box.
 */
void Flmm_Message::make_form()
{
  if (message_form) {
    message_form->size(410,103);
    return;
  }
  Fl_Window *w = message_form = new Fl_Window(410,103,"");

  message_box = new Fl_Box(60, 25, 340, 20);
  message_box->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE|FL_ALIGN_WRAP);
  message_box->labelfont(fl_message_font_);
  message_box->labelsize(fl_message_size_);

  input = new Fl_Input(60, 37, 340, 23);
  input->hide();
  input->labelfont(fl_message_font_);
  input->labelsize(fl_message_size_);
  input->textfont(fl_message_font_);
  input->textsize(fl_message_size_);

  icon = new Fl_Box(10, 10, 50, 50);
  icon->box(FL_THIN_UP_BOX);
  icon->labelfont(FL_TIMES_BOLD);
  icon->labelsize(34);
  icon->color(FL_WHITE);
  icon->labelcolor(FL_BLUE);

  for (int i=0; i<3; i++) {
    button[i] = new Fl_Button(310-i*100, 70, 90, 23);
    button[i]->align(FL_ALIGN_INSIDE|FL_ALIGN_WRAP);
    button[i]->labelfont(fl_message_font_);
    button[i]->labelsize(fl_message_size_);
  }
  button[0]->shortcut("^[");
  button[1]->hide();
  button[2]->hide();

  more = new Fl_Button(20, 75, 140, 20);
  more->labeltype(FL_SYMBOL_LABEL);
  more->labelfont(fl_message_font_);
  more->labelsize(fl_message_size_-2);
  more->box(FL_NO_BOX);
  more->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);

  browser_box = new Fl_Box(10, 105, 390, 200);
  browser_box->box(FL_THIN_DOWN_BOX);
  browser_box->hide();
  browser = new Fl_Scroll(10, 105, 390, 200);
  browser->labelfont(fl_message_font_);
  browser->labelsize(fl_message_size_);

  //browser->color(FL_LIGHT2);
  //browser->box(FL_DOWN_BOX);
  //w->resizable(new Fl_Box(60,10,110-60,27));
  w->end();
  w->set_modal();
  if (Flmm::is_plastic_scheme()) {
    message_box->box(FL_PLASTIC_THIN_DOWN_BOX);
    input->box(FL_PLASTIC_DOWN_BOX);
    icon->box(FL_PLASTIC_UP_BOX);
    button[0]->box(FL_PLASTIC_UP_BOX);
    button[1]->box(FL_PLASTIC_UP_BOX);
    button[2]->box(FL_PLASTIC_UP_BOX);
    browser->box(FL_PLASTIC_THIN_DOWN_BOX);
  }
}

/**
 * Resize the form to match the current message.
 */
void Flmm_Message::resize_form() 
{
  int	i;
  int	message_w, message_h;
  int   more_w, more_h;
  int	icon_size = 50;
  int	dlg_h;
  int	button_w[3], button_h[3];
  int   browser_h, bh;
  int	x, w, h, max_w, max_h;

  fl_font(fl_message_font_, fl_message_size_);
  message_w = 340; message_h = 0;
  fl_measure(message_box->label(), message_w, message_h);

  message_w += 10;
  message_h += 10;
  if (message_w < 340)
    message_w = 340;
  if (message_h < 30)
    message_h = 30;

  fl_font(button[0]->labelfont(), button[0]->labelsize());

  memset(button_w, 0, sizeof(button_w));
  memset(button_h, 0, sizeof(button_h));

  for (max_h = 25, i = 0; i < 3; i ++)
    if (button[i]->visible())
    {
      fl_measure(button[i]->label(), button_w[i], button_h[i]);

      if (i == 1)
        button_w[1] += 20;

      if (button_w[i]<60) 
        button_w[i] = 60;

      button_w[i] += 30;
      button_h[i] += 10;

      if (button_h[i] > max_h)
        max_h = button_h[i];
    }

  w = more_w = 0; h = more_h = 0;
  fl_font(fl_message_font_, fl_message_size_-2);
  fl_measure(label_more_information, w, h);
  fl_measure(label_less_information, more_w, more_h);
  if (more_w < w) more_w = w;
  if (more_h < h) more_h = h;
  if (more_h > max_h) max_h = more_h;
  more_w += fl_height() + 7; // make space for the symbol

  if (input->visible()) dlg_h = message_h + 25;
  else dlg_h = message_h;
  if (dlg_h<icon_size) dlg_h = icon_size;

  max_w = message_w + 10 + icon_size;
  w     = button_w[0] + button_w[1] + button_w[2] - 10;

  if (w > max_w)
    max_w = w;

  message_w = max_w - 10 - icon_size;

  /// \todo the browser code below doesn't work because the browser position is not yet known!
  fl_font(fl_message_font_, fl_message_size_-1);
  browser_h = 0;
  bh = 0;
  for (i=0; i<browser->children(); i++) {
    Fl_Widget *b = browser->child(i);
    if (b==&browser->scrollbar || b==&browser->hscrollbar) {
    } else if (b->h()<=2) {
      b->resize(browser->x()+30, browser->y()+browser_h+5, browser->w()-60, b->h());
      browser_h += b->h()+8;
    } else {
      int bbw = browser->w()-21, bbh = 0;
      fl_measure(b->label(), bbw, bbh);
      b->resize(browser->x()+3, browser->y()+browser_h+2, browser->w()-21, bbh);
      browser_h += bbh+2;
    }
  }
  browser_h += 19;
  if (browser_h>300) browser_h = 300;
  if (browser_box->visible()) {
    bh = browser_h;
  }

  w = max_w + 20;
  h = max_h + 30 + dlg_h;

  message_form->size(w, h+bh);
  message_form->size_range(w, h+bh, w, h+bh);

  message_box->resize(20 + icon_size, 10, message_w, message_h);
  icon->resize(10, 10, icon_size, icon_size);
  icon->labelsize((uchar)(icon_size - 10));
  input->resize(20 + icon_size, 10 + message_h, message_w, 25);

  for (x = w, i = 0; i < 3; i ++)
    if (button_w[i]) {
      x -= button_w[i];
      button[i]->resize(x, h - 10 - max_h, button_w[i] - 10, max_h);
    }

  more->resize(10, h - 7 - max_h, more_w - 10, max_h-5);

  browser_box->resize(10, h, w-20, browser_h-10);
  browser->resize(11, h+1, w-22, browser_h-12);
  browser->scrollbar.hide();
  browser->hscrollbar.hide();
}

/**
 * Show the dialog box and handle the event loop.
 */
int Flmm_Message::show_form()
{
  isMore = 0;

  int by = 0;
  browser->clear();
  browser->begin();
  for (int i = LogEntry::n-2; i>=0; i--) {
    LogEntry *log = LogEntry::list[i];
    if (log->text) {
      Fl_Box *box = new Fl_Box(browser->x()+10, browser->y()+by, browser->w()-20, 18, log->text);
      box->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE|FL_ALIGN_WRAP);
      box->labelfont(fl_message_font_);
      box->labelsize(fl_message_size_-1);
      by += 18;
      // draw a very skinny box as a seperator
      if (i!=0) { 
        Fl_Box *box2 = new Fl_Box(browser->x()+10, browser->y()+by, browser->w()-20, 2);
        box2->box(FL_THIN_DOWN_BOX);
        //(new Fl_Box(browser->x()+10, browser->y()+by, browser->w()-20, 1))->box(FL_BORDER_BOX);
        by+=2;
      }
    }
  }
  browser->end();

  browser_box->hide();
  more->label(label_more_information);
  if (isAsk) {
    button[0]->show();
    button[0]->label(fl_no);
    button[1]->show();
    button[1]->label(fl_yes);
    button[2]->hide();
  } else if (isChoice) {
    for (int i=0; i<3; i++) {
      if (choice_label[i]) {
        button[i]->show();
        button[i]->label(choice_label[i]);
        button[i]->show();
      } else {
        button[i]->hide();
      }
    }
  } else {
    button[0]->hide();
    button[1]->show();
    button[1]->label(fl_ok);
    button[2]->hide();
  }
  if (get_log_size()<=1)
    more->hide();
  else
    more->show();

  resize_form();
  message_form->hotspot(button[1]);
  message_form->show();

  int r = 1;
  for (;;) {
    Fl_Widget *o = Fl::readqueue();
    if (!o) Fl::wait();
    else if (o == button[0]) {r = 0; break;}
    else if (o == button[1]) {r = 1; break;}
    else if (o == button[2]) {r = 2; break;}
    else if (o == message_form) {r = 0; break;}
    else if (o == more) {
      if (isMore) {
	more->label(label_more_information);
        browser_box->hide();
      } else {
	more->label(label_less_information);
        browser_box->show();
      }
      isMore = !isMore;
      resize_form();
    }
  }

  message_form->hide();
  browser->clear();
  clear_log();
  isMore = 0;
  isAsk = 0;
  isChoice = 0;
  return r;
}


/**
 * Clear the error log stack.
 */
void Flmm_Message::clear_log()
{
  LogEntry::clear();
}

/**
 * Remove some of the last entries from the log stack.
 * \param n number of log messages to remove (default: all)
 */
void Flmm_Message::pop_log(int n) {
  LogEntry::clear(n);
}

/**
 * Get the number of log entries.
 */
int Flmm_Message::get_log_size()
{
  return LogEntry::n;
}

/**
 * Return the text message associated with an OS specific erro number.
 */
const char *Flmm_Message::system_message(int err)
{
  if (err==0xdecaff) {
    err = last_error();
  }
#ifdef WIN32
  if (err==0) {
    return "No error";
  } else {
	  LPTSTR szMsgBuf;
	  DWORD dwErr = (DWORD)err;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dwErr,
			MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
			(LPTSTR) &szMsgBuf,
			0,
			NULL );
    if (sys_msg)
      free(sys_msg);
    sys_msg = strdup(szMsgBuf);
	  LocalFree( szMsgBuf );
    return sys_msg;
  }
#else
  return strerror();
#endif
}


/**
 * Return an OS specific error number.
 */
int Flmm_Message::last_error() {
#ifdef WIN32
  DWORD err = GetLastError();
  return (int)err;
#else
  return errno;
#endif
}

//
// End of "$Id$".
//
