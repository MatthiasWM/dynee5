//
// "$Id: Flmp_Image.cxx 4650 2005-11-18 10:08:13Z matt $"
//
// Quicktime player support for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2005 by Bill Spitzak and others.
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
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//

#include "Flmp_Image.H"

#include <FL/Fl_Shared_Image.H>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
extern "C" {
#include <jpeglib.h>
}


char Flmp_Image::save_jpeg(const char *filename, int quality) {
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  cinfo.err = jpeg_std_error((jpeg_error_mgr *)&jerr);
  jpeg_create_compress(&cinfo);
  FILE* outfile;
  outfile = fopen(filename, "wb");
  if (!outfile) return 0;
  jpeg_stdio_dest(&cinfo, outfile);
  cinfo.image_width = w();
  cinfo.image_height = h();
  cinfo.input_components = d();
  if (d()==1)
    cinfo.in_color_space = JCS_GRAYSCALE; /* colorspace of input image */
  else
    cinfo.in_color_space = JCS_RGB; /* colorspace of input image */
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE);
  jpeg_start_compress(&cinfo, TRUE);
  JSAMPROW row_pointer[1];        /* pointer to a single row */
  int row_stride = ld() ? ld() : w()*d();
  unsigned char *dd = (unsigned char*)data()[0];
  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = dd+(cinfo.next_scanline * row_stride);
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  fclose(outfile);
  return 1;
}

Fl_Image *Flmp_Image::rotate90() {
  int sw = w(), sh = h(), sd = d(), i, j;
  uchar *new_array = new uchar[sw*sh*sd];
  Fl_RGB_Image *dst_img = new Fl_RGB_Image(new_array, sh, sw, sd);
  dst_img->alloc_array = 1;
  Flmp_Image *dst = (Flmp_Image*)dst_img;
  for (i=0; i<sw; i++) {
    for (j=0; j<sh; j++) {
      dst->set_pixel(j, i, get_pixel(i, sh-j-1));
    }
  }
  return dst_img;
}

Fl_Image *Flmp_Image::rotate180() {
  int sw = w(), sh = h(), sd = d(), i, j;
  uchar *new_array = new uchar[sw*sh*sd];
  Fl_RGB_Image *dst_img = new Fl_RGB_Image(new_array, sw, sh, sd);
  dst_img->alloc_array = 1;
  Flmp_Image *dst = (Flmp_Image*)dst_img;
  for (i=0; i<sw; i++) {
    for (j=0; j<sh; j++) {
      dst->set_pixel(i, j, get_pixel(sw-i-1, sh-j-1));
    }
  }
  return dst_img;
}

Fl_Image *Flmp_Image::rotate270() {
  int sw = w(), sh = h(), sd = d(), i, j;
  uchar *new_array = new uchar[sw*sh*sd];
  Fl_RGB_Image *dst_img = new Fl_RGB_Image(new_array, sh, sw, sd);
  dst_img->alloc_array = 1;
  Flmp_Image *dst = (Flmp_Image*)dst_img;
  for (i=0; i<sw; i++) {
    for (j=0; j<sh; j++) {
      dst->set_pixel(j, i, get_pixel(sw-i-1, j));
    }
  }
  return dst_img;
}


Fl_Image *Flmp_Image::scale(int W, int H) {
  return copy(W, H);
  if ((W == w() && H == h()) ||
      !w() || !h() || !d() || !array) {
    return copy(W, H);
  }
  if (W <= 0 || H <= 0) {
    return 0;
  }

  Fl_RGB_Image	*new_image;
  uchar		*new_array;
  new_array = new uchar [W * H * d()];
  new_image = new Fl_RGB_Image(new_array, W, H, d());
  new_image->alloc_array = 1;
  
  uchar *dst = new_array;
  int stride = ld() ? ld() : w()*d();

  for (int dy=0; dy<H; dy++) {
    for (int dx=0; dx<W; dx++) {
      const uchar *src = array + dy*stride + dx*d();
      for (int dc=0; dc<d(); dc++) {
        *dst++ = *src++;
      }
    }
  }
  return new_image;
}

Fl_Image *Flmp_Image::scale_to_fit(int nw, int nh) {
  float scl = nw/(float)w();
  if (h()*scl>nh) scl = nh/(float)h();
  return scale((int)(w()*scl+0.5f), (int)(h()*scl+0.5f));
}

Fl_Image *Flmp_Image::scale_and_clip(int nw, int nh) {
  float scl = nw/(float)w();
  if (h()*scl<nh) scl = nh/(float)h();
  int tw = (int)(w()*scl+0.5f), th = (int)(h()*scl+0.5f);
  Fl_Image *tmp = scale(tw, th);
  if (tw==nw && th==nh) return tmp;
  unsigned char *dd = (unsigned char*)(tmp->data()[0]);
  int stride = 0;
  // do we clip on top? Just cut a few rows on top.
  if (nh<th) {
    dd += tw*3*((th-nh)/2);
  }
  // do we need to clip on the sides? Just adjust the stride.
  if (nw<tw) {
    int off = (tw-nw)/2;
    dd += off*3;
    stride = tw*3;
  }
  Fl_Image *tmp2 = new Fl_RGB_Image(dd, nw, nh, 3, stride); // shared the same data array
  Fl_Image *img = tmp2->copy();
  delete tmp;
  delete tmp2;
  return img;
}

void Flmp_Image::compose(Fl_Image *m, int dx, int dy, int dw, int dh, 
                         int sx, int sy, int sw, int sh) {
  Flmp_Image *src = (Flmp_Image*)m;
  int i, j;
  for (i=0; i<dw; i++) {
    for (j=0; j<dh; j++) {
      set_pixel_a(dx+i, dy+j, src->get_pixel(sx+i%sw, sy+j%sh));
    }
  }
  uncache();
}

