//
// "$Id$"
//
// Dtk_Platform implementation for the Dyne Toolkit.
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

#ifdef WIN32
#pragma warning(disable : 4786)
#endif

#include "Dtk_Platform.h"
#include "Dtk_Template.h"
#include "Dtk_Script_Writer.h"
#include "Dtk_Error.h"

#include "globals.h"
#include "allNewt.h"

#include <FL/fl_ask.h>
#include <FL/Fl_Menu_Item.H>

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

extern  const char *platformStr1;
extern  const char *platformStr2;
extern  const char *platformStr3;
extern  const char *platformStr4;


/*---------------------------------------------------------------------------*/
bool Dtk_Platform::CStringSort::operator()(const char *a, const char *b) const
{
  return (strcmp(a, b)>0);
}

/*---------------------------------------------------------------------------*/
Dtk_Platform::Dtk_Platform(const char *ptfFilename, const char *constFilename)
: platform_(kNewtRefUnbind),
  templateChoiceMenu_(0L),
  methodsChoiceMenu_(0L),
  attributesChoiceMenu_(0L),
  ptfFilename_(strdup(ptfFilename)),
  constFilename_(strdup(constFilename))
{
  loadPtfFile();
  //loadConstFile(); // do this at compile time
}


/*---------------------------------------------------------------------------*/
Dtk_Platform::~Dtk_Platform()
{
  delete templateChoiceMenu_; // this should also delete all allocated menu labels
  delete methodsChoiceMenu_;
  delete attributesChoiceMenu_;
  // the menus in the map are not deleted
}


/*---------------------------------------------------------------------------*/
/**
 * Load the Platform support file into the internal compiler.
 * 
 * The Platform files are part of the original NTK. They can be downloaded from
 * Unna here: "http://www.unna.org/view.php?/apple/development/NTK/platformfiles/21PTF.ZIP"
 *
 * The archive contains two files. The first file, NEWTON21.PTF, conatins a description
 * of the platform interface int NSOF format and is required by the DyneTK layout editor.
 *
 * The second file, 21DEFS.TXT, conatins a list of all constants that may be used in 
 * Newton Script files. It is included into the "big script" before compilation.
 */
int Dtk_Platform::loadPtfFile()
{
  const char *filename = ptfFilename_;
#if 1
  // try to load the platform file:
  struct stat st;
  if (stat(filename, &st)) {
    fl_alert(
             "Platform file \"%s\" not found.\n\n"
             "This file is required to edit DyneTK projects. It must be placed in the DyneTK start directory.\n"
             "\"%s\" is part of the MSWindow NTK and should be available on Unna:\n\n"
             "http://www.unna.org/view.php?/apple/development/NTK/winntk/1.6b10/NTK.ZIP",
             filename, filename);
    return -1;
  }
  // load the file
  FILE *f = fopen(filename, "rb");
	uint8_t *pf = (uint8_t*)malloc(st.st_size);
  fread(pf, 1, st.st_size, f);
  fclose(f);
  // interprete the NSOF stream
  platform_ = NewtReadNSOF(pf, st.st_size);
  
#else
  // assemble a long string from a few short strings. VisualC6 can not deal with 
  // very long stringas and ends up in a buffer overflow (it is graceful enough
  // to give us an error description, even with a recomendation to increase heap size,
  // which does not fix the problem though)
  int len = strlen(platformStr1) + strlen(platformStr2) + strlen(platformStr3) + strlen(platformStr4);
  char *platformStr = (char*)malloc(len+10);
  strcpy(platformStr, platformStr1);
  strcat(platformStr, platformStr2);
  strcat(platformStr, platformStr3);
  strcat(platformStr, platformStr4);
  
  newtErr	err;
  NEWT_DUMPBC = 0;
  platform_ = NVMInterpretStr(platformStr, &err);
  /*
   NcSetGlobalVar(NSSYM(printLength), NSINT(9999));
   NcSetGlobalVar(NSSYM(printDepth), NSINT(8));
   NEWT_DUMPBC = 0;
   NEWT_INDENT = -2;
   FILE *ff = fopen("platform2.txt", "wb");
   NewtPrintObject(ff, platform_);
   fclose(ff);
   */
  free(platformStr);
#endif
  
  if (platform_==kNewtRefUnbind) {
    fl_alert("Error reading platform file.");
    return -1;
  } else {
    NcSetGlobalVar(NSSYM(DyneTK_Platform), platform_);
    return 0;
  }
}


