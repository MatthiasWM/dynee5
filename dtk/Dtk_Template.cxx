//
// "$Id$"
//
// Dtk_Template implementation for the Dyne Toolkit.
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
#pragma warning(disable : 4996)
#endif

#include "Dtk_Template_List.h"
#include "Dtk_Layout.h"
#include "Dtk_Slot_List.h"
#include "Dtk_Template.h"
#include "Dtk_Slot.h"
#include "Dtk_Script_Slot.h"
#include "Dtk_Rect_Slot.h"
#include "Dtk_Value_Slot.h"
#include "Dtk_Proto_Slot.h"
#include "Dtk_Script_Writer.h"
#include "Dtk_Platform.h"

#include "flnt/Flnt_Widget.h"

#include "fluid/main_ui.h"

#include "allNewt.h"

#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Group.H>


/*---------------------------------------------------------------------------*/
Dtk_Template::Dtk_Template(Dtk_Layout *layout, 
                           Dtk_Template_List *list,
                           char *proto)
:   layout_(layout),
list_(list),
tmplList_(0L),
slotList_(0L),
index_(0),
indent_(0),
browser_(0L),
browserName_(0L),
ntName_(0L),
ntId_(0L),
scriptName_(0L),
autoScriptName_(true),
widget_(0L),
viewBounds_(0L),
viewJustify_(0L),
defaultJustify_(0),
proto_(0L)
{
  //if (proto && proto->setup)
  //proto->setup(this);
}


/*---------------------------------------------------------------------------*/
Dtk_Template::~Dtk_Template()
{
  if (widget_)
    widget_->parent()->remove(widget_);
  delete widget_;
  delete scriptName_;
  delete ntId_;
  delete ntName_;
  delete browserName_;
  delete slotList_;
  delete tmplList_;
}


/*---------------------------------------------------------------------------*/
void Dtk_Template::clear()
{
  if (slotList_)
    slotList_->clear();
  if (tmplList_)
    tmplList_->clear();
}


/*---------------------------------------------------------------------------*/
int Dtk_Template::load(newtRef node)
{
  //printf("===========\n");
  //NewtPrintObject(stdout, node);
  // find the name of this template
  newtRef ntName = NewtGetFrameSlot(node, NewtFindSlotIndex(node, NSSYM(__ntName)));
  if (NewtRefIsString(ntName)) ntName_ = strdup(NewtRefToString(ntName));
  
  // find an ID; this does not mean too much as the ID can be changed later in the UI
  newtRef ntId = NewtGetFrameSlot(node, NewtFindSlotIndex(node, NSSYM(__ntId)));
  if (NewtRefIsSymbol(ntId)) id(NewtSymbolGetName(ntId));
  
  // now load all slots for this template
	newtRef value = NewtGetFrameSlot(node, NewtFindSlotIndex(node, NSSYM(value)));
  if (NewtRefIsFrame(value)) {
    int i, n = NewtFrameLength(value);
    if (n)
      slotList(); // create a slot list
    for (i=0; i<n; i++) {
      //uint32_t index;
      newtRef key = NewtGetFrameKey(value, i);
      newtRef slot = NewtGetFrameSlot(value, i);
      if (!NewtRefIsFrame(slot)) 
        continue;
      // the 'stepChildren slot generates a new template list with all children
      if (key==NSSYM(stepChildren)) {
        newtRef a = NewtGetFrameSlot(slot, NewtFindSlotIndex(slot, NSSYM(value)));
        if (NewtRefIsArray(a)) {
          int i, n = NewtArrayLength(a);
          if (n) {
            tmplList_ = new Dtk_Template_List(this);
          }
          for (i=0; i<n; i++) {
            Dtk_Template *tmpl = new Dtk_Template(layout_, tmplList_);
            tmplList_->add(tmpl);
            tmpl->load(NewtGetArraySlot(a, i));
          }
        }
      } else {
        addSlot(key, slot);
      }
    }
  }
  return 0;
}


