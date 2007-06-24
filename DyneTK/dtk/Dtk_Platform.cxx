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


#include "Dtk_Platform.h"

#include "allNewt.h"

#include <FL/fl_ask.h>

#include <stdio.h>
#include <sys/stat.h>


/*---------------------------------------------------------------------------*/
Dtk_Platform::Dtk_Platform(const char *filename)
:   platform_(kNewtRefUnbind),
    templateChoiceMenu_(0L)
{
    load(filename);
}


/*---------------------------------------------------------------------------*/
Dtk_Platform::~Dtk_Platform()
{
}


/*---------------------------------------------------------------------------*/
int Dtk_Platform::load(const char *filename)
{
    struct stat st;
    if (stat(filename, &st)) {
        fl_alert(
            "Platform file \"%s\" not found.\n\n"
            "This file is required to edit DyneTK projects. It must be placed in the DyneTK start directory.\n"
            "\"%s\" is part of the Mac NTK and should be available on Unna:\n\n"
            "http://www.unna.org/view.php?/apple/development/NTK/platformfiles/Newton2.1_Platform_File.sea.hqx",
            filename, filename);
        return -1;
    }
    FILE *f = fopen(filename, "rb");
	uint8_t *pf = (uint8_t*)malloc(st.st_size);
    fread(pf, 1, st.st_size, f);
    fclose(f);

    platform_ = NewtReadNSOF(pf, st.st_size);

    FILE *ff;
		NcSetGlobalVar(NSSYM(printLength), NSINT(9999));
		NcSetGlobalVar(NSSYM(printDepth), NSINT(8));
		NEWT_DUMPBC = 0;
		NEWT_INDENT = -2;
ff = fopen("platform1.txt", "wb");
NewtPrintObject(ff, platform_);
fclose(ff);

newtRef ta = NewtGetArraySlot(platform_, NewtFindSlotIndex(platform_, NSSYM(TemplateArray)));
if (ta==kNewtRefUnbind)
    return 0L;

ff = fopen("platform.txt", "wb");
NewtPrintObject(ff, ta);
fclose(ff);

    free(pf);

    if (platform_==kNewtRefUnbind) {
        fl_alert("Error reading platform file.");
        return -1;
    } else {
        NcSetGlobalVar(NSSYM(DyneTK_Platform), platform_);
        return 0;
    }
}


/*---------------------------------------------------------------------------*/
Fl_Menu_Item *Dtk_Platform::templateChoiceMenu()
{
    if (templateChoiceMenu_)
        return templateChoiceMenu_;
    if (platform_==kNewtRefUnbind)
        return 0L;

    newtRef ta = NewtGetArraySlot(platform_, NewtFindSlotIndex(platform_, NSSYM(TemplateArray)));
    if (ta==kNewtRefUnbind)
        return 0L;

//FILE *ff = fopen("platform.txt", "wb");
//NewtPrintObject(ff, ta);
//fclose(ff);

    int i, n = NewtArrayLength(ta);
    for (i=0; i<n; i++) {
        newtRef sym = NewtSlotsGetSlot(ta, i);
        NewtPrintObject(stdout, sym);
        if (NewtRefIsSymbol(sym)) {
            printf("SYM: %s\n", NewtSymbolGetName(sym));
        } else {
            printf("++\n");
        }
    }

    return 0L;
}


/*
void testNSOFReader(const char *filename) 
{
    struct stat st;
    stat("testing_script_writer.txt", &st);
    FILE *f = fopen("testing_script_writer.txt", "rb");
	char *script = (char*)malloc(st.st_size);
    fread(script, 1, st.st_size, f);
    fclose(f);

    
    
    if (!filename)
		filename = fl_file_chooser("Load NSOF File", "*", 0L);
	if (!filename) return;

	uint8_t *buffer;
	FILE *f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	int nn = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer = (uint8_t*)malloc(nn);
	int n = fread(buffer, 1, nn, f);
	fclose(f);
	if (n) {
		NcSetGlobalVar(NSSYM(printLength), NSINT(9999));
		NcSetGlobalVar(NSSYM(printDepth), NSINT(3));
		NEWT_DUMPBC = 0;
		NEWT_INDENT = -2;

		FILE *f = fopen("dump_nsof.txt", "wb");
		newtRef pkg = NewtReadNSOF(buffer, n);
		NewtPrintObject(f, pkg);
		fclose(f);

	}
}
*/


#if 0

