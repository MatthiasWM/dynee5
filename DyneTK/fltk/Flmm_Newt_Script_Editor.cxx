//
// "$Id$"
//
// Flmm_Newt_Script_Editor implementation for the FLMM extension to FLTK.
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


#include "Flmm_Newt_Script_Editor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifdef WIN32
# define strcasecmp stricmp
#endif

// FIXME add highlighting of complex symbols like |sym:bol|


static Fl_Text_Display::Style_Table_Entry
styletable[] = {
  { FL_BLACK,	        FL_COURIER,		    13 }, // A - Plain
  { FL_DARK_GREEN,    FL_COURIER_ITALIC,  13 }, // B - Line comments
  { FL_DARK_GREEN,    FL_COURIER_ITALIC,  13 }, // C - Block comments
  { FL_BLUE, 	        FL_COURIER,		    13 }, // D - Strings
  { FL_DARK_RED,      FL_COURIER,		    13 }, // E - Directives
  { FL_DARK_RED,      FL_COURIER_BOLD,	13 }, // F - Types
  { FL_BLUE, 	        FL_COURIER_BOLD,	13 }  // G - Keywords
};

const char *code_keywords[] = { // List of known Newt Script keywords...
  "begin",
  "break",
  "by",
  "call",
  "collect",
  "deeply",
  "do",
  "else",
  "end",
  "exists",
  "for",
  "foreach",
  "func",
  "if",
  "in",
  "loop",
  "native",
  "onexception",
  "repeat",
  "return",
  "then",
  "to",
  "try",
  "until",
  "while",
  "with",
};
const char *code_types[] = {
  "and",
  "array",
  "constant",
  "div",
  "global",
  "inherited",
  "int",
  "local",
  "mod",
  "nil",
  "not",
  "or",
  "self",
};
// "_parent", "_proto"
// Builtin functions!

//
// 'compare_keywords()' - Compare two keywords...
//

extern "C" {
  int compare_keywords(const void *a, const void *b) {
    return (strcasecmp(*((const char **)a), *((const char **)b)));
  }
}

static void
style_parse(const char *text,
            char	   *style,
            int 	   length) {
  char    current;
  int		col;
  int		last;
  char	buf[255], *bufptr;
  const char *temp;
  
  // Style letters:
  //
  // A - Plain
  // B - Line comments
  // C - Block comments
  // D - Strings
  // E - Symbol
  // F - Types
  // G - Keywords
  
  for (current = *style, col = 0, last = 0; length > 0; length --, text ++) {
    if (current == 'B' || current == 'F' || current == 'G') current = 'A';
    if (current == 'A') {
      // Check for directives, comments, strings, and keywords...
      if (col == 0 && *text == '#') {
        // Set style to directive
        current = 'E';
      } else if (strncmp(text, "//", 2) == 0) {
        current = 'B';
        for (; length > 0 && *text != '\n'; length --, text ++) *style++ = 'B';
        
        if (length == 0) break;
      } else if (strncmp(text, "/*", 2) == 0) {
        current = 'C';
      } else if (strncmp(text, "\\\"", 2) == 0) {
        // Quoted quote...
        *style++ = current;
        *style++ = current;
        text ++;
        length --;
        col += 2;
        continue;
      } else if (*text == '\"') {
        current = 'D';
      } else if (!last && (isalpha(*text) || *text == '_')) {
        // Might be a keyword...
        for (temp = text, bufptr = buf;
             (isalpha(*temp) || *temp == '_') && bufptr < (buf + sizeof(buf) - 1);
             *bufptr++ = *temp++);
        
        if (!isalpha(*temp) && *temp != '_') {
          *bufptr = '\0';
          
          bufptr = buf;
          
          if (bsearch(&bufptr, code_types,
                      sizeof(code_types) / sizeof(code_types[0]),
                      sizeof(code_types[0]), compare_keywords)) {
            while (text < temp) {
              *style++ = 'F';
              text ++;
              length --;
              col ++;
            }
            
            text --;
            length ++;
            last = 1;
            continue;
          } else if (bsearch(&bufptr, code_keywords,
                             sizeof(code_keywords) / sizeof(code_keywords[0]),
                             sizeof(code_keywords[0]), compare_keywords)) {
            while (text < temp) {
              *style++ = 'G';
              text ++;
              length --;
              col ++;
            }
            
            text --;
            length ++;
            last = 1;
            continue;
          }
        }
      } else if (!last && *text == '\'') {
        // might be a symbol...
        if (isalpha(text[1]) || text[1]=='_') {
          // it *is* a symbol
          *style++ = 'E';
          text++;
          length--;
          col++;
          while (isalnum(*text)||*text == '_') {
            *style++ = 'E';
            text++;
            length--;
            col++;
          }
          text --;
          length ++;
          last = 1;
          continue;
        } else if (text[1]=='|') {
          // it is a complex symbol
          *style++ = 'E'; text++; length--; col++;
          *style++ = 'E'; text++; length--; col++;
          while (((*text>=32)&&(*text<127)&&(*text!='|'))||((text[0]=='\\')&&(text[1]=='|'))) {
            if (*text=='\\') {
              *style++ = 'E'; text++; length--; col++;
            }
            *style++ = 'E'; text++; length--; col++;
          }
          *style++ = 'E'; 
          col++;
          last = 1;
          continue;
        }
      }
    } else if (current == 'C' && strncmp(text, "*/", 2) == 0) {
      // Close a C comment...
      *style++ = current;
      *style++ = current;
      text ++;
      length --;
      current = 'A';
      col += 2;
      continue;
    } else if (current == 'D') {
      // Continuing in string...
      if (strncmp(text, "\\\"", 2) == 0) {
        // Quoted end quote...
        *style++ = current;
        *style++ = current;
        text ++;
        length --;
        col += 2;
        continue;
      } else if (*text == '\"') {
        // End quote...
        *style++ = current;
        col ++;
        current = 'A';
        continue;
      }
    }
    
    // Copy style info...
    if (current == 'A' && (*text == '{' || *text == '}')) *style++ = 'G';
    else *style++ = current;
    col ++;
    
    last = isalnum(*text) || *text == '_' || *text == '.';
    
    if (*text == '\n') {
      // Reset column and possibly reset the style
      col = 0;
      if (current == 'B' || current == 'E') current = 'A';
    }
  }
}