/*---------------------------------------------------------------------------*/
int Dtk_Template::write(Dtk_Script_Writer &sw)
{
  char buf[1024];
  
  if (!scriptName_ || autoScriptName_) {
    if (scriptName_)
      free(scriptName_);
    char name[32];
    sprintf(name, "_view%03d", sw.viewCount++);
    scriptName_ = strdup(name);
    autoScriptName_ = true;
  }
  
  // FIXME run the 'beforeScript
  
  Dtk_Template *p = parent();
  if (p) {
    sprintf(buf, "%s := /* child of %s */\n", scriptName(), parent()->scriptName());
  } else {
    sprintf(buf, "%s :=\n", scriptName());
  }
  sw.put(buf);
  sw.put("    {\n");
  
  int needComma = 0;
  // if there is a valid magic pointer for our ID, write a '_proto slot
  int proto = dtkPlatform->findProto(id());
  if (proto!=-1) {
    sprintf(buf, "     _proto: @%d", proto);
    sw.put(buf);
    needComma = 1;
  }
  // now write all the other slots
  if (slotList_) {
    int i, n = slotList_->size(), ret = 0;
    for (i=0; i<n; i++) {
      if (needComma)
        sw.put(",\n");
      ret = slotList_->at(i)->write(sw);
      needComma = (ret==0) ? 1 : 0;
    }
    if (needComma)
      sw.put("\n");
  }
  
  sw.put("    };\n\n");
  
  if (tmplList_) {
    int i, n = tmplList_->size();
    if (n) {
      for (i=0; i<n; i++) {
        tmplList_->at(i)->write(sw);
      }
      sprintf(buf, "%s.stepChildren := [ ", scriptName());
      sw.put(buf);
      for (i=0; i<n; i++) {
        sw.put(tmplList_->at(i)->scriptName());
        if (i<n-1)
          sw.put(", ");
      }
      sw.put(" ];\n\n");
      // or "AddStepView"?
    }
  }
  
  // FIXME run the 'afterScript
  
  return 0;
}


/*---------------------------------------------------------------------------*/
void Dtk_Template::updateBrowserLink(Fl_Hold_Browser *browser, int &indent, int &index, bool add)
{
  if (browserName_) {
    free(browserName_);
    browserName_ = 0L;
  }
  browser_ = browser;
  indent_ = indent;
  index_ = index++;
  if (add) {
    browser->add(browserName(), this);
    newWidget();
  }
  if (tmplList_) {
    int i, n = tmplList_->size();
    indent++;
    for (i=0; i<n; i++) {
      Dtk_Template *tmpl = tmplList_->at(i);
      tmpl->updateBrowserLink(browser, indent, index, add);
    }
    indent--;
  }
  if (add) {
    widget_->end();
  }
}


/*---------------------------------------------------------------------------*/
const char *Dtk_Template::browserName()
{
  if (browserName_)
    return browserName_;
  
  char *name = ntName_;
  char *id = ntId_;
  if (!id ||!*id)
    id = "-unknown-";
  
  char buf[512];
  memset(buf, ' ', 511);
  if (name && *name) {
    sprintf(buf+4*indent_, "%s <%s>", name, id);
  } else {
    sprintf(buf+4*indent_, "%s", id);
  }
  // FIXME there can also be a userProto. The Format is then
  //       infoButton <userProto> (protoInfoButton.lyt)
  browserName_ = strdup(buf);
  return browserName_;
}


/*---------------------------------------------------------------------------*/
void Dtk_Template::edit()
{
  Fl_Hold_Browser *slotBrowser = layout()->slotBrowser();
  slotBrowser->clear();
  if (slotList_) {
    int i, n = slotList_->size();
    for (i=0; i<n; i++) {
      Dtk_Slot *slot = slotList_->at(i);
      slotBrowser->add(slot->key(), slot);
    }
  }
}


/*---------------------------------------------------------------------------*/
void Dtk_Template::getSize(int &t, int &l, int &b, int &r)
{
  if (viewBounds_) {
    viewBounds_->get(t, l, b, r);
  }
}


