//
// "$Id$"
//
// Nt_Group implementation for the FLMM extension to FLTK.
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

#ifdef WIN32
#pragma warning(disable : 4996)
#endif


#include "Nt_Group.H"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Nt_Group::Nt_Group(int X, int Y, int W, int H, const char *L)
:	Fl_Group(X, Y, W, H, L)
{
	left = X;
	top = Y;
	right = X+W;
	bottom = Y+H;
	justify = 0;
	box(FL_BORDER_BOX);
	color(FL_WHITE);
}

Nt_Group::Nt_Group(int L, int T, int R, int B, int J, const char *lbl)
:	Fl_Group(L, T, R-L, B-T, lbl)
{
	left = L;
	top = T;
	right = R;
	bottom = B;
	justify = J;
	box(FL_BORDER_BOX);
	color(FL_WHITE);
	layout();
}


Nt_Group::~Nt_Group()
{
}


void Nt_Group::layout(Fl_Group *par, Fl_Widget *sib)
{
	if (par==0L)
		par = parent();
	if (sib==0L && par!=0L) {
		int ix = par->find(this);
		if (ix>0 && ix<par->children())
			sib = par->child(ix-1);
		else
			sib = par->child(par->children()-1);
	}

	int d, xx = left, yy = top, ww = right-left, hh = bottom-top;


	printf("Layout: 0x%08x %9d, %4d %4d %4d %4d \"%s\"\n", justify, justify, left, top, right, bottom, label());
	if (justify & 0x0f) {
		d = 3; // not yet supported?
	}

	if (!sib) sib = par;
	int sx = sib->x(), sy = sib->y(), sw = sib->w(), sh = sib->h();
	int px = par->x(), py = par->y(), pw = par->w(), ph = par->h();

	if (par) {
		if (!par) {
			d = 4; // error!
		}
		switch (justify&48) {
		case 0: // align left
			xx = px + left;
			ww = right - left;
			break;
		case 16: // center
			ww = right-left;
			xx = px + 0.5*(pw-ww) + left;
			break;
		case 32: // right
			xx = px + pw + left;
			ww = right - left;
			break;
		case 48: // inset
			xx = px + left;
			ww = pw + right;
			break;
		}
		switch (justify&192) {
		case 0: // align left
			yy = py + top;
			hh = bottom - top;
			break;
		case 64: // center
			hh = bottom-top;
			yy = py + 0.5*(ph-hh) + top;
			break;
		case 128: // right
			yy = py + ph + top;
			hh = bottom - top;
			break;
		case 192: // inset
			//wt = w->t()+wt;
			//wb = w->b()+wb;
			yy = py + top;
			hh = ph + bottom;
			break;
		}
	}
	if (justify & 1536) {
		switch (justify & 1536) {
		case 1536: // left rel to left, right rel to right
			xx = sx + left;
			ww = sx + sw + right - xx;
			break;
		}
	}
	if (justify & 12288) {
		switch (justify & 12288) {
		case 8192: // relative to siblings bottom
			yy = sy + sh + top;
			hh = bottom - top;
			break;
		case 12288: // top rel to top and btm rel to btm
			yy = sy + top;
			hh = sy + sh + bottom - yy;
			break;
		}
	}
	resize(xx, yy, ww, hh);
}

//
// End of "$Id$".
//