{
  MagicPointerTable: [
  ], 
  platformFunctions: {
  }, 
  platformVariables: {
  }, 
  platformConstants: {
  }, 
  version: 5, 
  __ntDefaults: {
    RECT: {
      top: 0, 
      left: 0, 
      bottom: 0, 
      right: 0
    }, 
    SCPT: "func()\rbegin\rend", 
    EVAL: "nil"
  }, 
  NameMap: {
  }, 
  TemplateArray: [
    'protoApp, 
    {
      PutAway: "func(item)\rbegin\r\t// Put away a newly-received beam\rend", 
      __ntOptional: {
        appAll: "EVAL", 
        appObject: "EVAL", 
        filingChanged: "SCPT", 
        filterChanged: "SCPT", 
        folderChanged: "SCPT", 
        labelsFilter: "EVAL", 
        target: "EVAL", 
        targetView: "EVAL", 
        appSymbol: "EVAL", 
        PutAway: "SCPT", 
        setupRoutingSlip: "SCPT", 
        setupTarget: "SCPT", 
        DateFind: "SCPT", 
        Find: "SCPT", 
        FindSoupExcerpt: "SCPT", 
        titleHeight: "NUMB", 
        viewJustify: "NUMB", 
        ReorientToScreen: "SCPT"
      }, 
      setupTarget: "func(slip)\rbegin\r\t// Set up slip before routing\r\tslip.target := self.target;\r\tslip.targetView := self;\rend", 
      __ntHelp: {
        appAll: "A string that is displayed as the last item in the protoFolderTab popup menu.", 
        appObject: "The singular and plural forms of the application's data items.", 
        filingChanged: "Called when the user chooses a folder in which to file the current item.", 
        filterChanged: "Called when the user selects a different folder to view.", 
        folderChanged: "Called when the user adds, deletes, or renames a folder. It is called once for each soup for which the application is registered.", 
        labelsFilter: "A symbol or nil. Indicates the current filing category selected from the protoFolderTab popup list. Maintained by the filing system.", 
        target: "The soup entry with which the user is working, or nil if there is no active item.", 
        targetView: "The view to which routing and filing messages are sent.", 
        appSymbol: "The symbol uniquely identifying the application.", 
        PutAway: "Called to put away a newly-received beam.", 
        setupRoutingSlip: "Called to give the application a chance to change or add information  before routing.", 
        setupTarget: "This method can be defined if the application needs to set up the target data at the time of routing.", 
        DateFind: "Called to handle a search for application items from before or after a user-specified date.", 
        Find: "Called to handle a text search of the application's data.", 
        FindSoupExcerpt: "Returns string to display in the find overview.", 
        title: "The title of your application.", 
        titleHeight: "The height of the application title."
      }, 
      titleHeight: 12, 
      viewFormat: 83951953, 
      appAll: "\"All Items\"", 
      target: "nil\t// Current soup entry", 
      folderChanged: "func(soupName, oldFolder, newFolder)\rbegin\r\t// A folder was added, renamed, or deleted\rend", 
      viewFlags: 5, 
      DateFind: "func(findTime, filter, results, scope, statusContext)\rbegin\r\t// Find all items from before or after findTime\rend", 
      appObject: "[\"item\", \"items\"]\t\t// used for filing", 
      filingChanged: "func()\rbegin\r\t// User picked a new folder for current item\rend", 
      _proto: 157, 
      filterChanged: "func()\rbegin\r\t// User picked a new folder to view\rend", 
      labelsFilter: "nil\t// Currently displayed folder. Maintained by by filing system", 
      appSymbol: "nil\t// The symbol identifying the application", 
      viewJustify: 16, 
      title: "\"Application\"", 
      Find: "func(what, results, scope, statusContext)\rbegin\r\t// Find string what and put the result frame in array results\rend", 
      setupRoutingSlip: "func(fields)\rbegin\r\tfields.title := \"Application,\" && DateNTime(Time());\rend", 
      FindSoupExcerpt: "func(entry, resultFrame)\rbegin\r\t// Return a string to represent entry in find overview\rend", 
      targetView: "nil\t// For routing and filing", 
      __ntRequired: {
        title: "EVAL", 
        viewBounds: "RECT", 
        viewFormat: "NUMB"
      }, 
      ReorientToScreen: "func()\rbegin\r\tcall ROM_DefRotateFunc with ();\rend"
    }, 
    'protoFloater, 
    {
      _proto: 179, 
      noScroll: "\"This application does not support scrolling\"", 
      noOverview: "\"This application does not support Overview\"", 
      viewEffect: 236577, 
      viewFlags: 64, 
      viewJustify: 16, 
      __ntRequired: {
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        noScroll: "EVAL", 
        noOverview: "EVAL", 
        viewEffect: "NUMB"
      }, 
      __ntHelp: {
        noScroll: "\"Message the user gets when they hit the scroll arrows.\"", 
        noOverview: "\"Message the user gets when they hit the overview button.\""
      }, 
      __ntAncestor: 'protoDragger
    }, 
    'protoFloatNGo, 
    {
      _proto: 180, 
      __ntAncestor: 'protoFloater, 
      __ntRequired: {
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        noScroll: "EVAL", 
        noOverview: "EVAL", 
        viewEffect: "NUMB"
      }
    }, 
    'protoInputLine, 
    {
      _proto: 185, 
      viewFlags: 14849, 
      viewFormat: 12288, 
      viewLineSpacing: 20, 
      viewLinePattern: "nil", 
      viewJustify: 8388608, 
      viewFont: "editFont12", 
      ViewSetupDoneScript: "func()\rbegin\r\tinherited:?viewSetupDoneScript();\t\t// this method is defined internally\rend", 
      viewTransferMode: 1, 
      __ntRequired: {
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        text: "EVAL", 
        ViewChangedScript: "SCPT", 
        viewFont: "EVAL", 
        viewLinePattern: "EVAL", 
        viewLineSpacing: "NUMB"
      }, 
      __ntHelp: {
        text: "The default text for the input line.", 
        ViewChangedScript: "This is called whenever the value of the input field is changed.", 
        viewFont: "The font for the text the user writes in the input field.", 
        viewLinePattern: "The pattern used to draw the line.", 
        viewLineSpacing: "The height of the input line in pixels."
      }
    }, 
    'protoLabelInputLine, 
    {
      _proto: 189, 
      __ntAncestor: 'protoInputLine, 
      curLabelCommand: 0, 
      entryFlags: 14849, 
      indent: 0, 
      label: "\"Label\"", 
      labelActionScript: "func(cmdIndex)\rbegin\r\t// return true if selection completely handled, nil otherwise\rend", 
      labelClick: "func(unit)\rbegin\r\t// Return true if click has been completely handled, nil otherwise\rend", 
      labelCommands: "[\"picker option one\", \"picker option two\"]", 
      labelFont: "ROM_fontSystem9Bold", 
      text: "\"Some Initial Value\"", 
      textChanged: "func()\rbegin\rend", 
      textSetup: "func()\rbegin\r\ttext\t// return value becomes the initial value of the input field\rend", 
      viewFlags: 513, 
      viewFormat: 0, 
      __ntRequired: {
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        curLabelCommand: "NUMB", 
        entryFlags: "NUMB", 
        indent: "INTG", 
        label: "EVAL", 
        labelActionScript: "SCPT", 
        labelClick: "SCPT", 
        labelCommands: "EVAL", 
        labelFont: "EVAL", 
        text: "EVAL", 
        textChanged: "SCPT", 
        textSetup: "SCPT", 
        viewLinePattern: "EVAL", 
        viewLineSpacing: "NUMB"
      }, 
      __ntHelp: {
        curLabelCommand: "Index that specifies default label to check in the labelCommands array.", 
        entryFlags: "Used to set the type of input the entry line will accept.", 
        indent: "The distance from the left edge of the view for the dotted line to begin.", 
        label: "The label text for the input line.", 
        labelActionScript: "This is called when an item is chosen from the label popup.  The item index is passed into this method.", 
        labelClick: "This is called when the user taps in the label.", 
        labelCommands: "If this slot is supplied, then the pop-up feature is activated.  Specify an array of strings to use as popup items.", 
        labelFont: "Font to display the label in.", 
        text: "Used to set an initial value for the input line", 
        textChanged: "This is called when the input text changes.", 
        textSetup: "This is called to set the initial value of the entry field."
      }
    }, 
    'protoGauge, 
    {
      _proto: 182, 
      gaugeDrawLimits: TRUE, 
      maxValue: 100, 
      minValue: 0, 
      viewFlags: 3, 
      viewFormat: 0, 
      viewValue: 50, 
      __ntRequired: {
        viewBounds: "RECT", 
        ViewSetupFormScript: "SCPT", 
        viewValue: "NUMB"
      }, 
      __ntOptional: {
        gaugeDrawLimits: "BOOL", 
        maxValue: "NUMB", 
        minValue: "NUMB"
      }, 
      __ntHelp: {
        gaugeDrawLimits: "If this field is true, the gray background is displayed. If it is nil, the gray background is not displayed.", 
        maxValue: "The maximum value to display.", 
        minValue: "The minimum value to display.", 
        viewValue: "This is the value to display."
      }
    }, 
    'protoSlider, 
    {
      _proto: 212, 
      __ntAncestor: 'protoGauge, 
      changedSlider: "func()\rbegin\rend", 
      declareSelf: "'baseGauge", 
      gaugeDrawLimits: TRUE, 
      maxValue: 100, 
      minValue: 0, 
      ViewChangedScript: "func(slot, view)\rbegin\r\tinherited:?viewChangedScript(slot, view);\t\t// this method is defined internally\rend", 
      viewFinalChangeScript: "func()\rbegin\r\tinherited:?viewFinalChangeScript();\t\t// this method is defined internally\rend", 
      viewFlags: 513, 
      viewFormat: 0, 
      ViewSetupFormScript: "func()\rbegin\r\t// set the initial values for the slider\rend", 
      viewValue: 50, 
      __ntRequired: {
        changedSlider: "SCPT", 
        viewBounds: "RECT", 
        ViewSetupFormScript: "SCPT", 
        viewValue: "NUMB"
      }, 
      __ntOptional: {
        gaugeDrawLimits: "BOOL", 
        maxValue: "NUMB", 
        minValue: "NUMB", 
        trackSlider: "SCPT"
      }, 
      __ntHelp: {
        changedSlider: "This is called after the user changes the slider setting.", 
        trackSlider: "This is called repeatedly as the user drags the slider."
      }
    }, 
    'protoBorder, 
    {
      _proto: 160, 
      viewFlags: 1, 
      viewFormat: 5, 
      __ntRequired: {
        viewBounds: "RECT"
      }
    }, 
    'protoDivider, 
    {
      _proto: 172, 
      title: "\"\"", 
      titleHeight: 12, 
      viewBounds: {
        top: 0, 
        left: 0, 
        bottom: 16, 
        right: 0
      }, 
      viewFlags: 3, 
      viewFont: "ROM_fontSystem9Bold", 
      viewFormat: 0, 
      viewJustify: 48, 
      viewSetupChildren: "func()\rbegin\r\tinherited:?viewSetupChildren();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        title: "EVAL", 
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        titleHeight: "NUMB", 
        viewFont: "EVAL"
      }, 
      __ntHelp: {
        title: "A string which is the text displayed on the divider bar.", 
        titleHeight: "Height of the title. If this is nil then the height is calculated from viewFont."
      }
    }, 
    'protoTextButton, 
    {
      _proto: 226, 
      buttonClickScript: "func()\rbegin\rend", 
      buttonPressedScript: "func()\rbegin\rend", 
      text: "\"Button\"", 
      ViewClickScript: "func(unit)\rbegin\r\t// This method is defined internally - do not override it\r\t// Define your own buttonClickScript instead\rend", 
      viewFlags: 515, 
      viewFont: "ROM_fontSystem9Bold", 
      viewFormat: 67109457, 
      viewJustify: 8388614, 
      viewTransferMode: 1, 
      __ntRequired: {
        buttonClickScript: "SCPT", 
        text: "EVAL", 
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        buttonPressedScript: "SCPT", 
        viewFont: "EVAL"
      }, 
      __ntHelp: {
        buttonClickScript: "This is called when the button is tapped.", 
        buttonPressedScript: "This is called repeatedly as long as the button is pressed.", 
        text: "The label for the button."
      }
    }, 
    'protoPictureButton, 
    {
      _proto: 198, 
      buttonClickScript: "func()\rbegin\rend", 
      buttonPressedScript: "func()\rbegin\rend", 
      icon: NIL, 
      ViewClickScript: "func(unit)\rbegin\r\t// This method is defined internally - do not override it\r\t// Define your own buttonClickScript instead\rend", 
      viewFlags: 515, 
      viewFormat: 67109457, 
      viewJustify: 6, 
      __ntRequired: {
        viewBounds: "RECT", 
        buttonClickScript: "SCPT"
      }, 
      __ntOptional: {
        buttonPressedScript: "SCPT", 
        icon: "PICT"
      }, 
      __ntHelp: {
        buttonClickScript: "This is called when the button is tapped.", 
        buttonPressedScript: "This is called repeatedly as long as the button is pressed.", 
        icon: "The picture (PICT) for the button."
      }
    }, 
    'protoLargeClosebox, 
    {
      _proto: 163, 
      buttonClickScript: "func()\rbegin\r\tinherited:?buttonClickScript();\t\t// this method is defined internally\rend", 
      viewBounds: {
        left: -18, 
        top: -18, 
        right: -5, 
        bottom: -5
      }, 
      viewFormat: 67109457, 
      viewJustify: 166
    }, 
    'protoCloseBox, 
    {
      _proto: 166, 
      __ntAncestor: 'protoPictureButton, 
      buttonClickScript: "func()\rbegin\r\tinherited:?buttonClickScript();\t\t// this method is defined internally\rend", 
      viewBounds: {
        left: -14, 
        top: -14, 
        right: -1, 
        bottom: -1
      }, 
      viewFormat: 0, 
      viewJustify: 160
    }, 
    'protoCheckBox, 
    {
      _proto: 164, 
      buttonValue: "1", 
      declareSelf: "'base", 
      text: "\"Untitled Check\"", 
      valueChanged: "func()\rbegin\rend", 
      viewFlags: 513, 
      viewFont: "ROM_fontSystem9", 
      viewFormat: 0, 
      viewJustify: 4, 
      viewValue: NIL, 
      __ntRequired: {
        text: "EVAL", 
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        buttonValue: "EVAL", 
        declareSelf: "EVAL", 
        valueChanged: "SCPT", 
        viewFont: "EVAL", 
        viewValue: "BOOL"
      }, 
      __ntHelp: {
        buttonValue: "The value for viewValue when the checkbox is checked.", 
        text: "The label for the check box.", 
        valueChanged: "This is called when the state of the checkbox changes.", 
        viewValue: "The current value of the checkbox. Set to nil when unchecked and to the value specified in buttonValue when checked."
      }
    }, 
    'protoRCheckBox, 
    {
      _proto: 204, 
      __ntAncestor: 'protoCheckBox, 
      indent: 16, 
      text: "\"Right Checkbox\"", 
      viewJustify: 5, 
      ViewChangedScript: "func(slot, view)\rbegin\r\t// this method is defined internally, so call inherited:?viewChangedScript()\r\tnil;\rend", 
      ViewClickScript: "func(unit)\rbegin\r\tinherited:?viewClickScript(unit);\t\t// this method is defined internally\r\t// Return true if click has been completely handled, nil otherwise\rend", 
      viewFont: "ROM_fontSystem9", 
      ViewSetupDoneScript: "func()\rbegin\r\tinherited:?viewSetupDoneScript();\t\t// this method is defined internally\rend", 
      ViewSetupFormScript: "func()\rbegin\r\tself.indent := self.indent + StrFontWidth(self.text,self.viewFont);\r\tinherited:?viewSetupFormScript();\rend", 
      __ntRequired: {
        indent: "NUMB", 
        text: "EVAL", 
        viewBounds: "RECT", 
        ViewSetupFormScript: "SCPT"
      }, 
      __ntOptional: {
        buttonValue: "EVAL", 
        declareSelf: "EVAL", 
        valueChanged: "SCPT", 
        viewFont: "EVAL", 
        viewValue: "BOOL"
      }
    }, 
    'protoRadioButton, 
    {
      _proto: 202, 
      __ntAncestor: 'protoCheckBox, 
      buttonValue: "1", 
      radioClickScript: "func(unit)\rbegin\r\tInherited:?radioClickScript(unit);\rend", 
      text: "\"Untitled RadioButton\"", 
      ViewClickScript: "func(unit)\rbegin\r\tinherited:?viewClickScript(unit);\t\t// this method is defined internally\r\t// Return true if click has been completely handled, nil otherwise\rend", 
      viewFormat: 0, 
      ViewSetupDoneScript: "func()\rbegin\r\tinherited:?viewSetupDoneScript();\t\t// this method is defined internally\rend", 
      viewValue: NIL, 
      __ntRequired: {
        buttonValue: "EVAL", 
        viewBounds: "RECT", 
        text: "EVAL"
      }, 
      __ntOptional: {
        viewValue: "BOOL", 
        ViewClickScript: "SCPT"
      }, 
      __ntHelp: {
        buttonValue: "Used to set the clusterValue of a protoRadioCluster. Each radio button in a cluster should have a unique buttonValue.", 
        text: "A string that is the radio button text label.", 
        viewValue: "The current state of the radio button. Holds the value in buttonValue when selected and nil when unselected."
      }
    }, 
    'protoPictRadioButton, 
    {
      _proto: 197, 
      __ntAncestor: 'protoPictureButton, 
      buttonValue: "1", 
      icon: NIL, 
      ViewClickScript: "func(unit)\rbegin\r\tinherited:?viewClickScript(unit);\t\t// this method is defined internally\r\t// Return true if click has been completely handled, nil otherwise\rend", 
      ViewDrawScript: "func()\rbegin\r\t// if viewValue then hilight the radio button else take no action\rend", 
      UpdateBitmap: "func()\rbegin\r\tinherited:?UpdateBitmap();\t\t// this method is defined internally\rend", 
      viewValue: NIL, 
      __ntRequired: {
        buttonValue: "EVAL", 
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        icon: "PICT", 
        ViewDrawScript: "SCPT", 
        viewValue: "BOOL"
      }, 
      __ntHelp: {
        buttonValue: "Used to set the clusterValue of a protoRadioCluster. Each radio button in a cluster should have a unique buttonValue.", 
        ViewDrawScript: "This method must hilight the radio button in some appropriate fashion.", 
        viewValue: "The current state of the radio button. Holds the value in buttonValue when selected and nil when unselected."
      }
    }, 
    'protoRadioCluster, 
    {
      _proto: 203, 
      clusterChanged: "func()\rbegin\rend", 
      clusterValue: "1", 
      viewFlags: 3, 
      viewFormat: 0, 
      __ntRequired: {
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        clusterChanged: "SCPT", 
        clusterValue: "EVAL", 
        ViewSetupFormScript: "SCPT"
      }, 
      __ntHelp: {
        clusterChanged: "This is called when the value of the cluster changes.", 
        clusterValue: "The currently selected radio button buttonValue. The default radio button may be specified by storing its buttonValue here.", 
        ViewSetupFormScript: "This method can be used to calculate and set the default clusterValue at run time."
      }
    }, 
    'protoStaticText, 
    {
      _proto: 218, 
      styles: "[11, simpleFont9 + tsBold]", 
      tabs: "[10, 20, 30, 40]", 
      text: "\"Static Text\"", 
      viewFlags: 3, 
      viewFont: "ROM_fontSystem10Bold", 
      viewFormat: 0, 
      viewJustify: 8388608, 
      viewTransferMode: 1, 
      __ntRequired: {
        text: "EVAL", 
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        styles: "EVAL", 
        tabs: "EVAL", 
        viewFont: "EVAL"
      }, 
      __ntHelp: {
        styles: "An array of alternating run lengths and font information, if multiple font styles are used.", 
        tabs: "An array of up to 8 tab-stops, in pixels.", 
        text: "The text to be displayed."
      }
    }, 
    'protoGlance, 
    {
      _proto: 183, 
      text: "\"Text\"", 
      ViewClickScript: "func(unit)\rbegin\r\tinherited:?viewClickScript(unit);\t\t// this method is defined internally\r\t// Return true if click has been completely handled, nil otherwise\rend", 
      viewEffect: 327680, 
      viewFlags: 516, 
      viewFont: "ROM_fontSystem9Bold", 
      viewFormat: 66129, 
      viewIdleFrequency: 3000, 
      ViewIdleScript: "func()\rbegin\r\tinherited:?viewIdleScript();\t\t// this method is defined internally\r\t300;\t\t// Return the number of milliseconds to delay or nil to end idling.\rend", 
      viewJustify: 4, 
      ViewSetupDoneScript: "func()\rbegin\r\tinherited:?viewSetupDoneScript();\t\t// this method is defined internally\rend", 
      ViewSetupFormScript: "func()\rbegin\r\tself.evaluate := $  & DateNTime(realData.timeStamp) && EntrySize(realData) && \"bytes\";\r\tself.viewBounds := RelBounds(18, 2, StrFontWidth(self.evaluate, viewFont), 13);\rend", 
      __ntRequired: {
        text: "EVAL", 
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        viewIdleFrequency: "NUMB", 
        viewFont: "EVAL", 
        ViewSetupFormScript: "SCPT"
      }, 
      __ntHelp: {
        viewIdleFrequency: "The length of time the view should remain open, in milliseconds."
      }
    }, 
    'protoStatus, 
    {
      _proto: 219, 
      __ntAncestor: 'protoStatusBar, 
      viewFlags: 1, 
      viewFormat: 0, 
      viewJustify: 176, 
      viewBounds: {
        left: 0, 
        top: -18, 
        right: 0, 
        bottom: 0
      }
    }, 
    'protoStatusBar, 
    {
      _proto: 220, 
      viewFlags: 1, 
      viewFormat: 0, 
      viewJustify: 176, 
      viewBounds: {
        left: 0, 
        top: -18, 
        right: 0, 
        bottom: 0
      }
    }, 
    '|~protoExpandoShellDeprecated|, 
    {
      __ntOptional: {
        empty: "EVAL", 
        firstLine: "EVAL", 
        indent: "NUMB", 
        labelStyle: "EVAL", 
        lineHeight: "NUMB", 
        textStyle: "EVAL", 
        viewLineSpacing: "NUMB", 
        writeProtected: "BOOL"
      }, 
      editWidth: 150, 
      __ntHelp: {
        closeEdit: "This is called when an entry is changed.", 
        editHeight: "Height of the expanded items.", 
        editWidth: "Width of the expanded items.", 
        empty: "The text to use for an item if it has no value.", 
        firstLine: "The line to expand when the view is opened.  Nil means do not expand lines", 
        indent: "The amount to indent the text value in each line from the left edge of the view.", 
        labelStyle: "Font for the labels shown to the left of each field.", 
        lineHeight: "Line height for the unexpanded items.", 
        lines: "Array of templates of items to show in the expando, of type protoTextExpando, protoDateExpando or protoPhoneExpando.", 
        numLines: "Number of lines in the lines array that should be displayed.", 
        textStyle: "Font for the expanded text.", 
        writeProtected: "If true the expando items are read only."
      }, 
      indent: 100, 
      viewLineSpacing: 20, 
      ExpandKeys: "func()\rbegin\r\tinherited:?ExpandKeys();\t\t// this method is defined internally\rend", 
      ViewDrawScript: "func()\rbegin\r\tinherited:?viewDrawScript();\t\t// this method is defined internally\rend", 
      firstLine: "0", 
      lines: "[\t{\t_proto: protoTextExpando,\r\t\tlabel: \"Override\",\r\t\tpath: 'deleteThis,\r\t\tentryFlags: vVisible  + vAnythingAllowed },\r\t{\t_proto: protoTextExpando,\r\t\tlabel: \"These\",\r\t\tpath: 'deleteThis,\r\t\tentryFlags: vVisible  + vAnythingAllowed } ]", 
      editHeight: 50, 
      viewFlags: 513, 
      labelStyle: "ROM_fontSystem9Bold", 
      _proto: 175, 
      lineHeight: 16, 
      FlushEdits: "func()\rbegin\rend", 
      numLines: 2, 
      empty: "\"--\"", 
      writeProtected: NIL, 
      deleteThis: "\"delete\"", 
      textStyle: "userFont12", 
      ViewSetupFormScript: "func()\rbegin\r\tself.target := self;\r\t//call this after setting up the lines array\r\tinherited:?viewSetupFormScript()\rend", 
      ExpandNone: "func()\rbegin\r\tinherited:?ExpandNone();\t\t// this method is defined internally\rend", 
      ViewClickScript: "func(unit)\rbegin\r\tinherited:?viewClickScript(unit);\t\t// this method is defined internally\r\t// Return true if click has been completely handled, nil otherwise\rend", 
      ExpandLine: "func()\rbegin\r\tinherited:?ExpandLine();\t\t// this method is defined internally\rend", 
      closeEdit: "func(closedView)\rbegin\r\tnil\rend", 
      ViewSetupChildrenScript: "func()\rbegin\r\tinherited:?viewSetupChildrenScript();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        closeEdit: "SCPT", 
        deleteThis: "EVAL", 
        editHeight: "NUMB", 
        editWidth: "NUMB", 
        FlushEdits: "SCPT", 
        lines: "EVAL", 
        numLines: "NUMB", 
        viewBounds: "RECT", 
        ViewSetupFormScript: "SCPT"
      }
    }, 
    '|~protoTextExpandoDeprecated|, 
    {
      __ntOptional: {
        entryFlags: "NUMB", 
        indent: "INTG", 
        keyboard: "EVAL", 
        label: "EVAL", 
        labelActionScript: "SCPT", 
        labelClick: "SCPT", 
        labelCommands: "EVAL", 
        labelFont: "EVAL", 
        Setup1: "SCPT", 
        Setup2: "SCPT", 
        specialClass: "EVAL", 
        viewLineSpacing: "NUMB"
      }, 
      path: "nil", 
      entryFlags: 14849, 
      labelActionScript: "func(cmdIndex)\rbegin\rend", 
      __ntHelp: {
        entryFlags: "Used to set the type of input the entry line will accept.", 
        indent: "The distance from the left edge of the view for the dotted line to begin.", 
        keyboard: "A symbol naming a keyboard to be used when the edit field is expanded.", 
        label: "The label text for the edit field.", 
        labelActionScript: "This is called when an item is chosen from the label popup.  The item index is passed into this method.", 
        labelClick: "This is called when the user taps in the label.", 
        labelCommands: "If this slot is supplied, then the pop-up feature is activated.  Specify an array of strings to use as popup items.", 
        labelFont: "Font to display the label in.", 
        path: "A symbol that is a path expression identifying the slot in the target frame from which the initial value for the field is gotten, and in which the final value is to be stored.", 
        Setup1: "This method should return the text for the field label whenever the collapsed view of this field is displayed.  It is passed the contents of the target slot of the enclosing protoExpandoShell.", 
        Setup2: "This method should return the text for the field contents whenever the collapsed view of this field is displayed.  It is passed the contents of the target slot of the enclosing protoExpandoShell.", 
        specialClass: "A symbol used to set the subclass of the string stored in the target path."
      }, 
      indent: 0, 
      Setup1: "func(target)\rbegin\r\tlabel\t// return the label for the field when collapsed\rend", 
      viewFormat: 208897, 
      viewLineSpacing: 20, 
      specialClass: "nil", 
      viewFlags: 513, 
      _proto: 227, 
      keyboard: "'alphaKeyboard", 
      label: "", 
      labelCommands: "nil", 
      textSetup: "func()\rbegin\r\tinherited:?TextSetup();\t\t// this method is defined internally\rend", 
      __ntAncestor: 'protoLabelInputLine, 
      textChanged: "func()\rbegin\r\tinherited:?TextChanged();\t\t// this method is defined internally\rend", 
      Setup2: "func(target)\rbegin\r\ttarget.(path)\t// return the text to be displayed for the field value when collapsed\rend", 
      labelFont: "ROM_fontSystem9Bold", 
      __ntRequired: {
        path: "EVAL", 
        viewBounds: "RECT"
      }
    }, 
    '|~protoDateExpandoDeprecated|, 
    {
      _proto: 170, 
      Date: "GetDateStringSpec([[kElementYear, kFormatNumeric],\r\t\t\t\t\t\t[kElementMonth, kFormatNumeric],\r\t\t\t\t\t\t[kElementDay, kFormatNumeric]])", 
      entryFlags: 526849, 
      keyboard: "'dateKeyboard", 
      label: "", 
      path: "nil", 
      textSetup: "func()\rbegin\r\tinherited:?TextSetup();\t\t// this method is defined internally\rend", 
      textChanged: "func()\rbegin\r\tinherited:?TextChanged();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        viewBounds: "RECT", 
        path: "EVAL"
      }, 
      __ntOptional: {
        entryFlags: "NUMB", 
        Date: "EVAL", 
        keyboard: "EVAL", 
        label: "EVAL", 
        labelCommands: "EVAL", 
        Setup1: "SCPT", 
        Setup2: "SCPT", 
        specialClass: "EVAL"
      }, 
      __ntHelp: {
        Date: "What elements should be included in the date and how it should be formatted.", 
        label: "The label text for the input line.", 
        labelCommands: "If this slot is supplied, then the pop-up feature is activated.  Specify an array of strings to use as popup items.", 
        path: "A symbol that is a path expression identifying the slot in the target frame from which the initial value for the field is gotten, and in which the final value is to be stored. The specified slot should contain a string which is a date."
      }
    }, 
    '|~protoPhoneExpandoDeprecated|, 
    {
      _proto: 194, 
      __ntAncestor: 'protoLabelInputLine, 
      labelActionScript: "func()\rbegin\r\tinherited:?LabelActionScript();\t\t// this method is defined internally\rend", 
      textChanged: "func()\rbegin\r\tinherited:?TextChanged();\t\t// this method is defined internally\rend", 
      textSetup: "func()\rbegin\r\tinherited:?TextSetup();\t\t// this method is defined internally\rend", 
      entryFlags: 264705, 
      keyboard: "'phoneKeyboard", 
      labelCommands: "[\"Phone\", \"Home\", \"Work\", \"Fax\", \"Car\", \"Beeper\", \"Mobile\", \"Other\"]", 
      labelSymbols: "'[phone, homePhone, workPhone, faxPhone, carPhone, beeperPhone, mobilePhone, otherPhone]", 
      path: "nil", 
      phoneIndex: 0, 
      Setup2: "func(target)\rbegin\r\ttarget.(path)[phoneIndex]\t// return the text to be displayed for the field value when collapsed\rend", 
      __ntRequired: {
        viewBounds: "RECT", 
        path: "EVAL"
      }, 
      __ntOptional: {
        entryFlags: "NUMB", 
        keyboard: "EVAL", 
        labelCommands: "EVAL", 
        labelSymbols: "EVAL", 
        phoneIndex: "NUMB", 
        Setup2: "SCPT"
      }, 
      __ntHelp: {
        keyboard: "A symbol naming a keyboard to be used when the edit field is expanded.", 
        labelCommands: "The choices to appear on the picker.", 
        labelSymbols: "An array of symbols that are used to indentify phone numbers stored in the target slot. These should be subclasses of the 'string class.", 
        path: "A symbol that is a path expression identifying the slot in the target frame from which the initial value for the field is gotten, and in which the final value is to be stored. The specified slot should contain an array of phone number strings.", 
        phoneIndex: "An index into the array of phone strings in the slot referenced by target.(path)", 
        Setup2: "This method should return the text for the field contents whenever the collapsed view of this field is displayed.  It is passed the contents of the target slot of the enclosing protoExpandoShell."
      }
    }, 
    'protoFilingButton, 
    {
      _proto: 176, 
      __ntAncestor: 'protoPictureButton, 
      icon: {
        bits: <Binary, class "bits", length 60>, 
        bounds: {
          top: 0, 
          left: 0, 
          bottom: 11, 
          right: 15
        }
      }, 
      viewJustify: 2054, 
      viewBounds: {
        left: -23, 
        top: 2, 
        right: -6, 
        bottom: 15
      }, 
      viewFormat: 67109457, 
      buttonClickScript: "func()\rbegin\r\tinherited:?buttonClickScript();\t\t// this method is defined internally\rend", 
      Update: "func()\rbegin\r\tinherited:?Update();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        viewBounds: "RECT"
      }
    }, 
    'protoLabelPicker, 
    {
      __ntOptional: {
        indent: "NUMB", 
        labelActionScript: "SCPT", 
        pickerSetup: "SCPT", 
        textChanged: "SCPT", 
        textSetup: "SCPT", 
        viewFont: "EVAL"
      }, 
      labelActionScript: "func(cmd)\rbegin\rend", 
      __ntHelp: {
        indent: "The distance from the left edge of the text label to the picker.", 
        labelActionScript: "This is called when an item is chosen from the label popup.  The item index is passed into this method.", 
        labelCommands: "If this slot is supplied, then the pop-up feature is activated.  Specify an array of strings to use as popup items.", 
        pickerSetup: "Return true if the label popup should happen.", 
        text: "The text for the label.", 
        textChanged: "Called when the entry text is changed.", 
        textSetup: "Returns default text for the entry area.", 
        viewFont: "The font for the text label."
      }, 
      indent: 0, 
      viewFormat: 50397184, 
      _proto: 190, 
      pickerSetup: "func()\rbegin\r\ttrue\t\t// the popup should happen\rend", 
      viewJustify: 8388608, 
      height: "nil", 
      PickCancelledScript: "func()\rbegin\r\tinherited:?pickCancelledScript();\t\t// this method is defined internally\rend", 
      labelCommands: "[\"item 1\",\"item 2\"]", 
      textSetup: "func() // return first one as default\r\tif labelCommands then\r\t\tlabelCommands[0]\r\telse \"\"", 
      __ntAncestor: 'protoStaticText, 
      textChanged: "func()\rbegin\rend", 
      ViewClickScript: "func(unit)\rbegin\r\tinherited:?viewClickScript(unit);\t\t// this method is defined internally\r\t// Return true if click has been completely handled, nil otherwise\rend", 
      ViewHiliteScript: "func()\rbegin\r\tinherited:?viewHiliteScript();\t\t// this method is defined internally\rend", 
      PickActionScript: "func(itemSelected)\rbegin\r\tinherited:?pickActionScript(itemSelected);\t\t// this method is defined internally\rend", 
      text: "\"Label\"", 
      viewFont: "ROM_fontSystem9Bold", 
      __ntRequired: {
        labelCommands: "EVAL", 
        text: "EVAL", 
        viewBounds: "RECT"
      }
    }, 
    'protoRollBrowser, 
    {
      _proto: 207, 
      __ntAncestor: 'protoApp, 
      rollItems: "[{_proto: protoRollItem,\r\theight: 50,\r\toverview: \"Overview of 1\",\r\tviewBounds: {left: 0, top: 0, bottom: 50, right: 0},\r\tviewChildren: [{_proto: protoTextButton,\r\t\t\t\t  viewBounds: {left: 5, top: 5, right: -5, bottom: -5},\r\t\t\t\t  viewJustify: vjParentFullH + vjParentFullV,\r\t\t\t\t  buttonClickScript: func()\r\t\t\t\t  begin\r\t\t\t\t  \tPlaySound(ROM_funBeep)\r\t\t\t\t  end,\r\t\t\t\t  text: \"Play funBeep\"}]},\r\r\t{_proto: protoRollItem,\r\theight: 50,\r\toverview: \"Overview of 2\",\r\tviewBounds: {left: 0, top: 0, bottom: 50, right: 0},\r\tviewChildren: [{_proto: protoTextButton,\r\t\t\t\t  viewBounds: {left: 5, top: 5, right: -5, bottom: -5},\r\t\t\t\t  buttonClickScript: func()\r\t\t\t\t  begin\r\t\t\t\t  \tPlaySound(ROM_bootSound)\r\t\t\t\t  end,\r\t\t\t\t  viewJustify: vjParentFullH + vjParentFullV,\r\t\t\t\t  text: \"Play bootSound\"}]}]", 
      declareSelf: "'base", 
      rollCollapsed: TRUE, 
      rollIndex: 0, 
      title: "\"RollBrowser\"", 
      viewFormat: 328017, 
      viewJustify: 16, 
      __ntRequired: {
        rollItems: "EVAL", 
        title: "EVAL", 
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        rollIndex: "INTG", 
        rollCollapsed: "BOOL", 
        titleHeight: "NUMB"
      }, 
      __ntHelp: {
        title: "A title for the rollBrowser based application.", 
        rollIndex: "Start displaying rollItems from this index, if the browser is initially expanded.", 
        rollItems: "An array of items to be displayed, all of type protoRollItem.", 
        rollCollapsed: "Specifies if roll starts out in overview or not."
      }
    }, 
    'protoRoll, 
    {
      _proto: 206, 
      items: "[{_proto: protoRollItem,\r\theight: 50,\r\toverview: \"Overview of 1\",\r\tviewBounds: {left: 0, top: 0, bottom: 50, right: 0},\r\tviewChildren: [{_proto: protoTextButton,\r\t\t\t\t  viewBounds: {left: 5, top: 5, right: -5, bottom: -5},\r\t\t\t\t  viewJustify: vjParentFullH + vjParentFullV,\r\t\t\t\t  buttonClickScript: func()\r\t\t\t\t  begin\r\t\t\t\t  \tPlaySound(ROM_funBeep)\r\t\t\t\t  end,\r\t\t\t\t  text: \"Play funBeep\"}]},\r\r\t{_proto: protoRollItem,\r\theight: 50,\r\toverview: \"Overview of 2\",\r\tviewBounds: {left: 0, top: 0, bottom: 50, right: 0},\r\tviewChildren: [{_proto: protoTextButton,\r\t\t\t\t  viewBounds: {left: 5, top: 5, right: -5, bottom: -5},\r\t\t\t\t  buttonClickScript: func()\r\t\t\t\t  begin\r\t\t\t\t  \tPlaySound(ROM_bootSound)\r\t\t\t\t  end,\r\t\t\t\t  viewJustify: vjParentFullH + vjParentFullV,\r\t\t\t\t  text: \"Play bootSound\"}]}]", 
      GetOverview: "func()\rbegin\r\tinherited:?GetOverview();\t\t// this method is defined internally\rend", 
      ShowItem: "func()\rbegin\r\tinherited:?ShowItem();\t\t// this method is defined internally\rend", 
      allCollapsed: NIL, 
      declareSelf: "'roll", 
      index: 0, 
      viewFlags: 36, 
      ViewOverviewScript: "func()\rbegin\r\tinherited:?viewOverviewScript();\t\t// this method is defined internally\rend", 
      ViewScrollUpScript: "func()\rbegin\r\tinherited:?viewScrollUpScript();\t\t// this method is defined internally\rend", 
      ViewSetupChildrenScript: "func()\rbegin\r\tinherited:?viewSetupChildrenScript();\t\t// this method is defined internally\rend", 
      viewSrollDownScript: "func()\rbegin\r\tinherited:?viewSrollDownScript();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        items: "EVAL", 
        declareSelf: "EVAL", 
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        index: "INTG", 
        allCollapsed: "BOOL"
      }, 
      __ntHelp: {
        index: "Start showing expanded view from this element of the array of items.", 
        items: "Array of overview items.  Elements should be of type protoRollItem.", 
        allCollapsed: "Specifies if roll starts out in overview or not.", 
        declareSelf: "DeclareSelf to 'Roll to recieve scrolling and overview messages."
      }
    }, 
    'protoRollItem, 
    {
      _proto: 208, 
      height: 20, 
      overview: "\"Overview Text\"", 
      viewBounds: {
        top: 0, 
        left: 0, 
        bottom: 20, 
        right: 0
      }, 
      viewFlags: 1, 
      viewFormat: 337, 
      viewJustify: 8240, 
      __ntRequired: {
        height: "NUMB", 
        overview: "EVAL", 
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        viewJustify: "NUMB"
      }, 
      __ntHelp: {
        height: "The height of this item. Must be the same as the height of the view.", 
        overview: "The text to show in the overview.", 
        viewBounds: "Must be no taller than the height slot.", 
        viewJustify: "Must be at least sibling bottom V justified."
      }
    }, 
    'protoActionButton, 
    {
      _proto: 209, 
      __ntAncestor: 'protoPictureButton, 
      viewJustify: 38, 
      viewBounds: {
        left: -42, 
        top: 2, 
        right: -25, 
        bottom: 15
      }, 
      icon: {
        bits: <Binary, class "bits", length 60>, 
        bounds: {
          top: 0, 
          left: 0, 
          bottom: 11, 
          right: 13
        }
      }, 
      ViewClickScript: "func(unit)\rbegin\r\tinherited:?viewClickScript(unit);\t\t// this method is defined internally\r\t// Return true if click has been completely handled, nil otherwise\rend", 
      buttonClickScript: "func()\rbegin\r\tinherited:?buttonClickScript();\t\t// this method is defined internally\rend", 
      PickActionScript: "func(itemSelected)\rbegin\r\tinherited:?pickActionScript(itemSelected);\t\t// this method is defined internally\rend", 
      PickCancelledScript: "func()\rbegin\r\tinherited:?pickCancelledScript();\t\t// this method is defined internally\rend"
    }, 
    'protoTitle, 
    {
      _proto: 229, 
      title: "kAppName", 
      titleHeight: 12, 
      viewFlags: 3, 
      viewFont: "ROM_fontSystem10Bold", 
      viewFormat: 50331653, 
      viewJustify: 22, 
      viewTransferMode: 3, 
      __ntRequired: {
        title: "EVAL"
      }, 
      __ntOptional: {
        titleHeight: "INTG", 
        viewFont: "EVAL", 
        viewJustify: "NUMB", 
        titleIcon: "PICT"
      }, 
      __ntHelp: {
        title: "The text for the title.", 
        titleHeight: "Pixel height for the title. If nil, calculated from the font.", 
        titleIcon: "Picture to use for the title."
      }, 
      titleIcon: NIL
    }, 
    'protoTextList, 
    {
      _proto: 228, 
      DoScrollScript: "func(arg)\rbegin\r\t// This method is defined internally, so call inherited:?DoScrollScript()\rend", 
      DrawHilite: "func()\rbegin\r\tinherited:?DrawHilite();\t\t// this method is defined internally\rend", 
      GetViewHeight: "func(arg)\rbegin\r\treturn inherited:?GetViewHeight();\t\t// This method is defined internally\rend", 
      HiliteLine: "func(lineNum)\rbegin\r\t// This method is defined internally, so call inherited:?HiliteLine()\rend", 
      SetupList: "func()\rbegin\r\tinherited:?SetupList();\t\t// this method is defined internally\rend", 
      buttonClickScript: "func(textIndex)\rbegin\r\tprint(\"selected index \" & textIndex);\rend", 
      listItems: "nil", 
      ViewClickScript: "func(unit)\rbegin\r\tinherited:?viewClickScript(unit);\t\t// this method is defined internally\r\t// Return true if click has been completely handled, nil otherwise\rend", 
      viewFlags: 545, 
      viewFont: "ROM_fontSystem9", 
      viewFormat: 337, 
      viewLines: 6, 
      ViewScrollUpScript: "func()\rbegin\r\tinherited:?viewScrollUpScript();\t\t// this method is defined internally\rend", 
      ViewSetupFormScript: "func()\rbegin\r\tself.listItems := [\"one\", \"two\", \"three\"];\r\t:SetupList();\rend", 
      viewSrollDownScript: "func()\rbegin\r\tinherited:?viewSrollDownScript();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        buttonClickScript: "SCPT", 
        viewBounds: "RECT", 
        viewLines: "NUMB", 
        ViewSetupFormScript: "SCPT"
      }, 
      __ntOptional: {
        selection: "EVAL", 
        viewFont: "EVAL", 
        viewFormat: "NUMB"
      }, 
      __ntHelp: {
        buttonClickScript: "Called when the user chooses an item in the list.", 
        listItems: "Array of text items to show in the list. Must be set in viewSetupFormScript.", 
        selection: "The index of the item which is selected when the view opens.  If set to nil, nothing is hilighted.", 
        viewBounds: "Bottom will be setup by the :SetupList() call.", 
        viewLines: "Number of text items to show on the screen at one time. Controls the height of the list view.", 
        ViewSetupFormScript: "Must initialize listItems and call :SetupList()."
      }
    }, 
    'protoTable, 
    {
      _proto: 223, 
      def: "protoTableDef", 
      scrollAmount: 1, 
      viewFormat: 337, 
      selectThisCell: "func(viewTapped)\rbegin\r\t// First you must call the inherited method.\r\tinherited:?selectThisCell(viewTapped);\r\t// Here you can do you own thing...\rend", 
      ViewSetupFormScript: "func()\rbegin\r\t// first clone off a copy of the table definition to avoid Read only errors\r\tdef := Clone(def);\r\t// setup the number of entries in the table\r\tdef.tabDown := 3;\r\t//add an array of text values for each of the table entries\r\tdef.tabValues := [\"item 1\", \"item 2\", \"item 3\"];\r\t// now make each entry as wide as the interior of the table\r\tdef.tabWidths := (viewBounds.right - viewBounds.left) - 2 ;\rend", 
      ViewScrollDownScript: "func()\rbegin\r\tinherited:?ViewScrollDownScript();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        def: "EVAL", 
        ViewSetupFormScript: "SCPT", 
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        ViewScrollDownScript: "SCPT", 
        scrollAmount: "NUMB", 
        ViewScrollUpScript: "SCPT"
      }, 
      __ntHelp: {
        def: "The table definition, based on protoTableDef.", 
        ViewSetupFormScript: "This script should setup the table definition.", 
        scrollAmount: "Number of rows the table scrolls each tap."
      }
    }, 
    'protoTableEntry, 
    {
      _proto: 225, 
      text: "\"Text\"", 
      viewFlags: 515, 
      viewFormat: 0, 
      viewJustify: 8388612, 
      viewTransferMode: 1, 
      __ntRequired: {
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        text: "EVAL"
      }
    }, 
    'protoSetClock, 
    {
      _proto: 210, 
      hours: 14, 
      minutes: 10, 
      timeChanged: "func()\rbegin\r\t// do this so the old hands are erased...\r\tself:Dirty();\r\t// this is just for fun, put your own stuff here...\r\tprint(\"H:\" && hours && \"M:\" && minutes);\rend", 
      viewBounds: {
        top: 0, 
        left: 0, 
        bottom: 64, 
        right: 64
      }, 
      viewFlags: 1537, 
      viewFormat: 0, 
      viewJustify: 0, 
      __ntRequired: {
        hours: "NUMB", 
        minutes: "NUMB", 
        timeChanged: "SCPT", 
        viewBounds: "RECT"
      }, 
      __ntHelp: {
        hours: "The current hours setting for the clock.", 
        minutes: "The current minutes setting for the clock.", 
        timeChanged: "Called when the user draws in the clock. Should dirty the clock."
      }
    }, 
    'protoPicker, 
    {
      __ntOptional: {
        GetItemMark: "SCPT", 
        SetItemMark: "SCPT", 
        callbackContext: "EVAL", 
        PickActionScript: "SCPT", 
        pickAutoClose: "BOOL", 
        pickBottomMargin: "NUMB", 
        PickCancelledScript: "SCPT", 
        pickItemsMarkable: "BOOL", 
        pickLeftMargin: "NUMB", 
        pickMarkWidth: "NUMB", 
        pickRightMargin: "NUMB", 
        pickTextItemHeight: "NUMB", 
        pickTopMargin: "NUMB", 
        viewFont: "EVAL"
      }, 
      pickRightMargin: 5, 
      ViewSetupDoneScript: "func()\rbegin\r\tinherited:?viewSetupDoneScript();\t\t// this method is defined internally\rend", 
      __ntHelp: {
        GetItemMark: "Gets the mark character for an item in the list.", 
        SetItemMark: "Sets the mark character for an item in the list.", 
        bounds: "The top-left point of this rectangle is used to position the picker within the parent.", 
        callbackContext: "View containing the pickActionScript. If nil, looks in self.", 
        PickActionScript: "Called when an item is selected from the pick list.", 
        pickAutoClose: "Determines if the picker automatically closes after a selection.", 
        pickBottomMargin: "Margin in pixels below each bitmap item in the list.", 
        PickCancelledScript: "Called if the pick list is cancelled by the user tapping outside of it.", 
        pickItems: "Array of items to pick, either strings, pick frames or bitmaps.", 
        pickItemsMarkable: "Determines if picker should reserve space for marks.", 
        pickLeftMargin: "Margin in pixels to the left of each bitmap item in the list.", 
        pickMarkWidth: "Number of pixels to reserve for marks.", 
        pickRightMargin: "Margin in pixels to the right of each bitmap item in the list.", 
        pickTextItemHeight: "Pixel height to reserve for each item in the pick list.", 
        pickTopMargin: "Margin in pixels above each bitmap item in the list."
      }, 
      viewFormat: 67109456, 
      pickTopMargin: 2, 
      top: 0, 
      pickItemsMarkable: NIL, 
      viewFlags: 578, 
      _proto: 195, 
      pickMarkWidth: 10, 
      GetItemMark: "func(index)\rbegin\r\tnil;\rend", 
      AfterScript: "// this proto uses the bounds instead of the viewbounds slot\rthisView.bounds := thisView.viewBounds;", 
      viewJustify: 6, 
      callbackContext: "nil", 
      PickCancelledScript: "func()\rbegin\rend", 
      bounds: {
        top: 0, 
        left: 0, 
        bottom: 0, 
        right: 0
      }, 
      pickLeftMargin: 4, 
      left: 0, 
      viewEffect: 393216, 
      PickActionScript: "func(actionCode)\rbegin\r\tnil;\rend", 
      pickBottomMargin: 2, 
      pickItems: "[\r\"one\",\r{item:\"two\", pickable: nil, mark:$•},\r\"three\",\r'pickseparator,\r{item:\"five\", pickable:true}\r]", 
      viewFont: "ROM_fontSystem10Bold", 
      pickAutoClose: TRUE, 
      SetItemMark: "func(index, char)\rbegin\r\tnil;\rend", 
      __ntRequired: {
        viewBounds: "RECT", 
        pickItems: "EVAL", 
        bounds: "RECT", 
        AfterScript: "EVAL"
      }, 
      pickTextItemHeight: 13
    }, 
    'protoPictIndexer, 
    {
      _proto: 196, 
      Hiliter: "func(arg)\rbegin\r\t// This method is defined internally, so call inherited:?Hiliter()\rend", 
      TrackPictHilite: "func(arg)\rbegin\r\t// This method is defined internally, so call inherited:?TrackPictHilite()\rend", 
      curIndex: 0, 
      icon: NIL, 
      iconBBox: {
        top: 0, 
        left: 0, 
        bottom: 0, 
        right: 0
      }, 
      indexClickScript: "func(currIndex)\rbegin\r\tnil;\rend", 
      numIndices: 0, 
      viewBounds: {
        left: 0, 
        top: -16, 
        right: 0, 
        bottom: 0
      }, 
      ViewClickScript: "func(unit)\rbegin\r\tinherited:?viewClickScript(unit);\t\t// this method is defined internally\r\t// Return true if click has been completely handled, nil otherwise\rend", 
      ViewDrawScript: "func()\rbegin\r\tinherited:?viewDrawScript();\t\t// this method is defined internally\rend", 
      viewFlags: 513, 
      viewFormat: 1, 
      viewJustify: 182, 
      ViewSetupDoneScript: "func()\rbegin\r\tinherited:?viewSetupDoneScript();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        icon: "PICT", 
        iconBBox: "RECT", 
        numIndices: "NUMB", 
        indexClickScript: "SCPT", 
        curIndex: "NUMB"
      }, 
      __ntHelp: {
        icon: "Picture to use for the indexer, if it is not 16 pixels high, you must change the viewbounds.", 
        iconBBox: "The bounding box of the icon starting from 0,0.", 
        numIndices: "The number of 'items' in the icon that the user can choose from.", 
        currIndex: "The index of the currently selected item.", 
        indexClickScript: "When the selection changes this method is passed the index of the chosen item."
      }
    }, 
    'protoDrawer, 
    {
      _proto: 173, 
      hideSound: "ROM_drawerclose", 
      showSound: "ROM_draweropen", 
      viewEffect: 133120, 
      viewFlags: 68, 
      viewFormat: 592, 
      __ntRequired: {
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        hideSound: "EVAL", 
        showSound: "EVAL"
      }, 
      __ntHelp: {
        hideSound: "The sound that plays when the drawer view is closed.", 
        showSound: "The sound that plays when the drawer view is opened."
      }
    }, 
    'protoPrintFormat, 
    {
      _proto: 200, 
      auxForm: "nil", 
      PrintNextPageScript: "func()\rbegin\r\tnil;\rend", 
      textscript: "'plainTextScript", 
      title: "\"Untitled\"", 
      viewBounds: {
        left: 60, 
        top: 0, 
        right: -60, 
        bottom: 0
      }, 
      viewFlags: 3, 
      ViewSetupChildrenScript: "func()\rbegin\r\tinherited:?viewSetupChildrenScript();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        PrintNextPageScript: "SCPT", 
        title: "EVAL", 
        symbol: "EVAL"
      }, 
      __ntOptional: {
        auxForm: "EVAL", 
        textscript: "SCPT", 
        ViewSetupChildrenScript: "SCPT", 
        version: "NUMB", 
        SetupItem: "SCPT", 
        GetCursorFormat: "SCPT", 
        usesCursors: "EVAL", 
        orientation: "EVAL", 
        margins: "RECT", 
        storeAlias: "EVAL", 
        sizeLimit: "NUMB", 
        storeCursors: "EVAL"
      }, 
      __ntHelp: {
        auxForm: "A template to get extra information for routing.", 
        PrintNextPageScript: "Sets up next page to print using, returns nil if no more pages.", 
        textscript: "Symbol of a script that is called to find the text representation of an object.", 
        viewBounds: "These are calculated by this proto and will be overwritten.", 
        title: "String to be displayed in protoFormatPicker", 
        version: "Integer identifying the version of this format", 
        SetupItem: "Used to initialize the item to be sent", 
        symbol: "Symbol that uniquely identifies a format from all others", 
        GetCursorFormat: "Returns a format or nil", 
        usesCursors: "if true, viewformats will be passed cursors", 
        orientation: "if set to 'landscape, print format used paper horizontally", 
        margins: "a bounds rectangle for the margins"
      }, 
      version: 0, 
      SetupItem: "func(item,tInfo)\rbegin\r\tInherited:?SetupItem(item,tInfo);\rend", 
      symbol: "'protoRoutingFormat", 
      usesCursors: "nil", 
      orientation: "'portrait", 
      margins: {
        top: 0, 
        left: 60, 
        bottom: 0, 
        right: 60
      }, 
      NextItem: "func()\rbegin\rend", 
      GetCursorFormat: "func(target)\rbegin\rend", 
      storeAlias: "nil", 
      sizeLimit: 256, 
      storeCursors: "true"
    }, 
    'protoKeyboard, 
    {
      _proto: 187, 
      __ntAncestor: 'protoFloater, 
      AfterScript: "// Set the saveBounds to the viewBounds since this is probably what you want\rthisView.saveBounds := Clone(thisView.viewBounds);", 
      freeze: TRUE, 
      saveBounds: {
        top: 0, 
        left: 0, 
        bottom: 100, 
        right: 100
      }, 
      ViewClickScript: "func(unit)\rbegin\r\tinherited:?viewClickScript(unit);\t\t// this method is defined internally\r\t// Return true if click has been completely handled, nil otherwise\rend", 
      viewFlags: 64, 
      viewJustify: 16, 
      ViewSetupFormScript: "func()\rbegin\r\tinherited:?viewSetupFormScript();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        AfterScript: "EVAL", 
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        freeze: "BOOL", 
        saveBounds: "RECT"
      }, 
      __ntHelp: {
        freeze: "if true, prevents the system from moving the keyboard to prevent obscuring.", 
        saveBounds: "Size and location where you want the Keyboard view to appear."
      }
    }, 
    'protoKeypad, 
    {
      _proto: 188, 
      keyDefinitions: "[[keyVUnit, keyVUnit,\r\t\t\t\t$a,  $a,\t\tkeyHUnit + keyVUnit + keyFramed + keyAutoHilite + keyRoundingUnit*3,\r\t\t\t\t$b,  $b,\t\tkeyHUnit + keyVUnit + keyFramed + keyAutoHilite + keyRoundingUnit*3],\r [keyVUnit, keyVUnit,\r\t\t\t\t$c,  $c,\t\tkeyHUnit + keyVUnit + keyFramed + keyAutoHilite + keyRoundingUnit*3,\r\t\t\t\t$d,  $d,\t\tkeyHUnit + keyVUnit + keyFramed + keyAutoHilite + keyRoundingUnit*3]\r\t\t\t]", 
      keyArrayIndex: 0, 
      keyHighlightKeys: TRUE, 
      keyPressScript: "func(key)\rbegin\r\tprint(key);\rend", 
      keyReceiverView: "'viewFrontKey", 
      keyResultsAreKeycodes: NIL, 
      keySound: "nil", 
      viewFont: "ROM_fontSystem9Bold", 
      __ntRequired: {
        keyDefinitions: "EVAL", 
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        viewFont: "EVAL", 
        viewFormat: "NUMB", 
        keyArrayIndex: "INTG", 
        keyHighlightKeys: "BOOL", 
        keyResultsAreKeycodes: "BOOL", 
        keyReceiverView: "EVAL", 
        keySound: "EVAL", 
        keyPressScript: "SCPT"
      }, 
      __ntHelp: {
        keyArrayIndex: "Dynamically determine the array element to be used for a key legend or result.", 
        keyDefinitions: "An array of that defines the individual keys.", 
        keyHighlightKeys: "Array of keys to be highlighted on the keyboard.", 
        keyPressScript: "Called whenver a key is pressed.", 
        keyReceiverView: "View that key presses are sent to if no KeyPressScript is defined.", 
        keyResultsAreKeycodes: "If true, interpret integer results as key codes.", 
        keySound: "A sound frame that is played when a key is pressed."
      }
    }, 
    'protoLabeledBatteryGauge, 
    {
      _proto: 316, 
      ViewSetupDoneScript: "func()\rbegin\r\tinherited:?viewSetupDoneScript();\t\t// this method is defined internally\rend", 
      ViewSetupChildrenScript: "func()\rbegin\r\tinherited:?viewSetupChildrenScript();\t\t// this method is defined internally\rend", 
      ViewIdleScript: "func()\rbegin\r\tinherited:?viewIdleScript();\t\t// this method is defined internally\r\t300;\t\t// Return the number of milliseconds to delay or nil to end idling.\rend", 
      ReadBattery: "func()\rbegin\r\tinherited:?ReadBattery();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        viewBounds: "RECT"
      }
    }, 
    'protoRecToggle, 
    {
      _proto: 234, 
      viewFlags: 1, 
      viewFormat: 67109457, 
      viewBounds: {
        top: 2, 
        left: 25, 
        bottom: 15, 
        right: 43
      }
    }, 
    'newtApplication, 
    {
      prefsView: "// Template used for the prefs slip, use GetLayout here\rnil", 
      __ntOptional: {
        aboutInfo: "EVAL", 
        aboutView: "EVAL", 
        scrollingEndBehavior: "EVAL", 
        scrollingUpBehavior: "EVAL", 
        statusBarSlot: "EVAL", 
        prefsView: "EVAL", 
        theApp: "EVAL", 
        allViewDefs: "EVAL", 
        allDataDefs: "EVAL", 
        superSymbol: "EVAL", 
        doCardRouting: "EVAL", 
        dateFindSlot: "EVAL", 
        routeScripts: "EVAL", 
        labelsFilter: "EVAL"
      }, 
      allViewDefs: "// Required if your app supports stationery\rnil", 
      superSymbol: "// symbol to tie application with its stationery\rkAppSymbol", 
      __ntHelp: {
        appSymbol: "The application's unique symbol.", 
        title: "The title of your application.", 
        appObject: "Singular and plural of objects in the soup.", 
        appAll: "A string used in the folder tab to provide the \"All <items>\" option.", 
        aboutInfo: "Frame that specifies what to display in the About slip of the NewtApp.", 
        aboutView: "Template that is used for the About slip.", 
        allSoups: "Frame describing the application soups whose slots are instances of NewtSoup.", 
        allLayouts: "Frame whose slots contain file names for at least a default and overview layout.", 
        scrollingEndBehavior: "Action when scrolling wraps. One of 'beepAndWrap, 'wrap, 'stop or 'beepAndStop", 
        scrollingUpBehavior: "Show 'top or 'bottom if scrolling up to an entry that is larger than the screen", 
        statusBarSlot: "Symbol that is the declared name of the newtStatusBar. Used for layouts to specify what buttons appear in the bar.", 
        prefsView: "Template for a preferences slip that is opened from the Newt info button.", 
        theApp: "Used by entry views and the default info box to set a reference to the base newtApp.", 
        allDataDefs: "Frame whose slots are instances of data definitions to be registered with this app. Only required if your application supports stationery.", 
        allViewDefs: "Frame whose slots are instances of stationeries to be registered with this app. Only required if your application supports stationery.", 
        superSymbol: "Symbol used to tie the newtApplication with its stationery", 
        dateFindSlot: "Enables date find. Set to a path expression into your soup entries that is a slot with a date value", 
        routeScripts: "Override to change the action items from the default Delete and Duplicate.", 
        labelsFilter: "Used by showBar to store filing settings."
      }, 
      scrollingEndBehavior: "// What to do when scrolling wraps: beepAndWrap, 'wrap, 'stop or 'beepAndStop\r'beepAndWrap", 
      theApp: "// Almost always set to the newtApp base view\rnil", 
      appAll: "\"All Items\"", 
      scrollingUpBehavior: "// What to show if scrolling to entry that is larger than the screen\r// can be 'top or 'bottom\r'bottom", 
      viewFlags: 5, 
      doCardRouting: "// True to do moving to/from card from filing slip\rnil", 
      appObject: "[\"Item\", \"Items\"]", 
      dateFindSlot: "// A path expression into a soup entry that contains a date for use in find\rnil", 
      _proto: 398, 
      labelsFilter: "nil", 
      allSoups: "{\r\tmySoup: {\r\t\t_proto: newtSoup,\r\t\tsoupName: \"SoupName:SIG\",\r\t\tsoupIndicies: [],\r\t\tsoupQuery: {},\r\t},\r}", 
      ViewScrollDownScript: "func()\rbegin\r\tinherited:?ViewScrollDownScript();\t\t// this method is defined internally\rend", 
      ViewOverviewScript: "func()\rbegin\r\tinherited:?ViewOverviewScript();\t\t// this method is defined internally\rend", 
      appSymbol: "kAppSymbol", 
      title: "kAppName", 
      allDataDefs: "// Required if your app supports stationery\rnil", 
      aboutInfo: "{\r\ttagLine: \"NiftyApp\uffffff9920\uby Me\",\r\tversion: \"0.1d1\",\r\tcopyright: \"Copyright ©1996 By Me\",\r\ttrademarks: \"myTM\",\r}", 
      ViewScrollUpScript: "func()\rbegin\r\tinherited:?ViewScrollUpScript();\t\t// this method is defined internally\rend", 
      __ntAncestor: 'protoDragger, 
      routeScripts: "//newtApp supplied defaults\r[\r\t{\r\t\tGetTitle: func(item) if item then \"Duplicate\",\r\t\ticon: ROM_routeDuplicateIcon,\r\t\trouteScript: 'newtDuplicateScript,\r\t},\r\t{\r\t\tGetTitle: func(item) if item then \"Delete\",\r\t\ticon: ROM_routeDeleteIcon,\r\t\trouteScript: 'newtDeleteScript,\r\t},\r]", 
      statusBarSlot: "// Symbol that is the declared name of the newtStatusBar\r'status", 
      ViewClickScript: "func()\rbegin\r\tinherited:?ViewClickScript();\t\t// this method is defined internally\rend", 
      allLayouts: "// Must contain a default and overview layout.\r// Use GetLayout to include a layout\r{\r\tdefault: nil,\r\toverview: nil,\r}", 
      aboutView: "// Template used for the about slip, use GetLayout here\rnil", 
      ViewSetupChildrenScript: "func()\rbegin\r\tinherited:?ViewSetupChildrenScript();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        appSymbol: "EVAL", 
        title: "EVAL", 
        appObject: "EVAL", 
        appAll: "EVAL", 
        allSoups: "EVAL", 
        allLayouts: "EVAL", 
        viewBounds: "RECT", 
        viewFlags: "NUMB"
      }
    }, 
    'newtAZTabs, 
    {
      _proto: 430, 
      __ntAncestor: 'protoPictIndexer, 
      PickLetterScript: "func(theLetter)\rbegin\r\tInherited:?PickLetterScript(theLetter);\r\tprint(theLetter);\rend", 
      RetargetNotify: "func()\rbegin\rend", 
      ViewSetupDoneScript: "func()\rbegin\r\tinherited:?ViewSetupDoneScript();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        PickLetterScript: "SCPT"
      }, 
      __ntOptional: {
        RetargetNotify: "SCPT", 
        ViewSetupDoneScript: "SCPT"
      }
    }, 
    'newtStationery, 
    {
      _proto: 451, 
      symbol: "nil", 
      name: "\"\"", 
      description: "\"\"", 
      superSymbol: "nil // set to appropriate project-wide constant", 
      version: 0, 
      height: 0, 
      __ntRequired: {
        symbol: "EVAL", 
        name: "EVAL", 
        description: "EVAL", 
        superSymbol: "EVAL"
      }, 
      __ntOptional: {
        version: "NUMB", 
        height: "NUMB", 
        icon: "PICT"
      }, 
      __ntHelp: {
        symbol: "Data type symbol (name of slot in allDataDefs).", 
        name: "User-visible name of data type.", 
        description: "User-visible description of data type.", 
        icon: "Icon representing data type (that appears in New button).", 
        version: "Version number.", 
        height: "Used by roll types."
      }
    }, 
    'newtStationeryPopupButton, 
    {
      _proto: 812, 
      __ntAncestor: 'protoPopupButton, 
      text: "\" Button\"", 
      form: "'viewDef", 
      symbols: "nil", 
      types: "['viewer, 'editor]", 
      sorter: "'|str<|", 
      shortCircuit: TRUE, 
      SetUpStatArray: "func()\rbegin\r\tGetDefs(form, symbols, types);\rend", 
      StatScript: "func(stationeryItem)\rbegin\r\t// create new stationery...\rend", 
      buttonClickScript: "// **** This method is defined internally and should not be overriden.****", 
      PickActionScript: "// **** This method is defined internally and should not be overriden.****", 
      PickCancelledScript: "// **** This method is defined internally and should not be overriden.****", 
      viewJustify: 17414, 
      viewBounds: {
        top: 0, 
        left: 7, 
        bottom: 13, 
        right: 45
      }, 
      __ntRequired: {
        text: "EVAL", 
        form: "EVAL", 
        types: "EVAL", 
        symbols: "EVAL", 
        StatScript: "SCPT"
      }, 
      __ntOptional: {
        sorter: "EVAL", 
        shortCircuit: "BOOL", 
        SetUpStatArray: "SCPT"
      }, 
      __ntHelp: {
        text: "String to display as the button title", 
        form: "What form of stationery to display: 'viewDef or 'dataDef", 
        types: "Array specifying types of viewDefs are included e.g. ['viewer, 'editor, ...]", 
        symbols: "Array of stationery to display or NIL for all stationery", 
        sorter: "NIL for no sorting of the list or a symbol for the type of sort e.g. '|str<|", 
        shortCircuit: "Bypass the picker if only one item", 
        SetUpStatArray: "Return a list of stationery to display in the picker", 
        StatScript: "Called when an item is chosen from the stationery picker"
      }
    }, 
    'newtNewStationeryButton, 
    {
      _proto: 813, 
      __ntAncestor: 'newtStationeryPopupButton, 
      viewBounds: {
        top: 0, 
        left: 7, 
        bottom: 13, 
        right: 45
      }, 
      form: "'dataDef", 
      SetUpStatArray: "func()\rbegin\r\tinherited:?SetUpStatArray();\t\t// this method is defined internally\rend", 
      StatScript: "func(stationeryItem)\rbegin\r\tinherited:?StatScript(stationeryItem);\rend", 
      __ntRequired: {
      }, 
      __ntOptional: {
        SetUpStatArray: "SCPT", 
        StatScript: "SCPT", 
        shortCircuit: "BOOL", 
        sorter: "EVAL"
      }, 
      __ntHelp: {
      }
    }, 
    'newtShowStationeryButton, 
    {
      _proto: 814, 
      __ntAncestor: 'newtStationeryPopupButton, 
      text: "\" Show\"", 
      viewBounds: {
        top: 0, 
        left: 7, 
        bottom: 13, 
        right: 45
      }, 
      __ntRequired: {
      }, 
      __ntOptional: {
        SetUpStatArray: "SCPT", 
        StatScript: "SCPT", 
        shortCircuit: "BOOL", 
        types: "EVAL", 
        sorter: "EVAL"
      }, 
      __ntHelp: {
      }
    }, 
    'newtRollShowStationeryButton, 
    {
      _proto: 815, 
      __ntAncestor: 'newtShowStationeryButton, 
      viewBounds: {
        top: 0, 
        left: 7, 
        bottom: 13, 
        right: 45
      }, 
      __ntRequired: {
      }, 
      __ntOptional: {
        SetUpStatArray: "SCPT", 
        StatScript: "SCPT", 
        shortCircuit: "BOOL", 
        types: "EVAL", 
        sorter: "EVAL"
      }, 
      __ntHelp: {
      }
    }, 
    'newtEntryShowStationeryButton, 
    {
      _proto: 816, 
      __ntAncestor: 'newtShowStationeryButton, 
      viewBounds: {
        top: 0, 
        left: 7, 
        bottom: 13, 
        right: 45
      }, 
      shortCircuit: NIL, 
      __ntRequired: {
      }, 
      __ntOptional: {
        SetUpStatArray: "SCPT", 
        StatScript: "SCPT", 
        shortCircuit: "BOOL", 
        types: "EVAL", 
        sorter: "EVAL"
      }, 
      __ntHelp: {
      }
    }, 
    'protoMapTextPicker, 
    {
      _proto: 631, 
      __ntAncestor: 'protoTextPicker, 
      viewBounds: {
        top: 0, 
        left: 0, 
        bottom: 18, 
        right: 0
      }, 
      label: "\"Text\"", 
      params: "{spots: MP_CountrySoupName, result: 'name}", 
      Popit: "func(x)\rbegin\r\tInherited:?Popit(x);\rend", 
      textSetup: "func()\rbegin\r\tinherited:?TextSetup();\t\t// this method is defined internally\rend", 
      PickActionScript: "func(itemSelected)\rbegin\r\tinherited:?pickActionScript(itemSelected);\t\t// this method is defined internally\rend", 
      __ntRequired: {
        viewBounds: "RECT", 
        label: "EVAL", 
        params: "EVAL"
      }, 
      __ntOptional: {
        Popit: "SCPT", 
        textSetup: "SCPT", 
        PickActionScript: "SCPT"
      }, 
      __ntHelp: {
      }
    }, 
    'protoCountryTextPicker, 
    {
      _proto: 632, 
      __ntAncestor: 'protoMapTextPicker, 
      viewBounds: {
        top: 0, 
        left: 0, 
        bottom: 18, 
        right: 0
      }, 
      label: "\"Text\"", 
      params: "{spots: MP_CountrySoupName, result: 'name}", 
      Popit: "func(x)\rbegin\r\tInherited:?Popit(x);\rend", 
      textSetup: "func()\rbegin\r\tinherited:?TextSetup();\t\t// this method is defined internally\rend", 
      PickActionScript: "func(itemSelected)\rbegin\r\tinherited:?pickActionScript(itemSelected);\t\t// this method is defined internally\rend", 
      __ntRequired: {
        viewBounds: "RECT", 
        label: "EVAL", 
        params: "EVAL"
      }, 
      __ntOptional: {
        Popit: "SCPT", 
        textSetup: "SCPT", 
        PickActionScript: "SCPT"
      }, 
      __ntHelp: {
      }
    }, 
    'protoUSstatesTextPicker, 
    {
      _proto: 633, 
      __ntAncestor: 'protoMapTextPicker, 
      viewBounds: {
        top: 0, 
        left: 0, 
        bottom: 18, 
        right: 0
      }, 
      label: "\"Text\"", 
      params: "{spots: ROM_usstatesoupname, zoomer: 'northAmerica, result: 'name}", 
      Popit: "func(x)\rbegin\r\tInherited:?Popit(x);\rend", 
      textSetup: "func()\rbegin\r\tinherited:?TextSetup();\t\t// this method is defined internally\rend", 
      PickActionScript: "func(itemSelected)\rbegin\r\tinherited:?pickActionScript(itemSelected);\t\t// this method is defined internally\rend", 
      __ntRequired: {
        viewBounds: "RECT", 
        label: "EVAL", 
        params: "EVAL"
      }, 
      __ntOptional: {
        Popit: "SCPT", 
        textSetup: "SCPT", 
        PickActionScript: "SCPT"
      }, 
      __ntHelp: {
      }
    }, 
    'protoCitiesTextPicker, 
    {
      _proto: 634, 
      __ntAncestor: 'protoMapTextPicker, 
      params: "{spots: ROM_citysoupname, result: 'name}", 
      __ntRequired: {
        params: "EVAL"
      }, 
      __ntOptional: {
      }, 
      __ntHelp: {
      }
    }, 
    'protoTimeDeltaTextPicker, 
    {
      _proto: 522, 
      __ntAncestor: 'protoTextPicker, 
      viewBounds: {
        top: 0, 
        left: 0, 
        bottom: 18, 
        right: 0
      }, 
      label: "kPopChar & \"Offset\"", 
      Time: "nil", 
      rMonthDayStrSpec: 1, 
      minValue: 0, 
      Popit: "func(x)\rbegin\r\tInherited:?Popit(x);\rend", 
      textSetup: "func()\rbegin\r\tinherited:?TextSetup();\t\t// this method is defined internally\rend", 
      PickActionScript: "func(newTime)\rbegin\r\tInherited:?PickActionScript(newTime);\rend", 
      __ntRequired: {
        viewBounds: "RECT", 
        label: "EVAL", 
        Time: "EVAL"
      }, 
      __ntOptional: {
        rMonthDayStrSpec: "NUMB", 
        minValue: "NUMB", 
        Popit: "SCPT", 
        textSetup: "SCPT", 
        PickActionScript: "SCPT"
      }, 
      __ntHelp: {
      }
    }, 
    'protoLongLatTextPicker, 
    {
      _proto: 523, 
      __ntAncestor: 'protoTextPicker, 
      viewBounds: {
        top: 0, 
        left: 0, 
        bottom: 18, 
        right: 0
      }, 
      label: "kPopChar & \"Where\"", 
      longitude: 0, 
      latitude: 0, 
      worldClock: TRUE, 
      Popit: "func(x)\rbegin\r\tInherited:?Popit(x);\rend", 
      textSetup: "func()\rbegin\r\tinherited:?TextSetup();\t\t// this method is defined internally\rend", 
      PickActionScript: "func(long,lat)\rbegin\r\tInherited:?PickActionScript(long,lat);\rend", 
      __ntRequired: {
        viewBounds: "RECT", 
        label: "EVAL", 
        
      }, 
      __ntOptional: {
        Popit: "SCPT", 
        textSetup: "SCPT", 
        PickActionScript: "SCPT"
      }, 
      __ntHelp: {
        worldClock: "This slot must be true to correctly display the longitude and lattitude."
      }
    }, 
    'protoAMPMCluster, 
    {
      _proto: 379, 
      Time: "0", 
      __ntRequired: {
        viewBounds: "RECT", 
        Time: "EVAL"
      }, 
      __ntOptional: {
      }, 
      __ntHelp: {
      }
    }, 
    'protoTransportPrefs, 
    {
      _proto: 678, 
      viewBounds: {
        top: 0, 
        left: 0, 
        bottom: 100, 
        right: 200
      }, 
      viewJustify: 80, 
      title: "\"PrefsTitle\"", 
      appSymbol: "'aSymbol", 
      __ntRequired: {
        appSymbol: "EVAL", 
        title: "EVAL"
      }, 
      __ntOptional: {
      }, 
      __ntHelp: {
      }
    }, 
    'protoFullRouteSlip, 
    {
      _proto: 655, 
      PrepareToSend: "func(when)\rbegin\r\tlocal item := fields;\r\r\t// extract information from your 'recipient pickers' and add them to the item like:\r\t// item.toRef := myProtoAddressPicker.selected;\r\r\tinherited:?PrepareToSend(when); // inherited method submits item via :ContinueSend...\rend;", 
      bottomIndent: 20, 
      viewFormat: 592, 
      viewEffect: 18940928, 
      viewJustify: 112, 
      ViewSetupChildrenScript: "func()\rbegin\r\tinherited:?ViewSetupChildrenScript();\t\t// this method is defined internally\rend", 
      ViewDrawScript: 'GetSelected, 
      ViewHideScript: "func()\rbegin\r\tinherited:?ViewHideScript();\t\t// this method is defined internally\rend", 
      __ntRequired: {
        PrepareToSend: "SCPT"
      }, 
      __ntOptional: {
        bottomIndent: "NUMB", 
        ViewSetupChildrenScript: "SCPT"
      }, 
      __ntHelp: {
      }
    }, 
    'protoNumberPicker, 
    {
      _proto: 72, 
      viewBounds: {
        top: 51, 
        left: 20, 
        bottom: 75, 
        right: 180
      }, 
      declareSelf: "'digitBase", 
      viewClass: 76, 
      icon: NIL, 
      ViewSetupDoneScript: "func()\rbegin\r\tinherited:?ViewSetupDoneScript();\t\t// this method is defined internally\rend", 
      value: "nil", 
      maxValue: 14330, 
      minValue: 0, 
      hiOrd: 0, 
      PrepareForClick: "func()\rbegin\rend", 
      ClickDone: "func()\rbegin\rend", 
      __ntRequired: {
        value: "EVAL", 
        maxValue: "NUMB", 
        minValue: "NUMB"
      }, 
      __ntOptional: {
        viewFlags: "NUMB", 
        viewFormat: "NUMB", 
        viewBounds: "RECT", 
        declareSelf: "EVAL", 
        
      }, 
      __ntHelp: {
        value: "The initial and currently selected value.", 
        maxValue: "The maximum value in the list.", 
        minValue: "The minimum value in the list.", 
        
      }
    }
  ], 
  ViewClassArray: [
    'LinkedSubview, 
    {
      _proto: 'ntLinkedSubview, 
      __ntRequired: {
        viewBounds: "RECT"
      }
    }, 
    'clView, 
    {
      viewClass: 74, 
      viewFlags: 1, 
      viewFormat: 0, 
      __ntRequired: {
        viewBounds: "RECT", 
        viewFlags: "NUMB", 
        viewFormat: "NUMB"
      }
    }, 
    'clPictureView, 
    {
      viewClass: 76, 
      icon: NIL, 
      viewFlags: 1, 
      __ntRequired: {
        icon: "PICT", 
        viewBounds: "RECT", 
        viewFlags: "NUMB", 
        viewFormat: "NUMB"
      }
    }, 
    'clEditView, 
    {
      viewClass: 77, 
      viewFlags: 1, 
      viewFormat: 337, 
      viewLinePattern: "nil", 
      viewLineSpacing: 20, 
      __ntRequired: {
        viewFlags: "NUMB", 
        viewFormat: "NUMB", 
        viewBounds: "RECT"
      }, 
      __ntOptional: {
        viewLinePattern: "EVAL", 
        viewLineSpacing: "NUMB"
      }, 
      __ntHelp: {
        viewLinePattern: "A custom pattern used to draw lines in the view."
      }
    }, 
    'clParagraphView, 
    {
      viewClass: 81, 
      styles: "[4, simpleFont18]", 
      tabs: "[10, 20, 30, 40]", 
      text: "\"Text\"", 
      textFlags: "nil", 
      viewFlags: 1, 
      viewFont: "simpleFont18", 
      viewFormat: 12625, 
      viewJustify: 0, 
      viewLineSpacing: 20, 
      __ntRequired: {
        viewBounds: "RECT", 
        viewFlags: "NUMB", 
        viewFont: "EVAL", 
        viewFormat: "NUMB", 
        viewLineSpacing: "NUMB"
      }, 
      __ntOptional: {
        styles: "EVAL", 
        tabs: "EVAL", 
        text: "EVAL", 
        
      }, 
      __ntHelp: {
        styles: "An array of alternating run lengths and font information, if multiple font styles are used.", 
        tabs: "An array of up to 8 tab-stop positions, in pixels.", 
        text: "The text to display in the view"
      }
    }, 
    'clPolygonView, 
    {
      viewClass: 82, 
      viewFlags: 1, 
      viewFormat: 512, 
      points: "nil", 
      timeStamp: "nil", 
      ViewSetupFormScript: "func()\rbegin\r\tself.points := arrayToPoints([10, 5, 0, 0, 0, 18, 18, 18, 18, 0, 0, 0]);\rend", 
      __ntRequired: {
        viewBounds: "RECT", 
        viewFormat: "NUMB", 
        viewFlags: "NUMB", 
        
      }, 
      __ntOptional: {
        
      }, 
      __ntHelp: {
        
      }
    }, 
    'clRemoteView, 
    {
      viewClass: 88, 
      __ntRequired: {
        viewBounds: "RECT", 
        viewFlags: "NUMB", 
        viewFormat: "NUMB"
      }
    }
  ], 
  ScriptSlots: {
    ViewDropRemoveScript: {
      value: "func(dragRef)\rbegin\r\t//return true or nil\rend", 
      __ntHelp: "Returning true means that this method handled removing the item."
    }, 
    GetTargetInfo: {
      value: "func(reason) // reason could be 'routing, 'filing, or other symbols.\r\tbegin\r\t\t{target: target,\r\t\t targetView: base, // return targetView or base view\r\t\t}\r\tend", 
      __ntHelp: "Returns a target information frame required by system services such as Filing and Routing."
    }, 
    ViewSetupDoneScript: {
      value: "func()\rbegin\rend", 
      __ntHelp: "This is called after all of the child views are instantiated."
    }, 
    ViewGestureScript: {
      value: "func(unit, gestureKind)\rbegin\r\t// Return true if gesture has been completely handled, nil otherwise\rend", 
      __ntHelp: "This is called when the user writes a gesture inside the view."
    }, 
    ViewQuitScript: {
      value: "// must return the value of inherited:?ViewQuitScript()\rfunc()\rbegin\r\tinherited:?ViewQuitScript();\rend", 
      __ntHelp: "This is called just before the view (and all of its children) is disposed."
    }, 
    ViewDrawScript: {
      value: "func()\rbegin\rend", 
      __ntHelp: "This is called after the view system does its drawing."
    }, 
    ViewDropDoneScript: {
      value: "func()\rbegin\rend", 
      __ntHelp: "Sent at the very end each drag and drop to let the destination view know that all specified items have been dropped or moved."
    }, 
    ViewDrawDragDataScript: {
      value: "func(bounds)\rbegin\r\t//return true or nil\rend", 
      __ntHelp: "This method should draw the image that will be dragged."
    }, 
    ViewGetDropDataScript: {
      value: "func(dragType,dragRef)\rbegin\r\t//return a frame containing the actual data to be dropped\rend", 
      __ntHelp: "Returns a frame containing the actual data to be dropped into the destination view."
    }, 
    ViewStrokeScript: {
      value: "func(unit)\rbegin\r\t// Return true if stroke has been completely handled, nil otherwise\rend", 
      __ntHelp: "This is called when the user writes inside the view."
    }, 
    ViewGetDropTypesScript: {
      value: "func(currentPoint)\rbegin\r\t//return an array of symbols\rend", 
      __ntHelp: "This method should return an array of symbols: the data types accepted by the view at the location currentPoint."
    }, 
    ViewIdleScript: {
      value: "func()\rbegin\r\t300;\t// Return the number of milliseconds to delay or nil to end idling.\rend", 
      __ntHelp: "This is called at regular intervals."
    }, 
    ViewFindTargetScript: {
      value: "func(dragInfo)\rbegin\r\t//return a view frame of the view that should get the drop messages.\rend", 
      __ntHelp: "Lets the destination view redirect the drop to a different view."
    }, 
    ViewChangedScript: {
      value: "func(slot, view)\rbegin\r\tnil;\rend", 
      __ntHelp: "This is called when the value of a slot in the view is changed as the result of a SetValue function call."
    }, 
    PrintNextPageScript: {
      value: "func()\rbegin\rend", 
      __ntHelp: "This is called when routing data so that your application can prepare the next page of information."
    }, 
    ViewDropScript: {
      value: "func(dropType,dropData,dropPt)\rbegin\r\t//return true or nil\rend", 
      __ntHelp: "True means that this method handled the drop."
    }, 
    ViewScrollDownScript: {
      value: "func()\rbegin\rend", 
      __ntHelp: "This is called when the view system receives a scrollDown event."
    }, 
    ViewOverviewScript: {
      value: "func()\rbegin\rend", 
      __ntHelp: "This is called when the view system receives an Overview event."
    }, 
    AfterScript: {
      value: "nil // this code will be executed after the template is processed", 
      __ntHelp: "This slot exists only at build time and is executed after the template is processed. The template frame is available in the variable thisView."
    }, 
    ViewDropChildScript: {
      value: "func(oldOne)\rbegin\r\t//This should return true if you manually removed the child\rend", 
      __ntHelp: "This is called when a child view is about to be removed from the view."
    }, 
    PowerOffScript: {
      value: "func(what)\rbegin\rend", 
      __ntHelp: "This is called if the view that has this method has been registered as a power off handler."
    }, 
    ViewDragFeedbackScript: {
      value: "func(dragInfo,currentPoint,show)\rbegin\r\t//return true or nil\rend", 
      __ntHelp: "This method should return a Boolean value. True means that the method did draw. nil means that no feedback was drawn"
    }, 
    PickCancelledScript: {
      value: "func()\rbegin\rend", 
      __ntHelp: "This is called if the view has been specified as the notifyView for a DoPopup() call, and the picker is cancelled."
    }, 
    ViewScrollUpScript: {
      value: "func()\rbegin\rend", 
      __ntHelp: "This is called when the view system receives a scrollUp event."
    }, 
    BeforeScript: {
      value: "nil // this code will be executed before the template is processed", 
      __ntHelp: "This slot exists only at build time and is executed before the template is processed. It may define functions and data that will be available during the processing of a single template."
    }, 
    ViewDropMoveScript: {
      value: "func(dragRef,offset,lastDragPt,copy)\rbegin\r\t//return true or nil\rend", 
      __ntHelp: "True means that this method handled the move."
    }, 
    ViewSetupFormScript: {
      value: "// be sure to call inherited:?ViewSetupFormScript()\rfunc()\rbegin\r\tinherited:?ViewSetupFormScript();\rend", 
      __ntHelp: "This is called before any slots in the view template are read."
    }, 
    ViewAddChildScript: {
      value: "func(newOne)\rbegin\r\t//This should return true if you manually added the child\rend", 
      __ntHelp: "This is called when a child view is about to be added to the view."
    }, 
    ViewClickScript: {
      value: "func(unit)\rbegin\r\t// Return true if click has been completely handled, nil otherwise\rend", 
      __ntHelp: "If the view accepts clicks, then this is called when the user taps or writes inside the view."
    }, 
    ViewHiliteScript: {
      value: "func(hilite)\rbegin\r\t// hilite = true if view is to be highlighted\r\t// hilite = nil if view is to be unhighlighted\rend", 
      __ntHelp: "This is called just before the view system changes the hilite of the view."
    }, 
    ViewDrawDragBackgroundScript: {
      value: "func(bounds,copy)\rbegin\r\t//return true or nil\rend", 
      __ntHelp: "This method should draw the image that will appear behind the dragged data."
    }, 
    PickActionScript: {
      value: "func(itemSelected)\rbegin\rend", 
      __ntHelp: "This is called if the view has been specified as the notifyView for a DoPopup() call, and an item has been selected."
    }, 
    ViewHideScript: {
      value: "func()\rbegin\rend", 
      __ntHelp: "This is called when the view receives a hide command."
    }, 
    ViewWordScript: {
      value: "func(unit)\rbegin\r\t// Return true if input has been completely handled, nil otherwise\rend", 
      __ntHelp: "This is called when a word is recognized and passed to your view."
    }, 
    ViewDropApproveScript: {
      value: "func(destView)\rbegin\r\t//return true or nil\rend", 
      __ntHelp: "This method provides a way for the sourceview to disallow dropping onto a particular view."
    }, 
    ViewSetupChildrenScript: {
      value: "func()\rbegin\rend", 
      __ntHelp: "This is called after the view is created, but before its children are instantiated."
    }, 
    ViewShowScript: {
      value: "func()\rbegin\rend", 
      __ntHelp: "This is called when the view receives a show command."
    }, 
    SoupChanged: {
      value: "func(theSoupName)\rbegin\rend", 
      __ntHelp: "This is called when a different application (or the system) has changed a soup that you own or use."
    }
  }, 
  AttributeSlots: {
    viewBounds: {
      __ntDataType: "RECT", 
      value: {
        top: 0, 
        left: 0, 
        bottom: 0, 
        right: 0
      }, 
      __ntHelp: "This slot defines the bounding box of the view, or the location and size of the view."
    }, 
    viewFlags: {
      
    }, 
    viewFormat: {
      
    }, 
    viewJustify: {
      
    }, 
    viewEffect: {
      
    }, 
    viewFont: {
      
    }, 
    viewOriginX: {
      
    }, 
    viewOriginY: {
      
    }, 
    viewTransferMode: {
      
    }, 
    declareSelf: {
      
    }, 
    copyProtection: {
      
    }, 
    showSound: {
      
    }, 
    hideSound: {
      
    }, 
    scrollUpSound: {
      
    }, 
    scrollDownSound: {
      
    }, 
    doCardRouting: {
      
    }
  }, 
  screenSize: {
    left: 0, 
    top: 0, 
    right: 240, 
    bottom: 336
  }, 
}

#endif

//
// End of "$Id$".
//