/*---------------------------------------------------------------------------*/
unsigned int Dtk_Template::justify()
{
  return viewJustify_ ? (unsigned int)viewJustify_->value() : defaultJustify_;
}


/*---------------------------------------------------------------------------*/
void Dtk_Template::selectedInView()
{
  int i, n = browser_->size();
  for (i=1; i<=n; ++i) {
    if (browser_->data(i)==this)
      break;
  }
  if (i>n) return; // Ooops?
  browser_->value(i);
  browser_->do_callback();
}


/*---------------------------------------------------------------------------*/
char Dtk_Template::isSelected()
{
  return browser_->value()==index_ ? 1 : 0;
}


/*---------------------------------------------------------------------------*/
Dtk_Template *Dtk_Template::parent()
{
  if (list_)
    return list_->parent();
  return 0L;
}


/*---------------------------------------------------------------------------*/
Dtk_Template *Dtk_Template::addTemplate(int x, int y, int w, int h, char *proto)
{
  if (!proto) {
    Fl_Choice *c = dtkMain->tTemplateChoice;
    proto = (char*)c->menu()[c->value()].label();
  }
  
  Dtk_Template *tmpl = new Dtk_Template(layout_, list_, proto);
  
  tmpl->load(dtkPlatform->newtTemplate(proto));
  
  //if (tmpl->viewBounds_)
  //    tmpl->viewBounds_->set(x, y, w, h);
  
  if (!tmplList_)
    tmplList_ = new Dtk_Template_List(this);
  tmplList_->add(tmpl);
  
  int indent = indent_, index = index_;
  updateBrowserLink(browser_, indent, index);
  browser_->insert(tmpl->index_, tmpl->browserName(), tmpl);
  
  widget_->begin();
  tmpl->newWidget();
  tmpl->widget_->resize(x, y, w, h);
  tmpl->widget_->newtSetJustify(tmpl->justify());
  tmpl->widget_->signalBoundsChanged();
  Fl_Group::current(0L);
  widget_->redraw();
  
  return tmpl;
}

/*---------------------------------------------------------------------------*/
Dtk_Slot_List *Dtk_Template::slotList() 
{ 
  if (!slotList_)
    slotList_ = new Dtk_Slot_List(this);
  return slotList_; 
}

/*---------------------------------------------------------------------------*/
void Dtk_Template::addSlot(Dtk_Slot *slot) 
{
  slotList()->add(slot);
}

/*---------------------------------------------------------------------------*/
void Dtk_Template::removeSlot(Dtk_Slot *slot) 
{
  if (slot==viewBounds_)
    viewBounds_ = 0L; // FIXME: this *will* cause a crash!
  if (slot==viewJustify_)
    viewJustify_ = 0L; 
  slotList()->remove(slot);
}

/*---------------------------------------------------------------------------*/
void Dtk_Template::removeAllSlots()
{
  if (slotList_) {
    while (slotList_->size()) {
      DeleteSlot(slotList_->at(0));
    }
  }
}

/*---------------------------------------------------------------------------*/
void Dtk_Template::removeAllChildren()
{
  if (tmplList_) {
    while (tmplList_->size()) {
      DeleteTemplate(tmplList_->at(0));
    }
  }
}