/*---------------------------------------------------------------------------*/
int Dtk_Platform::loadConstFile()
{
  // read it back into memory
  struct stat st;
  stat(constFilename_, &st);
  FILE *f = fopen(constFilename_, "rb");
	char *script = (char*)malloc(st.st_size);
  fread(script, 1, st.st_size, f);
  fclose(f);
  // compile and run ir
  newtErr	err;
  NVMInterpretStr(script, &err);
  // release the memory taken by the script
  free(script);  
  // handle any error
  if (err) {
    InspectorPrintf("**** Reading Platform Constants: %s: %s\n", newt_error_class(err), newt_error(err));
    return -1;
  }
  return 0;
}


/*---------------------------------------------------------------------------*/
void Dtk_Platform::writeConstants(Dtk_Script_Writer &sw)
{
  // read it back into memory
  struct stat st;
  stat(constFilename_, &st);
  FILE *f = fopen(constFilename_, "rb");
	char *script = (char*)malloc(st.st_size);
  fread(script, 1, st.st_size, f);
  fclose(f);
  sw.put(script, st.st_size);
  free(script);
}


/*---------------------------------------------------------------------------*/
Fl_Menu_Item *Dtk_Platform::templateChoiceMenu()
{
  if (templateChoiceMenu_)
    return templateChoiceMenu_;
  if (platform_==kNewtRefUnbind)
    return 0L;
  
  newtRef ta = NewtGetArraySlot(platform_, NewtFindSlotIndex(platform_, NSSYM(TemplateArray)));
  int nta = NewtRefIsArray(ta) ? NewtArrayLength(ta) : 0;
  newtRef va = NewtGetArraySlot(platform_, NewtFindSlotIndex(platform_, NSSYM(ViewClassArray)));
  int nva = NewtRefIsArray(va) ? NewtArrayLength(va) : 0;
  
  // create the choice of templates by finding all symbols inside the arrays. 
  int mi = 0, i;
  templateChoiceMenu_ = (Fl_Menu_Item*)calloc(nta+nva+1, sizeof(Fl_Menu_Item));
  for (i=0; i<nta; i++) {
    newtRef sym = NewtSlotsGetSlot(ta, i);
    if (NewtRefIsSymbol(sym)) {
      templateChoiceMenu_[mi].label(strdup(NewtSymbolGetName(sym)));
      newtRef tmplDB = NewtGetFrameSlot(ta, i+1);
      newtRef proto = NewtGetFrameSlot(tmplDB, NewtFindSlotIndex(tmplDB, NSSYM(_proto)));
      templateChoiceMenu_[mi].user_data((void*)NewtRefToInteger(proto));
      mi++; i++;
    }
  }
  for (i=0; i<nva; i++) {
    newtRef sym = NewtSlotsGetSlot(va, i);
    if (NewtRefIsSymbol(sym)) {
      templateChoiceMenu_[mi].label(strdup(NewtSymbolGetName(sym)));
      newtRef tmplDB = NewtGetFrameSlot(va, i+1);
      newtRef proto = NewtGetFrameSlot(tmplDB, NewtFindSlotIndex(tmplDB, NSSYM(_proto)));
      if (NewtRefIsInteger(proto))
        templateChoiceMenu_[mi].user_data((void*)NewtRefToInteger(proto));
      mi++; i++;
    }
  }
  templateChoiceMenu_ = (Fl_Menu_Item*)realloc(templateChoiceMenu_, (mi+1) * sizeof(Fl_Menu_Item));
  
  sort(templateChoiceMenu_);
  
  return templateChoiceMenu_;
}