static void style_unfinished_cb(int, void*) 
{
  // blank
}



Flmm_Newt_Script_Editor::Flmm_Newt_Script_Editor(int xx, int yy, int ww, int hh)
:	Fl_Text_Editor(xx, yy, ww, hh),
loading(0),
changed(1)
{
  textbuf = new Fl_Text_Buffer();
  stylebuf = new Fl_Text_Buffer();
  buffer(textbuf);
  highlight_data(stylebuf, styletable,
                 sizeof(styletable) / sizeof(styletable[0]),
                 'A', style_unfinished_cb, this);
  textfont(FL_COURIER);
  textsize(12);
  textbuf->add_modify_callback(style_update_, this);
  textbuf->add_modify_callback(changed_cb_, this);
  textbuf->call_modify_callbacks();
}

void Flmm_Newt_Script_Editor::style_update_(
                                            int pos, int nInserted,
                                            int nDeleted, int,	const char *, void *cbArg)
{
  Flmm_Newt_Script_Editor *e = (Flmm_Newt_Script_Editor*)cbArg;
  e->style_update(pos, nInserted, nDeleted);
}

void Flmm_Newt_Script_Editor::style_update(
                                           int pos, int nInserted, int nDeleted)
{
  if (nInserted == 0 && nDeleted == 0) {
    stylebuf->unselect();
    return;
  }
  // Track changes in the text buffer...
  if (nInserted > 0) {
    // Insert characters into the style buffer...
    char *style = new char[nInserted + 1];
    memset(style, 'A', nInserted);
    style[nInserted] = '\0';
    
    stylebuf->replace(pos, pos + nDeleted, style);
    delete[] style;
  } else {
    // Just delete characters in the style buffer...
    stylebuf->remove(pos, pos + nDeleted);
  }
  // Select the area that was just updated to avoid unnecessary
  // callbacks...
  stylebuf->select(pos, pos + nInserted - nDeleted);
  
  int start, end;
  char last, *style, *text;
  start = textbuf->line_start(pos);
  //	if (start > 0) start = textbuf->line_start(start - 1);
  end	= textbuf->line_end(pos + nInserted);
  text	= textbuf->text_range(start, end);
  style = stylebuf->text_range(start, end);
  if (start==end)
    last = 0;
  else
    last  = style[end - start - 1];
  
  //	printf("start = %d, end = %d, text = \"%s\", style = \"%s\", last='%c'...\n",
  //		   start, end, text, style, last);
  
  style_parse(text, style, end - start);
  
  //	printf("new style = \"%s\", new last='%c'...\n", 
  //		   style, style[end - start - 1]);
  
  stylebuf->replace(start, end, style);
  redisplay_range(start, end);
  
  if (start==end || last != style[end - start - 1]) {
    //	  printf("Recalculate the rest of the buffer style\n");
    // Either the user deleted some text, or the last character 
    // on the line changed styles, so reparse the
    // remainder of the buffer...
    free(text);
    free(style);
    
    end   = textbuf->length();
    text  = textbuf->text_range(start, end);
    style = stylebuf->text_range(start, end);
    
    style_parse(text, style, end - start);
    
    stylebuf->replace(start, end, style);
    redisplay_range(start, end);
  }
  
  free(text);
  free(style);
  
}

void Flmm_Newt_Script_Editor::changed_cb_(int, int nInserted, int nDeleted,int, const char*, void* v)
{
  Flmm_Newt_Script_Editor *e = (Flmm_Newt_Script_Editor*)v;
  e->changed_cb(nInserted, nDeleted);
}

void Flmm_Newt_Script_Editor::changed_cb(int nInserted, int nDeleted)
{
  if ((nInserted || nDeleted) && !loading) 
    changed = 1;
  if (loading) show_insert_position();
}


//
// End of "$Id$".
//