/*---------------------------------------------------------------------------*/
Dtk_Slot *Dtk_Template::addSlot(newtRef key, newtRef slot) 
{
  if (!NewtRefIsFrame(slot)) 
    return 0L;
  // the 'stepChildren slot generates a new template list with all children
  if (key==NSSYM(stepChildren))
    return 0L;
  newtRef datatype = NewtGetFrameSlot(slot, NewtFindSlotIndex(slot, NSSYM(__ntDatatype)));
  Dtk_Slot *dSlot = 0L;
  if (NewtRefIsString(datatype)) {
    const char *dt = NewtRefToString(datatype);
    const char *keyname = NewtSymbolGetName(key);
    if (strcmp(dt, "SCPT")==0 || strcmp(dt, "EVAL")==0) {
      dSlot = new Dtk_Script_Slot(slotList_, keyname, slot);
    } else if (strcmp(dt, "RECT")==0) {
      dSlot = new Dtk_Rect_Slot(slotList_, keyname, slot);
    } else if (strcmp(dt, "NUMB")==0) {
      dSlot = new Dtk_Value_Slot(slotList_, keyname, slot);
    } else if (strcmp(dt, "PROT")==0) {
      dSlot = new Dtk_Proto_Slot(slotList_, "_proto", slot);
    } else {
      // also: RECT, CLAS, NUMB, PICT, USER, any more?
      // CLAS is __ntTemplate
      // PROT is __ntTemplate
      // USER is __ntTemplate
      // PICT is icon
      printf("Unsupported slot datatype \"%s\" (%s)\n", dt, keyname);
      dSlot = new Dtk_Slot(slotList_, keyname, slot);
    }
  }
  if (key==NSSYM(viewBounds)) {
    viewBounds_ = (Dtk_Rect_Slot*)dSlot;
    viewBounds_->signalRectChanged.connect(this, (Flmm_Slot)&Dtk_Template::viewBoundsChangedSignal);
  } else if (key==NSSYM(viewJustify)) {
    viewJustify_ = (Dtk_Value_Slot*)dSlot;
    viewJustify_->signalValueChanged.connect(this, (Flmm_Slot)&Dtk_Template::viewJustifyChangedSignal);
  }
  if (dSlot) {
    slotList_->add(dSlot);
  }
  return dSlot;
}

/*---------------------------------------------------------------------------*/
Dtk_Slot *Dtk_Template::findSlot(const char *key)
{
  return slotList()->find(key);
}


/*---------------------------------------------------------------------------*/
void Dtk_Template::id(const char *cNewID)
{
  char *newID = strdup(cNewID);
  if (ntId_)
    free(ntId_);
  ntId_ = newID;
  if (browserName_) {
    free(browserName_);
    browserName_ = 0L;
  }
  layout()->templateBrowser()->text(index_, browserName());
  if (widget_)
    widget_->copy_label(browserName());
  
  // A few protos have a different default justify if no viewJustif slot exists
  if (strcasecmp(newID, "protoApp")==0) {
    defaultJustify_ = 16;
  } else {
    defaultJustify_ = 0;
  }
  if (widget_ && !viewJustify_) {
    widget_->newtSetJustify(defaultJustify_);
  }
}


/*---------------------------------------------------------------------------*/
Flnt_Widget *Dtk_Template::newWidget()
{
  widget_ = new Flnt_Widget(this);
  widget_->signalBoundsChanged.connect(this, (Flmm_Slot)&Dtk_Template::widgetBoundsChangedSignal);
  widget_->newtSetJustify(justify());
  viewBoundsChangedSignal();
  return widget_;
}


/*---------------------------------------------------------------------------*/
void Dtk_Template::widgetBoundsChangedSignal()
{
  if (viewBounds_ && widget_) {
    int t, l, b, r;
    widget_->newtGetRect(t, l, b, r);
    viewBounds_->set(t, l, b, r);
    viewBounds_->revert();
  }
}

/*---------------------------------------------------------------------------*/
void Dtk_Template::viewBoundsChangedSignal()
{
  if (viewBounds_ && widget_) {
    int t, l, b, r;
    viewBounds_->get(t, l, b, r);
    widget_->newtSetRect(t, l, b, r);
  }
}


/*---------------------------------------------------------------------------*/
void Dtk_Template::viewJustifyChangedSignal()
{
  if (viewJustify_ && widget_) {
    unsigned int j = (unsigned int)viewJustify_->value();
    widget_->newtSetJustify(j);
    // setting the justify will likely modify the viewBounds!
    widgetBoundsChangedSignal();
  }
}