/*---------------------------------------------------------------------------*/
Fl_Menu_Item *Dtk_Platform::specificChoiceMenu(Dtk_Template *tmpl)
{
  if (platform_==kNewtRefUnbind)
    return 0L;
  if (!tmpl)
    return 0L;
  char *id = tmpl->id();
  if (!id)
    return 0L;
  
  Fl_Menu_Item *menu;
  // have we previously created this menu?
  std::map<char*,Fl_Menu_Item*,CStringSort>::iterator it = specificMenuMap_.find(id);
  if (it == specificMenuMap_.end()) {
    // find the id in the platform database
    newtRef ta = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(TemplateArray)));
    int ix = NewtFindArrayIndex(ta, NewtMakeSymbol(id), 0);
    if (ix==-1) {
      ta = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(ViewClassArray)));
      ix = NewtFindArrayIndex(ta, NewtMakeSymbol(id), 0);
    }
    newtRef tmplDB = NewtGetFrameSlot(ta, ix+1);
    newtRef opt = NewtGetFrameSlot(tmplDB, NewtFindSlotIndex(tmplDB, NSSYM(__ntOptional)));
    newtRef rqd = NewtGetFrameSlot(tmplDB, NewtFindSlotIndex(tmplDB, NSSYM(__ntRequired)));
    int i, mi = 0;
    int no = NewtRefIsFrame(opt) ? NewtFrameLength(opt) : 0;
    int nr = NewtRefIsFrame(rqd) ? NewtFrameLength(rqd) : 0;
    
    menu = (Fl_Menu_Item*)calloc(no+nr+1, sizeof(Fl_Menu_Item));
    for (i=0; i<no; i++, mi++) {
      newtRef sym = NewtGetFrameKey(opt, i);
      menu[mi].label(strdup(NewtSymbolGetName(sym)));
    }
    if (no) {
      sort(menu, no);
      menu[mi-1].flags |= FL_MENU_DIVIDER;
    }
    for (i=0; i<nr; i++, mi++) {
      newtRef sym = NewtGetFrameKey(rqd, i);
      menu[mi].label(strdup(NewtSymbolGetName(sym)));
    }
    if (nr)
      sort(menu+no, nr);
    specificMenuMap_.insert(std::make_pair(strdup(id), menu));
  } else {
    menu = it->second;
  }
  return menu;
}


/*---------------------------------------------------------------------------*/
Fl_Menu_Item *Dtk_Platform::methodsChoiceMenu()
{
  if (methodsChoiceMenu_)
    return methodsChoiceMenu_;
  if (platform_==kNewtRefUnbind)
    return 0L;
  
  newtRef ta = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(ScriptSlots)));
  if (ta==kNewtRefUnbind)
    return 0L;
  
  // create the choice of methods by finding all symbols inside the MethodsArray. 
  int i, n = NewtFrameLength(ta);
  methodsChoiceMenu_ = (Fl_Menu_Item*)calloc(n+1, sizeof(Fl_Menu_Item));
  for (i=0; i<n; i++) {
    newtRef sym = NewtGetFrameKey(ta, i);
    methodsChoiceMenu_[i].label(strdup(NewtSymbolGetName(sym)));
  }
  
  sort(methodsChoiceMenu_);
  
  return methodsChoiceMenu_;
}


/*---------------------------------------------------------------------------*/
Fl_Menu_Item *Dtk_Platform::attributesChoiceMenu()
{
  if (attributesChoiceMenu_)
    return attributesChoiceMenu_;
  if (platform_==kNewtRefUnbind)
    return 0L;
  
  newtRef ta = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(AttributeSlots)));
  if (ta==kNewtRefUnbind)
    return 0L;
  
  // create the choice of attributes by finding all symbols inside the AttributesArray. 
  int i, n = NewtFrameLength(ta);
  attributesChoiceMenu_ = (Fl_Menu_Item*)calloc(n+1, sizeof(Fl_Menu_Item));
  for (i=0; i<n; i++) {
    newtRef sym = NewtGetFrameKey(ta, i);
    attributesChoiceMenu_[i].label(strdup(NewtSymbolGetName(sym)));
  }
  
  sort(attributesChoiceMenu_);
  
  return attributesChoiceMenu_;
}


/*---------------------------------------------------------------------------*/
void Dtk_Platform::updateActivation(Fl_Menu_Item *menu, Dtk_Template *tmpl)
{
  for (;;) {
    const char *id = menu->label();
    if (!id) 
      break;
    if (tmpl->findSlot(id)) {
      menu->flags |= FL_MENU_INACTIVE;
    } else {
      menu->flags &= ~FL_MENU_INACTIVE;
    }
    ++menu;
  }
}


/*---------------------------------------------------------------------------*/
static int cStringCompare(const void *a, const void *b) 
{
  return strcasecmp(((Fl_Menu_Item*)a)->label(), ((Fl_Menu_Item*)b)->label());
}


/*---------------------------------------------------------------------------*/
void Dtk_Platform::sort(Fl_Menu_Item *menu, int n)
{
  if (n==-1) {
    Fl_Menu_Item *m = menu;
    while (m->label()) m++;
    n = m - menu;
  }
  qsort(menu, n, sizeof(Fl_Menu_Item), cStringCompare);
}


