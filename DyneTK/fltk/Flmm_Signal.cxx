//
// "$Id$"
//
// Flmm_Signal implementation for the FLMM extension to FLTK.
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
// Please report all bugs and problems to "flmm@matthiasm.com".
//

#include "Flmm_Signal.h"

#include <stdlib.h>
#include <string.h>

/*
void Flmm_Signal::call(Nix *k, slot_fn f)
{
    (k->*f)(0L, 0L);
}

void Flmm_Signal::call2(void *k, slot_fn f)
{
    //(k->*f)(0L, 0L);
}

void Flmm_Signal::call3(void *k, Flmm_Slot f)
{
    Nix *n = (Nix*)k;
    (n->*f)(0L, 0L);
}
*/


/*---------------------------------------------------------------------------*/
Flmm_Signal::Slot::Slot(void *base, Flmm_Slot func, void *user)
:   base_((Flmm_Slot_Class*)base),
    func_(func),
    user_(user)
{
}


/*---------------------------------------------------------------------------*/
void Flmm_Signal::Slot::call(void *caller)
{
    (base_->*func_)(caller, user_);
}


/*---------------------------------------------------------------------------*/
Flmm_Signal::Flmm_Signal()
:   rcvr_(0L),
    nRcvr_(0),
    NRcvr_(0)
{
}


/*---------------------------------------------------------------------------*/
Flmm_Signal::~Flmm_Signal()
{
    if (rcvr_)
        free(rcvr_);
}


/*---------------------------------------------------------------------------*/
void Flmm_Signal::connect(void *base, Flmm_Slot func, void *user)
{
    if (!rcvr_) {
        NRcvr_ = 2;
        rcvr_ = (Slot*)malloc(sizeof(Slot)*NRcvr_);
    } else if (nRcvr_==NRcvr_) {
        NRcvr_ *= 2;
        rcvr_ = (Slot*)realloc(rcvr_, sizeof(Slot)*NRcvr_);
    }
    Slot &s = rcvr_[nRcvr_++];
    s.base_ = (Flmm_Slot_Class*)base;
    s.func_ = func;
    s.user_ = user;
}


/*---------------------------------------------------------------------------*/
void Flmm_Signal::disconnect(void *base, Flmm_Slot func)
{
    int i;
    for (i=0; i<nRcvr_; i++) {
        Slot &s = rcvr_[i];
        if (s.base_==(Flmm_Slot_Class*)base && s.func_==func) {
            memmove(rcvr_+i, rcvr_+i+1, (nRcvr_-i-1)*sizeof(Slot));
            break;
        }
    }
    if (nRcvr_==0 && rcvr_) {
        free(rcvr_);
        NRcvr_ = 0;
        rcvr_ = 0L;
    }
}


/*---------------------------------------------------------------------------*/
void Flmm_Signal::operator()(void *caller)
{
    int i;
    for (i=0; i<nRcvr_; i++) {
        rcvr_[i].call(caller);
    }
}


//
// End of "$Id$".
//