/*---------------------------------------------------------------------------*/
newtRef	Dtk_Template::save()
{
  newtRefVar valueA[512]; // ouch, no fixed sizes!
  newtRefVar tmpA[1024]; // ouch, no fixed sizes!
  int vi = 0, ti = 0;
  
  // the first slot is the template if we have one
  int v = dtkPlatform->findProto(id());
  if (v!=-1) {
    tmpA[ti++] = NSSYM(value);
    tmpA[ti++] = NewtMakeInt30(v);
    tmpA[ti++] = NSSYM(__ntDataType);
    tmpA[ti++] = NewtMakeString("PROT", true);
    tmpA[ti++] = NSSYM(__ntFlags);
    tmpA[ti++] = NewtMakeInt30(16);
    valueA[vi++] = NSSYM(__ntTemplate);
    valueA[vi++] = NewtMakeFrame2(3, tmpA+ti-6);
  }
  // now walk the slots
  int i, n = slotList_->size();
  for (i=0; i<n; ++i) {
    Dtk_Slot *slot = slotList_->at(i);
    slot->apply();
    newtRef newtSlot = slot->save();
    if (newtSlot!=kNewtRefUnbind) {
      valueA[vi++] = NewtMakeSymbol(slot->key());
      valueA[vi++] = newtSlot;
    }
  }
  
  // now walk the children
  n = tmplList_ ? tmplList_->size() : 0;
  if (n) {
    tmpA[ti++] = NSSYM(value);
    newtRef sc = tmpA[ti++] = NewtMakeArray(NSSYM(stepChildren), n);
    tmpA[ti++] = NSSYM(__ntDataType);
    tmpA[ti++] = NewtMakeString("ARAY", true);
    tmpA[ti++] = NSSYM(__ntFlags);
    tmpA[ti++] = NewtMakeInt30(64);
    for (i=0; i<n; ++i) {
      Dtk_Template *tmpl = tmplList_->at(i);
      newtRef newtTmpl = tmpl->save();
      NewtSetArraySlot(sc, i, newtTmpl);
    }
    valueA[vi++] = NSSYM(stepChildren);
    valueA[vi++] = NewtMakeFrame2(3, tmpA+ti-6);
  }
  
  /*
   stepChildren: {
   value: [
   stepChildren: 
   {
   __ntDataType: "ARAY", 
   __ntFlags: 64
   */
  newtRef value = NewtMakeFrame2(vi/2, valueA);
  
  int hi = 0;
  newtRefVar hrcA[20];
  hrcA[hi++] = NSSYM(value);
  hrcA[hi++] = value;
  if (ntId_ && *ntId_) {
    hrcA[hi++] = NSSYM(__ntId);
    hrcA[hi++] = NewtMakeSymbol(id());
  }
  if (ntName_ && *ntName_) {
    hrcA[hi++] = NSSYM(__ntName);
    hrcA[hi++] = NewtMakeString(ntName_,true);
  };
  //NSSYM(__ntExternFile),  ...,
  //NSSYM(__ntDeclare),     ...
  newtRef hrc = NewtMakeFrame2(hi/2, hrcA);
  
  NewtPrintObject(stdout, hrc);
  
  return hrc;
}

/*---------------------------------------------------------------------------*/
void Dtk_Template::setName(const char *name)
{
  if (name && (*name==0L))
    name = 0L;
  
  // check if anything changed at all
  if (name && ntName_ && strcmp(name, ntName_)==0)
    return;
  if ((name==0L)&&(ntName_==0L))
    return;
  
  // invalidate all naming related variables
  if (browserName_) {
    free(browserName_);
    browserName_ = 0L;
  }
  if (scriptName_) {
    free(scriptName_);
    scriptName_ = 0L;
  }
  if (ntName_) {
    free(ntName_);
    ntName_ = 0L;
  }
  if (name) {
    ntName_ = strdup(name);
  }
  
  // now update the GUI that we know
  /// \todo don't update anything here, just send a signal to all subscribers
  if (browser_)
    browser_->text(index_, browserName());
  if (widget_)
    widget_->copy_label(browserName());
}


//
// End of "$Id$".
//