/*---------------------------------------------------------------------------*/
newtRef Dtk_Platform::newtTemplate(char *id)
{
  if (platform_==kNewtRefUnbind)
    return kNewtRefUnbind;
  if (!id)
    return kNewtRefUnbind;
  newtRef idRef = NewtMakeSymbol(id);
  
  // find the id in the platform database
  newtRef ta = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(TemplateArray)));
  int ix = NewtFindArrayIndex(ta, idRef, 0);
  if (ix==-1) {
    ta = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(ViewClassArray)));
    ix = NewtFindArrayIndex(ta, idRef, 0);
  }
  if (ix==-1)
    return kNewtRefUnbind;
  
  newtRef tmplDB = NewtGetFrameSlot(ta, ix+1);
  newtRef rqd = NewtGetFrameSlot(tmplDB, NewtFindSlotIndex(tmplDB, NSSYM(__ntRequired)));
  int i, vi = 0, ai = 0, as = 0;
  int nr = NewtRefIsFrame(rqd) ? NewtFrameLength(rqd) : 0;
  
  newtRefVar attr[100];    
  newtRefVar val[100];
  int p = findProto(id);
  if (p!=-1) {
    attr[ai++] = NSSYM(value);
    attr[ai++] = NewtMakeInt30(p);
    attr[ai++] = NSSYM(__ntDatatype);
    attr[ai++] = NewtMakeString("PROT", true);
    val[vi++] = NSSYM(__ntTemplate);
    val[vi++] = NewtMakeFrame2(2, attr+ai-4);
  }
  for (i=0; i<nr; i++) {
    newtRef sym = NewtGetFrameKey(rqd, i);
//    int def = NewtFindSlotIndex(tmplDB, sym);
    as = ai;
    attr[ai++] = NSSYM(value);
//  attr[ai++] = NewtGetFrameSlot(tmplDB, def);
    attr[ai++] = getSlotDefaultValue(idRef, sym);
    attr[ai++] = NSSYM(__ntDatatype);
    attr[ai++] = NewtGetFrameSlot(rqd, i);
    val[vi++] = sym;
    val[vi++] = NewtMakeFrame2((ai-as)/2, attr+as);
    //menu[mi].label(strdup(NewtSymbolGetName(sym)));
  }
  
  newtRef value = NewtMakeFrame2(vi/2, val);
  
  newtRefVar ret[] = { NSSYM(value), value, NSSYM(__ntId), NewtMakeSymbol(id) };
  newtRef result = NewtMakeFrame2(2, ret);
  //NewtPrintObject(stdout, result);
  return result;
}


/*---------------------------------------------------------------------------*/
newtRef Dtk_Platform::getSlotDefaultValue(newtRefArg id, newtRefArg key)
{
  // abort if no database loaded
  if (platform_==kNewtRefUnbind)
    return kNewtRefUnbind;

  if (NewtRefIsSymbol(id)) {
    // if an id is given search the TemplateArray and the ViewClassArray
    newtRef ta = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(TemplateArray)));
    int ix = NewtFindArrayIndex(ta, id, 0);
    if (ix==-1) {
      ta = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(ViewClassArray)));
      ix = NewtFindArrayIndex(ta, id, 0);
    }
    if (ix>=0) {
      newtRef tmplDB = NewtGetFrameSlot(ta, ix+1);
      if (NewtRefIsFrame(tmplDB)) {
        newtRef value = NewtGetFrameSlot(tmplDB, NewtFindSlotIndex(tmplDB, key));
        if (value!=kNewtRefUnbind)
          return value;
        // recurse into __ntAncestor and finally into the Script and Attribute Slots
        newtRef ancestor = NewtGetFrameSlot(tmplDB, NewtFindSlotIndex(tmplDB, NSSYM(__ntAncestor)));
        return getSlotDefaultValue(ancestor, key); // it's ok if there is no ancestor!
      }
    }
  }
  // if no id was given or no entry was found, then search ScriptSlots and AttributeSlots
  newtRef db = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(ScriptSlots)));
  if (NewtRefIsFrame(db)) {
    newtRef def = NewtGetFrameSlot(db, NewtFindSlotIndex(db, key));
    if (NewtRefIsFrame(def)) {
      newtRef value = NewtGetFrameSlot(def, NewtFindSlotIndex(def, NSSYM(Value)));
      if (value!=kNewtRefUnbind)
        return value;
    }
  }
  db = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(AttributeSlots)));
  if (NewtRefIsFrame(db)) {
    newtRef def = NewtGetFrameSlot(db, NewtFindSlotIndex(db, key));
    if (NewtRefIsFrame(def)) {
      newtRef value = NewtGetFrameSlot(def, NewtFindSlotIndex(def, NSSYM(Value)));
      if (value!=kNewtRefUnbind)
        return value;
    }
  }
  return kNewtRefUnbind;
}

