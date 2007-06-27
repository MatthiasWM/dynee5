//
// "$Id$"
//
// Flmm_Signal header file for the FLMM extension to FLTK.
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

#ifndef FLMM_SIGNAL_H
#define FLMM_SIGNAL_H


class Flmm_Slot_Class { };
typedef void (Flmm_Slot_Class::*Flmm_Slot)(void *, void *);

/**
 * Testing.
 */
class Flmm_Signal
{
    class Slot {
        friend class Flmm_Signal;
    public:
        Slot(void *base, Flmm_Slot func, void *user);
        void call(void *caller);
    private:
        Flmm_Slot_Class *base_;
        Flmm_Slot func_;
        void *user_;
    };
public:
    Flmm_Signal();
    ~Flmm_Signal();
    void connect(void *base, Flmm_Slot func, void *user=0L);
    void disconnect(void *base, Flmm_Slot func);
    void operator()(void *caller=0L);
private:
    Slot *rcvr_;
    short nRcvr_, NRcvr_;
};


#endif

//
// End of "$Id$".
//