void Flmp_Image::compose_frame(Fl_Image *m, int x1, int y1, int x2, int y2) {
  Flmp_Image *src = (Flmp_Image*)m;
  int w0 = x1, w1 = x2-x1, w2 = src->w()-x2;
  int h0 = y1, h1 = y2-y1, h2 = src->h()-y2;
  compose(src, 0,      0,      w0, h0, 0, 0, w0, h0); // top left
  compose(src, 0,      h()-h2, w0, h2, 0, y2, w0, h2); // bottom left
  compose(src, w()-w2, 0,      w2, h0, x2, 0, w2, h0); // top right
  compose(src, w()-w2, h()-h2, w2, h2, x2, y2, w2, h2); // bottom right
  compose(src, x1,     0,      w()-w0-w2, h0, x1, 0, w1, h0); // top
  compose(src, x1,     h()-h2, w()-w0-w2, h2, x1, y2, w1, h2); // bottom
  compose(src, 0,      x1,     w0, h()-h0-h2, 0, y1, w0, h1); // left
  compose(src, w()-w2, x1,     w2, h()-h0-h2, x2, y1, w2, h1); // right
}

unsigned int Flmp_Image::get_pixel(int x, int y) {
  if (x<0 || x>=w() || y<0 || y>=h()) return 0;
  unsigned char *dd = (unsigned char*)data()[0];
  int s = ld() ? ld() : w()*d();
  dd += s*y + d()*x;
  if (d()==3) {
    return ( (dd[0]<<24) | (dd[1]<<16) | dd[2]<<8 | 255 );
  } else {
    return ( (dd[0]<<24) | (dd[1]<<16) | dd[2]<<8 | dd[3] );
  }
}

void Flmp_Image::set_pixel(int x, int y, unsigned int color) {
  if (x<0 || x>=w() || y<0 || y>=h()) return;
  unsigned char *dd = (unsigned char*)data()[0];
  int s = ld() ? ld() : w()*d();
  dd += s*y + d()*x;
  dd[0] = color>>24;
  dd[1] = color>>16;
  dd[2] = color>>8;
  if (d()==4) {
    dd[3] = 255;
  }
}

void Flmp_Image::set_pixel_a(int x, int y, unsigned int color) {
  unsigned char a = color & 255;
  if (a==0) return;
  if (a==255) { set_pixel(x, y, color); return; }
  if (x<0 || x>=w() || y<0 || y>=h()) return;
  unsigned int c1 = get_pixel(x, y);
  unsigned char *dd = (unsigned char*)data()[0];
  int s = ld() ? ld() : w()*d();
  dd += s*y + d()*x;
  unsigned char a1 = 255-a;
  dd[0] = (((c1>>24)&0xff)*a1 + ((color>>24)&0xff)*a)>>8;
  dd[1] = (((c1>>16)&0xff)*a1 + ((color>>16)&0xff)*a)>>8;
  dd[2] = (((c1>> 8)&0xff)*a1 + ((color>> 8)&0xff)*a)>>8;
  if (d()==4) {
    dd[3] = 255;
  }
}

const char *Flmp_Image_Target::get_filename() {
  return pFilename;
}

void Flmp_Image_Target::clear_image() {
  delete pImage;
  pImage = 0L;
  delete pScaledImage;
  pScaledImage = 0L;
  redraw();
}

const Fl_Image *Flmp_Image_Target::get_image() {
  return pImage;
}

void Flmp_Image_Target::set_image(const Fl_Image *img) {
  if (!img || !img->d()) {
    clear_image();
    return;
  }
  pImage = (Fl_Image*)(img);
  Flmp_Image *mpi = (Flmp_Image*)img;
  pScaledImage = mpi->scale_to_fit(w()-7, h()-7);
  redraw();
}

char Flmp_Image_Target::load_image(const char *fn) {
  Fl_Image *img = 0L;
  if (fn && *fn) {
    Fl_Shared_Image *simg = Fl_Shared_Image::get(fn);
    img = simg->copy();
    simg->release();
  }
  if (img && img->d()) {
    set_image(img);
    pFilename = strdup(fn);
    return 1;
  } else {
    clear_image();
    if (pFilename) {
      free(pFilename);
      pFilename = 0L;
    }
    delete img;
    return 0; // we should return 1 if the image actually gets cleared
  }
}

static char *dnd_filename(const char *fn) {
  static char *buf = 0L;
  if (!fn) return (char*)"";
  if (buf) free(buf);
  if (strncmp(fn, "file://", 7)==0)
    fn += 7;
  buf = strdup(fn);
  char *d = buf;
  for (;;) {
    unsigned char c = (unsigned char)*d;
    if (c==0) break;
    if (c<' ') break;
    d++;
  }
  *d = 0;
  return buf;
}

int Flmp_Image_Target::handle(int event) {
  char ret;
  switch (event) {
  case FL_DND_ENTER:
  case FL_DND_DRAG:
  case FL_DND_RELEASE:
    return 1;
  case FL_PASTE:
    ret = load_image(dnd_filename(Fl::event_text()));
    if (ret && callback())
      do_callback();
    return 1;
  default:
    break;
  }
  return Fl_Button::handle(event);
}

void Flmp_Image_Target::draw() {
  Fl_Button::draw();
  if (pScaledImage) {
    int xp = (w()-pScaledImage->w())/2;
    int yp = (h()-pScaledImage->h())/2;
    pScaledImage->draw(x()+xp, y()+yp);
  }
}

//
// End of "$Id: Flmp_Image.cxx 4650 2005-11-18 10:08:13Z matt $".
//