/*---------------------------------------------------------------------------*/
newtRef Dtk_Platform::getSpecificSlotDescription(Dtk_Template *tmpl, newtRefArg key)
{
  // abort if no database loaded
  if (platform_==kNewtRefUnbind)
    return kNewtRefUnbind;
  
  // abort if we have no id
  char *idStr = tmpl->id();
  if (!idStr)
    return kNewtRefUnbind;
  newtRef id = NewtMakeSymbol(idStr);
  
  // find the default settings for this slot in the given template
  newtRef ta = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(TemplateArray)));
  int ix = NewtFindArrayIndex(ta, id, 0);
  if (ix==-1) {
    ta = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(ViewClassArray)));
    ix = NewtFindArrayIndex(ta, id, 0);
  }
  if (ix==-1)
    return kNewtRefUnbind;
  
  newtRef tmplDB = NewtGetFrameSlot(ta, ix+1);
  if (!NewtRefIsFrame(tmplDB))
    return kNewtRefUnbind;
  /// \todo Also search __ntAncestor
  
  // now search for the type in the __ntRequired section
  newtRef type = kNewtRefUnbind;
  newtRef sect = NewtGetFrameSlot(tmplDB, NewtFindSlotIndex(tmplDB, NSSYM(__ntRequired)));
  if (NewtRefIsFrame(sect))
    type = NewtGetFrameSlot(sect, NewtFindSlotIndex(sect, key));

  // if we still don't know the type, try __ntOptional frame
  sect = NewtGetFrameSlot(tmplDB, NewtFindSlotIndex(tmplDB, NSSYM(__ntOptional)));
  if (NewtRefIsFrame(sect))
    type = NewtGetFrameSlot(sect, NewtFindSlotIndex(sect, key));
  
  // if we did not find the key at all, then this key is not part of the specific slots. Abort.
  if (!NewtRefIsString(type))
    return kNewtRefUnbind;

  // find the default value for this key
  newtRef value = getSlotDefaultValue(id, key);
  /// \todo Do something smart if no default value was found
    
  newtRef tmp[] = { 
    NSSYM(Value),         value, 
    NSSYM(__ntDatatype),  type
  };
  newtRef ret = NewtMakeFrame2(2, tmp);
  
  return ret;
}

/*---------------------------------------------------------------------------*/
newtRef Dtk_Platform::getScriptSlotDescription(newtRefArg key)
{
  // abort if no platform file
  if (platform_==kNewtRefUnbind)
    return kNewtRefUnbind;
  
  // get the script slot database
  newtRef db = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(ScriptSlots)));
  if (!NewtRefIsFrame(db))
    return kNewtRefUnbind;
  
  // find the description for this key
  newtRef def = NewtGetFrameSlot(db, NewtFindSlotIndex(db, key));
  if (!NewtRefIsFrame(def))
    return kNewtRefUnbind;

  // find the default value for this slot
  newtRef value = NewtGetFrameSlot(def, NewtFindSlotIndex(def, NSSYM(Value)));
    
  newtRef tmp[] = { 
    NSSYM(Value),         value, 
    NSSYM(__ntDatatype),  NewtMakeString("SCPT", true) 
  };
  newtRef ret = NewtMakeFrame2(2, tmp);
  
  return ret;
}


/*---------------------------------------------------------------------------*/
newtRef Dtk_Platform::getAttributesSlotDescription(newtRefArg key)
{
  // abort if no platform file
  if (platform_==kNewtRefUnbind)
    return 0L;
  
  // get the attribute slot database
  newtRef db = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(AttributeSlots)));
  if (!NewtRefIsFrame(db))
    return kNewtRefUnbind;

  // find the description for this key
  newtRef def = NewtGetFrameSlot(db, NewtFindSlotIndex(db, key));
  if (!NewtRefIsFrame(def))
    return kNewtRefUnbind;

  return def;
}


