//
// "$Id$"
//
// Fldtk_Rect_Slot_Editor implementation for the FLMM extension to FLTK.
//
// Copyright 2007 by Matthias Melcher.
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

#include "Fldtk_Rect_Slot_Editor.h"
#include "dtk/Dtk_Rect_Slot.h"

#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Box.H>

#include <stdio.h>


/*---------------------------------------------------------------------------*/

static char *widthStr = " Width: %d";
static char *heightStr = "Height: %d";


/*---------------------------------------------------------------------------*/
Fldtk_Rect_Slot_Editor::Fldtk_Rect_Slot_Editor(Fl_Group *container, Dtk_Rect_Slot *slot)
:   Fl_Group(container->x(), container->y(), container->w(), container->h()),
    slot_(slot)
{
    wLeft_  = new Fl_Int_Input(x()+50, y()+25, 70, 20, "Left:");
    wLeft_->textsize(12);
    wLeft_->textfont(FL_COURIER);
    wLeft_->labelsize(12);
    wLeft_->callback((Fl_Callback*)update_cb, this);
    wLeft_->when(FL_WHEN_CHANGED);
    wRight_  = new Fl_Int_Input(x()+180, y()+25, 70, 20, "Right:");
    wRight_->textsize(12);
    wRight_->textfont(FL_COURIER);
    wRight_->labelsize(12);
    wRight_->callback((Fl_Callback*)update_cb, this);
    wRight_->when(FL_WHEN_CHANGED);
    wTop_   = new Fl_Int_Input(x()+50, y()+55, 70, 20, "Top:");
    wTop_->textsize(12);
    wTop_->textfont(FL_COURIER);
    wTop_->labelsize(12);
    wTop_->callback((Fl_Callback*)update_cb, this);
    wTop_->when(FL_WHEN_CHANGED);
    wBottom_ = new Fl_Int_Input(x()+180, y()+55, 70, 20, "Bottom:");
    wBottom_->textsize(12);
    wBottom_->textfont(FL_COURIER);
    wBottom_->labelsize(12);
    wBottom_->callback((Fl_Callback*)update_cb, this);
    wBottom_->when(FL_WHEN_CHANGED);
    wWidth_ = new Fl_Box(x()+260, y()+25, 100, 20, " Width: 0");
    wWidth_->labelsize(12);
    wWidth_->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    wHeight_ = new Fl_Box(x()+260, y()+55, 100, 20, "Height: 0");
    wHeight_->labelsize(12);
    wHeight_->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    end();
    callback((Fl_Callback*)editor_cb, this);
}


/*---------------------------------------------------------------------------*/
Fldtk_Rect_Slot_Editor::~Fldtk_Rect_Slot_Editor()
{
}


/*---------------------------------------------------------------------------*/
void Fldtk_Rect_Slot_Editor::setRect(int t, int l, int b, int r)
{
    char buf[32];
    sprintf(buf, "%d", t); wTop_->value(buf);
    sprintf(buf, "%d", l); wLeft_->value(buf);
    sprintf(buf, "%d", b); wBottom_->value(buf);
    sprintf(buf, "%d", r); wRight_->value(buf);
    update_cb(0L, this);
}


/*---------------------------------------------------------------------------*/
void Fldtk_Rect_Slot_Editor::getRect(int &t, int &l, int &b, int &r)
{
    t = atoi(wTop_->value());
    l = atoi(wLeft_->value());
    b = atoi(wBottom_->value());
    r = atoi(wRight_->value());
}


/*---------------------------------------------------------------------------*/
void Fldtk_Rect_Slot_Editor::update_cb(Fl_Widget*, Fldtk_Rect_Slot_Editor *e)
{
    char buf[100];
    int left = atoi(e->wLeft_->value());
    int right = atoi(e->wRight_->value());
    sprintf(buf, widthStr, right-left);
    e->wWidth_->copy_label(buf);
    int top = atoi(e->wTop_->value());
    int bottom = atoi(e->wBottom_->value());
    sprintf(buf, heightStr, bottom-top);
    e->wHeight_->copy_label(buf);
}


/*---------------------------------------------------------------------------*/
void Fldtk_Rect_Slot_Editor::editor_cb(Fldtk_Rect_Slot_Editor *w, unsigned int cmd)
{
    switch (cmd) {
    case 'aply': w->slot_->apply(); break;
    case 'rvrt': w->slot_->revert(); break;
    }
}

//
// End of "$Id$".
//