/*---------------------------------------------------------------------------*/
int Dtk_Platform::findProto(const char *id)
{
  const Fl_Menu_Item *mi = templateChoiceMenu();
  for (;;++mi) {
    const char *t = mi->label();
    if (!t) {
      return -1;
    }
    if (strcasecmp(t, id)==0) {
      return (int)mi->user_data();
    }
  }
}


/*---------------------------------------------------------------------------*/
const char *Dtk_Platform::getHelp(newtRefArg id, newtRefArg key)
{
  // abort if no database loaded
  if (platform_==kNewtRefUnbind)
    return 0L;
  
  if (NewtRefIsSymbol(id)) {
    // if an id is given search the TemplateArray and the ViewClassArray
    newtRef ta = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(TemplateArray)));
    int ix = NewtFindArrayIndex(ta, id, 0);
    if (ix==-1) {
      ta = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(ViewClassArray)));
      ix = NewtFindArrayIndex(ta, id, 0);
    }
    if (ix>=0) {
      newtRef tmplDB = NewtGetFrameSlot(ta, ix+1);
      if (NewtRefIsFrame(tmplDB)) {
        newtRef help = NewtGetFrameSlot(tmplDB, NewtFindSlotIndex(tmplDB, NSSYM(__ntHelp)));
        if (NewtRefIsFrame(help)) {
          newtRef value = NewtGetFrameSlot(help, NewtFindSlotIndex(help, key));
          if (NewtRefIsString(value))
            return NewtRefToString(value);
        }
        // recurse into __ntAncestor and finally into the Script and Attribute Slots
        newtRef ancestor = NewtGetFrameSlot(tmplDB, NewtFindSlotIndex(tmplDB, NSSYM(__ntAncestor)));
        return getHelp(ancestor, key); // it's ok if there is no ancestor!
      }
    }
  }
  // if no id was given or no entry was found, then search ScriptSlots and AttributeSlots
  newtRef db = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(ScriptSlots)));
  if (NewtRefIsFrame(db)) {
    newtRef def = NewtGetFrameSlot(db, NewtFindSlotIndex(db, key));
    if (NewtRefIsFrame(def)) {
      newtRef value = NewtGetFrameSlot(def, NewtFindSlotIndex(def, NSSYM(__ntHelp)));
      if (NewtRefIsString(value))
        return NewtRefToString(value);
    }
  }
  db = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(AttributeSlots)));
  if (NewtRefIsFrame(db)) {
    newtRef def = NewtGetFrameSlot(db, NewtFindSlotIndex(db, key));
    if (NewtRefIsFrame(def)) {
      newtRef value = NewtGetFrameSlot(def, NewtFindSlotIndex(def, NSSYM(__ntHelp)));
      if (NewtRefIsString(value))
        return NewtRefToString(value);
    }
  }
  return 0L;
}


/*---------------------------------------------------------------------------*/
const char *Dtk_Platform::getHelp(Dtk_Template *tmpl, const char *slot)
{
  if (platform_==kNewtRefUnbind)
    return 0L;

  // get the _proto of the template as a C-String
  char *id = tmpl->id();
  if (!id)
    return 0L;
  
  return getHelp(NewtMakeSymbol(id), NewtMakeSymbol(slot));
  
  // find the frame containing descriptions for template types
  /// \todo Also search Templates: and ViewClassArray:
  newtRef ta = NewtGetFrameSlot(platform_, NewtFindSlotIndex(platform_, NSSYM(TemplateArray)));
  if (!NewtRefIsArray(ta))
    return 0L;

  // find the frame that describes our template
  int ix = NewtFindArrayIndex(ta, NewtMakeSymbol(id), 0);
  if (ix<0)
    return 0L;

  newtRef tmplDB = NewtGetFrameSlot(ta, ix+1);
  if (!NewtRefIsFrame(tmplDB))
    return 0L;

  // within the template description, find the __ntHelp slot
  newtRef help = NewtGetFrameSlot(tmplDB, NewtFindSlotIndex(tmplDB, NSSYM(__ntHelp)));
  if (!NewtRefIsFrame(help))
    return 0L;

  // now find the help text for our slot
  newtRef txt = NewtGetFrameSlot(help, NewtFindSlotIndex(help, NewtMakeSymbol(slot)));
  if (NewtRefIsString(txt))
    return NewtRefToString(txt);

  /// \todo Search in the rest of the database for more help texts
  ///       Also search ScriptSlots: and AttributeSlots:

  return 0L;
}


//
// End of "$Id$".
//
