//
// "$Id$"
//
// Dtk_Project implementation for the Dyne Toolkit.
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
// Please report all bugs and problems to "dtk@matthiasm.com".
//

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#include "dtk/Dtk_Project.h"
#include "dtk/Dtk_Error.h"
#include "dtk/Dtk_Document.h"
#include "dtk/Dtk_Platform.h"
#include "dtk/Dtk_Script_Writer.h"
#include "dtk/Dtk_Document_List.h"

#include "fluid/main_ui.h"
#include "main.h"

#include <FL/filename.h>
#include "fltk/Flmm_Message.H"
#include "fluid/Fldtk_Proj_Settings.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#ifdef WIN32
# include <winsock2.h>
# include <direct.h>
# define strdate _strdate
# define strtime _strtime
#else
# include <arpa/inet.h>
#endif


#include "allNewt.h"


/*---------------------------------------------------------------------------*/
Dtk_Project::Dtk_Project()
: packagename_(0L),
  shortname_(0L),
  filename_(0L),
  name_(0L),
  startdir_(0L),
  dos_pathname_(0L),
  posix_pathname_(0L),
  package_(kNewtRefNIL),
  documentList_(0L),
  wMainWindow(dtkMain),
  wBrowserTabs(dtkBrowserTabs)
{
  documentList_ = new Dtk_Document_List(this);
  wMainWindow->label("DyneTK: - unnamed -");
  wBrowserTabs->activate();
}


/*---------------------------------------------------------------------------*/
Dtk_Project::~Dtk_Project()
{
  // remove the project list and all documents it links to
  delete documentList_;
  
  // remove all name shortcuts
	if (shortname_)
		free(shortname_);
	if (filename_)
		free(filename_);
	if (dos_pathname_)
		free(dos_pathname_);
	if (posix_pathname_)
		free(posix_pathname_);
	if (packagename_)
		free(packagename_);
	if (startdir_)
		free(startdir_);
  wMainWindow->label("DyneTK");
  wBrowserTabs->deactivate();  
}


/*---------------------------------------------------------------------------*/
void Dtk_Project::setFilename(const char *filename)
{
	if (shortname_)
		free(shortname_);
	if (filename_)
		free(filename_);
	if (packagename_)
		free(packagename_);
	if (filename) {
		// save the full filename
		filename_ = strdup(filename);
		// save the full filename, but with .pkg extension
		const char *ext = fl_filename_ext(filename);
		int n = ext-filename;
		packagename_ = (char*)malloc(n+6);
		memcpy(packagename_, filename, n);
		memcpy(packagename_+n, ".pkg", 5);
		// save the name only without the extension
		name_ = (char*)fl_filename_name(filename_);
		ext = fl_filename_ext(name_);
		n = ext-name_;
		shortname_ = (char*)calloc(n+1, 1);
		memcpy(shortname_, name_, n);
    // The pathname is the path part of the filename with a trailing slash.
    posix_pathname_ = (char*)calloc(1, name_-filename_+1);
    memcpy(posix_pathname_, filename_, name_-filename_);
    // For compatibility with MSWindows, the path separators are backslashes!
    dos_pathname_ = (char*)calloc(1, name_-filename_+1);
    memcpy(dos_pathname_, filename_, name_-filename_);
    for (char *s=dos_pathname_; *s; ++s) {
      if (*s=='/') *s='\\';
    }
	}
  if (name_ && *name_) {
    char buf[1024];
    sprintf(buf, "DyneTK: %s", name_);
    wMainWindow->copy_label(buf);
  } else {
    wMainWindow->label("DyneTK: - unnamed -");
  }
}


/*---------------------------------------------------------------------------*/
char *Dtk_Project::dos_pathname()
{
  return dos_pathname_;
}


/*---------------------------------------------------------------------------*/
char *Dtk_Project::posix_pathname()
{
  return posix_pathname_;
}


/*---------------------------------------------------------------------------*/
char *Dtk_Project::getPackageName()
{
	return packagename_;
}

/* .ntk file format
 * R -> reading this item is completed
 * W -> writing this item is completed
 * D -> setting the default for this item is completed
 * X -> item is fully implemented 
 * U -> will remain unused
 
      {
 RWDU	  ntkPlatform: 1, 
 RWDU	  fileVersion: 2, 
        projectItems: {
          sortOrder: 0, 
          items: [ 
            { 
              file: { 'fileReference, }, 
              type: 0, 
              isMainLayout: NIL
            }
          ]
      }, 
      projectSettings: {
        PLATFORM: "Newton21", 
        language: "English", 
        debugBuild: TRUE, 
        ignoreNative: NIL, 
        checkglobalfunctions: TRUE, 
        oldBuildRules: NIL, 
        usestepchildren: TRUE, 
        suppressbytecodes: NIL,
        fasterFunctions: TRUE 
      }, 
      outputSettings: {
 RWDX		applicationName: "minimal", 
 RWDX		applicationSymbol: "Minimal:SIG", 
        partType: 0, 
        topFrameExpression: "", 
 RWDU		autoClose: TRUE,  // I can not find the corresponding flag in the Package file format
        customPartType: "UNKN", 
        fasterSoups: NIL, 
        iconpronormal: {
          __ntExternFile: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
          __ntmaskfile: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
          __ntCreateMask: 0, 
          imageinfo1: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
          imageinfo2: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
          imageinfo4: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
          imageinfo8: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
          maskinfo: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
          maskoption: 0
        }, 
        iconprohighlighted: {
          __ntExternFile: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
          __ntmaskfile: {class: 'fileReference, projectPath: "", deltaFromProject:""}, 
          __ntCreateMask: 0, 
          imageinfo1: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
          imageinfo2: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
          imageinfo4: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
          imageinfo8: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
          maskinfo: {class: 'fileReference, projectPath: "", deltaFromProject: ""}, 
          maskoption: 0
        }, 
        iconFile: {class: 'fileReference, projectPath: "", deltaFromProject: ""}
      }, 
      packageSettings: {
 RWDX		packageName: "Minimal:SIG", 
 RWDX		version: "1", 
 RWDX		copyright: "Æ1997 Apple Computer, Inc. All rights reserved.", 
        optimizeSpeed: TRUE, 
        copyProtected: NIL, 
 RWDX		deleteOnDownload: TRUE, 
        dispatchOnly: NIL, 
        fourByteAlignment: TRUE, 
        zippyCompression: TRUE, 
        newton20Only: NIL
      }, 
      profilerSettings: {
        memory: TRUE,
        percent: TRUE, 
        compileforprofiling: NIL, 
        compileForSpeed: NIL, 
        detailedSystemCalls: NIL, 
        detailedUserFunctions: TRUE
      }, 
 RWDU	windowRect: {left: 3, top: 410, bottom:550, right: 730}
    }
 
 
 */

/*---------------------------------------------------------------------------*/
int Dtk_Project::load()
{
	uint8_t buf[4];
	FILE *f = fopen(filename_, "rb");
	if (!f) {
		return -1;
	}
	int n = fread(buf, 4, 1, f);
	fclose(f);
	if (n<=0) {
		return -1;
	}
	AddPrevProj(filename_);
	if (buf[0]==2) {
		return loadWin();
	} else if (buf[3]==103) {
		return loadMac();
	} else {
		printf("Unrecognized file type in '%s'\n", filename_);
		return -1;
	}
}

static uint32_t readInt(FILE *f) {
	uint32_t v;
	fread(&v, 1, 4, f);
	return ntohl(v);
}

static uint16_t readWord(FILE *f) {
	uint16_t v;
	fread(&v, 1, 2, f);
	return ntohs(v);
}

static uint8_t readByte(FILE *f) {
	uint8_t v;
	fread(&v, 1, 1, f);
	return v;
}

static char *PtoCFilename(uint8_t *src) {
	int i, n = (int)src[0];
	char *d = (char*)malloc(n+2), *dst = d;
  *d++ = '/';
	for (i=1; i<=n; i++) {
		char c = (char)src[i];
		if (c>126 || c<32) c = '_';
		switch (c) {
			case ':': *d++ = '/'; break;
			case '/': *d++ = '.'; break;
			case '?': *d++ = '_'; break;
			default: *d++ = c; break;
		}
	}
	*d = 0;
	return dst;
}

/*---------------------------------------------------------------------------*/
int Dtk_Project::loadMac()
{
	printf("Reading MAC file\n");
	int i, j;
  
	// open the resource fork
	char buf[1024];
	sprintf(buf, "%s/rsrc", filename_);
	FILE *rsrc = fopen(buf, "rb");
	if (!rsrc) {
		strcpy(buf, filename_);
		char *fn = (char*)fl_filename_name(buf);
		if (fn) {
			memmove(fn+2, fn, strlen(fn)+1);
			memcpy(fn, "._", 2);
			rsrc = fopen(buf, "rb");
			if (!rsrc)
        return -1;
		} else {
			return -1;
		}
	}
  
	// open the data fork
	FILE *data = fopen(filename_, "rb");
	if (!data) {
		return -1;
	}
  
	pushDir();
  
	/*uint32_t id =*/ readInt(data);
	uint16_t fileCount = readWord(data);
	/*uint32_t sortBy =*/ readInt(data);
	for (i=0; i<fileCount; i++) {
		uint32_t len = readInt(data);
		// if len==70 (sizeof(FSSpec)) we must conver a filespec
		// otherwise, this is an alias
		uint8_t *alias = (uint8_t*)malloc(len);
		fread(alias, len, 1, data);
#               if 1 // find the name in any case
    char *fn = PtoCFilename(alias+0x32);
    char *filename = documentList_->findFile(fn);
    documentList_->add(filename);
    printf("Alias %d = %s\n", i, filename);
    free(fn);
    free(filename);
#               else // find the path and name, but doesn't always work
    char *fn = PtoCFilename(alias+0xc1);
    char *filename = documentList_->findFile(fn);
    documentList_->add(filename);
    printf("Alias %d = %s\n", i, filename);
    free(fn);
    free(filename);
#               endif
	}
  // FIXME mainLayout is an index, but not by the same sorting standard!
	uint16_t mainLayout = readWord(data);
  if (mainLayout>0 && mainLayout<=documentList_->size())
    documentList_->setMainDocument(documentList_->at(mainLayout-1));
  
	// read the resource fork
  
	// find and read the PJPF resource
	// -- read the header
	uint32_t rData = readInt(rsrc);
	uint32_t rMap  = readInt(rsrc);
  
	// -- read the resource map
	fseek(rsrc, rMap+24, SEEK_SET);
	uint16_t rsrcType = readWord(rsrc);
	/*uint16_t rsrcName =*/ readWord(rsrc);
	uint16_t nRsrc = readWord(rsrc)+1;
  
	// -- walk the resource type list	
	for (i=0; i<nRsrc; i++) {
		fseek(rsrc, rMap+rsrcType+8*i+2, SEEK_SET);
		uint32_t type = readInt(rsrc);
		uint16_t nType = readWord(rsrc)+1;
		uint16_t list = readWord(rsrc);
		//printf("Rsrc %d: %.4s %d\n", i, &type, nType);
		if (type=='PJPF') {
			for (j=0; j<nType; j++) {
				fseek(rsrc, rMap+rsrcType+list+12*j, SEEK_SET);
				uint16_t id = readWord(rsrc);
				/*uint16_t name =*/ readWord(rsrc);
				uint32_t offs = readInt(rsrc); //&0xffffff;
				/*uint32_t handle =*/ readInt(rsrc);
				//printf("PJPF: %d %d %d \n", id, name, offs);
				if (id==9999) {
					fseek(rsrc, rData+offs, SEEK_SET);
					/*uint32_t len =*/ readInt(rsrc);
          char buf[64];
          fread(buf, 33, 1, rsrc);
          printf("Appname is %.*s\n", buf[0], buf+1);
          readByte(rsrc); // padding
          fread(buf, 33, 1, rsrc);
          printf("Iconname is %.*s\n", buf[0], buf+1);
          readByte(rsrc); // padding
          fread(buf, 33, 1, rsrc);
          printf("Platform is %.*s\n", buf[0], buf+1);
          readByte(rsrc); // padding
          fread(buf, 33, 1, rsrc);
          printf("Packagename is %.*s\n", buf[0], buf+1);
          readByte(rsrc); // padding
          fread(buf, 33, 1, rsrc);
          printf("AppSymbol is %.*s\n", buf[0], buf+1);
          readByte(rsrc); // padding
          fread(buf, 33, 1, rsrc);
          printf("Version is %.*s\n", buf[0], buf+1);
          readByte(rsrc); // padding
          fread(buf, 33, 1, rsrc);
          printf("Iconname is %.*s\n", buf[0], buf+1);
          readByte(rsrc); // padding
          fread(buf, 64, 1, rsrc);
          printf("Copyright is %.*s\n", buf[0], buf+1);
          readByte(rsrc); // OptimizeSpeed
          readByte(rsrc); // CopyProtected
          readByte(rsrc); // DeleteOnDownload
          readByte(rsrc); // DebugBuild
          readByte(rsrc); // AutoClose
          readByte(rsrc); // Padding
          fread(buf, 64, 1, rsrc);
          printf("IconFile is %.*s\n", buf[0], buf+1);
          readByte(rsrc); // CustomPart
          readByte(rsrc); // Padding
          readInt(rsrc); // PartType (form, book, auto, soup)
          fseek(rsrc, 256, SEEK_CUR); // TopFrameExpression
          readByte(rsrc); // MakeStream
          readByte(rsrc); // DispatchOnly
          readByte(rsrc); // Newton20Only
          readByte(rsrc); // Padding
          readByte(rsrc); // CompileForProfiling
          readByte(rsrc); // CompileForSpeed
          readByte(rsrc); // DetailedSystemCalls
          readByte(rsrc); // Padding
          readWord(rsrc); // Memory (always 4k)
          readByte(rsrc); // Percent (always 4)
          readByte(rsrc); // DetailedUserFunctions
          fread(buf, 64, 1, rsrc);
          printf("Language is %.*s\n", buf[0], buf+1);
          readByte(rsrc); // IgnoreNative
          readByte(rsrc); // CheckGlobalFunctions
          readByte(rsrc); // OldBuildRules
          readByte(rsrc); // UseStepChildren
          readByte(rsrc); // SuppressByteCodes
          readByte(rsrc); // FasterFunctions
          readByte(rsrc); // FasterSoups
          readByte(rsrc); // FourByteAlignment
          readByte(rsrc); // ZippyCompression
          readByte(rsrc); // Padding
				}
			}
		}
	}
  
	fclose(data);
	fclose(rsrc);
  
	popDir();
  
	return 0;
}

/*---------------------------------------------------------------------------*/
int Dtk_Project::loadWin()
{
  char buf[2048];

	// read the project settings from disk
	FILE *f = fopen(filename_, "rb");
	if (!f) {
		return -1;
	}
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);
	uint8_t *data = (uint8_t*)malloc(size);
	fread(data, size, 1, f);
	fclose(f);
  
	newtRef p = NewtReadNSOF(data, size);
  
	free(data);
  
	//NewtPrintObject(stdout, p);
  
	pushDir();
  
	// now extract all supported settings and write them into their locations
	if (NewtRefIsFrame(p)) {
		int32_t ix;
		// output
		newtRef output = NewtGetFrameSlot(p, NewtFindSlotIndex(p, NSSYM(outputSettings)));
		if (NewtRefIsFrame(output)) {
			// applicationName
			newtRef name = NewtGetFrameSlot(output, NewtFindSlotIndex(output, NSSYM(applicationName)));
			if (NewtRefIsString(name))
				dtkProjSettings->app->name->set(NewtRefToString(name));
			// applicationSymbol
			newtRef symbol = NewtGetFrameSlot(output, NewtFindSlotIndex(output, NSSYM(applicationSymbol)));
			if (NewtRefIsString(symbol))
				dtkProjSettings->app->symbol->set(NewtRefToString(symbol));
			// autoClose
			newtRef slot = NewtGetFrameSlot(output, NewtFindSlotIndex(output, NSSYM(autoClose)));
			if (slot==kNewtRefTRUE) dtkProjSettings->app->auto_close->set(1);
			if (slot==kNewtRefNIL) dtkProjSettings->app->auto_close->set(0);
			// partType (int30)
			// topFrameExpression (string)
			// customPartType (string, fourCC)
			// fasterSoups (bool)
			// iconFile (fileRef): do this before loading any extended icons below
		  newtRef iconFile = NewtGetFrameSlot(output, NewtFindSlotIndex(output, NSSYM(iconFile)));
      if (NewtRefIsFrame(iconFile)) {
        newtRef name = NewtGetFrameSlot(iconFile, NewtFindSlotIndex(iconFile, NSSYM(deltaFromProject)));
        if (NewtRefIsString(name)) {
			    char *filename = NewtRefToString(name);
          fl_filename_absolute(buf, 2047, filename);
          dtkProjSettings->icon->wIcon1->setImageFilename(buf);
        }
      }
			// iconProNormal (Frame)
      newtRef iconProNormal = NewtGetFrameSlot(output, NewtFindSlotIndex(output, NSSYM(iconProNormal)));
      if (NewtRefIsFrame(iconProNormal)) {
        // load the icon if any
        newtRef imageinfo1 = NewtGetFrameSlot(iconProNormal, NewtFindSlotIndex(iconProNormal, NSSYM(imageinfo1)));
        if (NewtRefIsFrame(imageinfo1)) {
          newtRef name = NewtGetFrameSlot(imageinfo1, NewtFindSlotIndex(imageinfo1, NSSYM(deltaFromProject)));
          if (NewtRefIsString(name)) {
				    char *filename = NewtRefToString(name);
            fl_filename_absolute(buf, 2047, filename);
            dtkProjSettings->icon->wIcon1->setImageFilename(buf);
          }
        }
        // load the mask if any
        newtRef maskinfo = NewtGetFrameSlot(iconProNormal, NewtFindSlotIndex(iconProNormal, NSSYM(maskinfo)));
        if (NewtRefIsFrame(maskinfo)) {
          newtRef name = NewtGetFrameSlot(maskinfo, NewtFindSlotIndex(maskinfo, NSSYM(deltaFromProject)));
          if (NewtRefIsString(name)) {
				    char *filename = NewtRefToString(name);
            fl_filename_absolute(buf, 2047, filename);
            dtkProjSettings->icon->wMask->setImageFilename(buf);
          }
        }
      }
			// iconProHighlighted (Frame): \\\todo Add highlight icon support
		}
		ix = NewtFindSlotIndex(p, NSSYM(packageSettings));
		if (ix>=0) {
			newtRef packageSettings = NewtGetFrameSlot(p, ix);
			// packageName
			ix = NewtFindSlotIndex(packageSettings, NSSYM(packageName));
			if (ix>=0) {
				newtRef packageName = NewtGetFrameSlot(packageSettings, ix);
				dtkProjSettings->package->name->set(NewtRefToString(packageName));
			}
			// copyright
			ix = NewtFindSlotIndex(packageSettings, NSSYM(copyright));
			if (ix>=0) {
				newtRef copyright = NewtGetFrameSlot(packageSettings, ix);
				dtkProjSettings->package->copyright->set(NewtRefToString(copyright));
			}
			// version
			ix = NewtFindSlotIndex(packageSettings, NSSYM(version));
			if (ix>=0) {
				newtRef version = NewtGetFrameSlot(packageSettings, ix);
				dtkProjSettings->package->version->set(NewtRefToString(version));
			}
			// optimizeSpeed (bool)
			// copyProtected (bool)
			// deleteOnDownload (bool)
			newtRef deleteOnDownload = NewtGetFrameSlot(packageSettings, NewtFindSlotIndex(packageSettings, NSSYM(deleteOnDownload)));
			if (deleteOnDownload==kNewtRefTRUE) dtkProjSettings->package->deleteOnDownload->set(1);
			if (deleteOnDownload==kNewtRefNIL)  dtkProjSettings->package->deleteOnDownload->set(0);
			// dispatchOnly (bool)
			// fourByteAlignment (bool)
			// zippyCompression (bool)
			// newton20Only (bool)
		}
		ix = NewtFindSlotIndex(p, NSSYM(projectItems));
		if (ix>=0) {
			newtRef projectItems = NewtGetFrameSlot(p, ix);
			ix = NewtFindSlotIndex(projectItems, NSSYM(items));
			if (ix>0) {
				newtRef items = NewtGetFrameSlot(projectItems, ix);
				uint32_t i, n = NewtArrayLength(items);
				for (i=0; i<n; i++) {
					// file, type, isMainLayout
          Dtk_Document *doc = 0L;
					newtRef item = NewtGetArraySlot(items, i);
					int type = -1;
					ix = NewtFindSlotIndex(item, NSSYM(type)); 
					if (ix>=0) {
						newtRef type_r = NewtGetFrameSlot(item, ix);
						type = NewtRefToInteger(type_r);
					}
					ix = NewtFindSlotIndex(item, NSSYM(file)); 
					if (ix>=0) {
						newtRef file = NewtGetFrameSlot(item, ix);
						ix = NewtFindSlotIndex(file, NSSYM(deltaFromProject)); 
						if (ix>=0) {
							newtRef name = NewtGetFrameSlot(file, ix);						
							char *filename = NewtRefToString(name);
							switch (type) {
								case 0:  doc = documentList_->newLayout(filename); 
                  doc->load();
                  doc->edit();
                  break;
								case 5:  doc = documentList_->newScript(filename); 
                  doc->load();
                  doc->edit();
                  break;
								default: doc = documentList_->add(filename); break;
							}
						}
					}
					ix = NewtFindSlotIndex(item, NSSYM(isMainLayout)); 
					if (ix>=0 && doc) 
            documentList_->setMainDocument(doc);
				}
			}
		}
	}
	dtkProjSettings->updateDialog();
	popDir();
	return 0;
}

/*---------------------------------------------------------------------------*/
void Dtk_Project::pushDir()
{
	char projPath[FL_PATH_MAX];
	char currPath[FL_PATH_MAX];
	strcpy(projPath, filename_);
	char *name = (char*)fl_filename_name(projPath);
	if (name) 
		*name = 0;
  
	if (startdir_) {
		free(startdir_);
		startdir_ = 0L;
	}
  
	const char *here = getcwd(currPath, FL_PATH_MAX);
	if (here) {
		startdir_ = strdup(here);
		chdir(projPath);
	}
} 

/*---------------------------------------------------------------------------*/
void Dtk_Project::popDir()
{
	if (startdir_) {
		chdir(startdir_);
	}
}

/*---------------------------------------------------------------------------*/
void Dtk_Project::setDefaults() 
{
	char buf[80];
  
	dtkProjSettings->app->name->set(shortname_);
	sprintf(buf, "%s:SIG", shortname_);
	dtkProjSettings->app->symbol->set(buf);
	dtkProjSettings->app->auto_close->set(1);
  
	sprintf(buf, "%s:SIG", shortname_);
	dtkProjSettings->package->name->set(buf);
	dtkProjSettings->package->copyright->set("(c) 2007 DyneTK, All Rights Reserved.");
	dtkProjSettings->package->version->set("1");
	dtkProjSettings->package->deleteOnDownload->set(1);
}


/*---------------------------------------------------------------------------*/
newtRef Dtk_Project::makeFileRef(const char *filename)
{
	newtRefVar fileReferenceA[] = {
		NSSYM(class),			        NSSYM(fileReference),
		NSSYM(projectPath),		    kNewtRefNIL,
		NSSYM(deltaFromProject),  kNewtRefNIL
	};
  
	if (filename) {
		char projPath[FL_PATH_MAX];
		char currPath[FL_PATH_MAX];
		char relPath[FL_PATH_MAX];
    
		strcpy(projPath, filename_);
		char *name = (char*)fl_filename_name(projPath);
		if (name) *name = 0;
    
		const char *here = getcwd(currPath, FL_PATH_MAX);
		if (here) {
			chdir(projPath);
			fl_filename_relative(relPath, FL_PATH_MAX, filename);
			chdir(currPath);
		} else {
			strcpy(relPath, filename);
		}
    
#ifdef WIN32
		char *d;
		for (d = projPath; *d; ++d) { if (*d=='/') *d = '\\'; }
		for (d =  relPath; *d; ++d) { if (*d=='/') *d = '\\'; }
#endif
		fileReferenceA[3] = NewtMakeString(projPath, false);
		fileReferenceA[5] = NewtMakeString(relPath, false);
	} else {
		fileReferenceA[3] = NewtMakeString("", false);
		fileReferenceA[5] = NewtMakeString("", false);
	}
  
	newtRef fileReference = NewtMakeFrame2(sizeof(fileReferenceA) / (sizeof(newtRefVar) * 2), fileReferenceA);
	return fileReference;
}


/*---------------------------------------------------------------------------*/
int Dtk_Project::saveAll() 
{
  int i, n = documentList_->size();
  for (i=0; i<n; ++i) {
    documentList_->at(i)->save();
  }
  return save();
}


/*---------------------------------------------------------------------------*/
int Dtk_Project::save() 
{
	newtRef items = documentList_->getProjectItemsRef();
  
	newtRefVar projectItemsA[] = {
		NSSYM(sortOrder),			NewtMakeInt30(0),
		NSSYM(items),				items
	};
	newtRef projectItems = NewtMakeFrame2(sizeof(projectItemsA) / (sizeof(newtRefVar) * 2), projectItemsA);
  
	newtRefVar projectSettingsA[] = {
		NSSYM(platform),			      NewtMakeString("Newton21", false),
		NSSYM(language),			      NewtMakeString("English", false),
		NSSYM(debugBuild),			    kNewtRefTRUE,
		NSSYM(ignoreNative),		    kNewtRefNIL,
		NSSYM(checkGlobalFunctions),kNewtRefTRUE,
		NSSYM(oldBuildRules),		    kNewtRefNIL,
		NSSYM(useStepChildren),		  kNewtRefTRUE,
		NSSYM(suppressByteCodes),	  kNewtRefNIL,
		NSSYM(fasterFunctions),		  kNewtRefTRUE
	};
	newtRef projectSettings = NewtMakeFrame2(sizeof(projectSettingsA) / (sizeof(newtRefVar) * 2), projectSettingsA);
  
	newtRefVar profilerSettingsA[] = {
		NSSYM(memory),				      kNewtRefTRUE,
		NSSYM(percent),				      kNewtRefTRUE,
		NSSYM(compileForProfiling),	kNewtRefNIL,
		NSSYM(compileForSpeed),		  kNewtRefNIL,
		NSSYM(detailedSystemCalls),	kNewtRefNIL,
		NSSYM(detailedUserFunctions),kNewtRefTRUE
	};
	newtRef profilerSettings = NewtMakeFrame2(sizeof(profilerSettingsA) / (sizeof(newtRefVar) * 2), profilerSettingsA);
  
	newtRefVar packageSettingsA[] = {
		NSSYM(packageName),			    NewtMakeString(dtkProjSettings->package->name->get(), true),
		NSSYM(version),				      NewtMakeString(dtkProjSettings->package->version->get(), true),
		NSSYM(copyright),			      NewtMakeString(dtkProjSettings->package->copyright->get(), true),
		NSSYM(optimizeSpeed),		    kNewtRefTRUE,
		NSSYM(copyProtected),		    kNewtRefNIL,
		NSSYM(deleteOnDownload),	  dtkProjSettings->package->deleteOnDownload->get() ? kNewtRefTRUE : kNewtRefNIL,
		NSSYM(dispatchOnly),		    kNewtRefNIL,
		NSSYM(fourByteAlignment),	  kNewtRefTRUE,
		NSSYM(zippyCompression),	  kNewtRefTRUE,
		NSSYM(newton20Only),		    kNewtRefNIL
	};
	newtRef packageSettings = NewtMakeFrame2(sizeof(packageSettingsA) / (sizeof(newtRefVar) * 2), packageSettingsA);
  
	newtRefVar iconProNormalA[] = {
    NSSYM(__ntExternFile),		  makeFileRef(dtkProjSettings->icon->wIcon1->getImageFilename()), 
		NSSYM(__ntmaskfile),		    makeFileRef(dtkProjSettings->icon->wMask->getImageFilename()), 
		NSSYM(__ntCreateMask),		  NewtMakeInt30(0),
		NSSYM(imageinfo1),			    makeFileRef(dtkProjSettings->icon->wIcon1->getImageFilename()), 
		NSSYM(imageinfo2),			    makeFileRef(0), 
		NSSYM(imageinfo4),			    makeFileRef(0), 
		NSSYM(imageinfo8),			    makeFileRef(0), 
		NSSYM(maskinfo),			      makeFileRef(dtkProjSettings->icon->wMask->getImageFilename()), 
		NSSYM(maskoption),			    NewtMakeInt30(0),
	};
	newtRef iconProNormal = NewtMakeFrame2(sizeof(iconProNormalA) / (sizeof(newtRefVar) * 2), iconProNormalA);
  
	newtRefVar iconProHighlightedA[] = {
		NSSYM(__ntExternFile),		  makeFileRef(0), 
		NSSYM(__ntmaskfile),		    makeFileRef(0), 
		NSSYM(__ntCreateMask),		  NewtMakeInt30(0),
		NSSYM(imageinfo1),			    makeFileRef(0), 
		NSSYM(imageinfo2),			    makeFileRef(0), 
		NSSYM(imageinfo4),			    makeFileRef(0), 
		NSSYM(imageinfo8),			    makeFileRef(0), 
		NSSYM(maskinfo),			      makeFileRef(0), 
		NSSYM(maskoption),			    NewtMakeInt30(0),
	};
	newtRef iconProHighlighted = NewtMakeFrame2(sizeof(iconProHighlightedA) / (sizeof(newtRefVar) * 2), iconProHighlightedA);
  
	newtRefVar outputSettingsA[] = {
		NSSYM(applicationName),		  NewtMakeString(dtkProjSettings->app->name->get(), false),
		NSSYM(applicationSymbol),	  NewtMakeString(dtkProjSettings->app->symbol->get(), false),
		NSSYM(partType),			      NewtMakeInt30(0),
		NSSYM(topFrameExpression),	NewtMakeString("", false),
		NSSYM(autoClose),			      dtkProjSettings->app->symbol->get() ? kNewtRefTRUE : kNewtRefNIL,
		NSSYM(customPartType),		  NewtMakeString("UNKN", false),
		NSSYM(fasterSoups),			    kNewtRefNIL,
		NSSYM(iconProNormal),		    iconProNormal,
		NSSYM(iconProHighlighted),	iconProHighlighted, 
		NSSYM(iconFile),			      makeFileRef(dtkProjSettings->icon->wIcon1->getImageFilename())
	};
	newtRef outputSettings = NewtMakeFrame2(sizeof(outputSettingsA) / (sizeof(newtRefVar) * 2), outputSettingsA);
  
	// this is the size of the Project subwindow which do not use in DTK, so we give this fixed values
	newtRefVar windowRectA[] = {
		NSSYM(left),				        NewtMakeInt30(3),
		NSSYM(top),					        NewtMakeInt30(410),
		NSSYM(bottom),				      NewtMakeInt30(550),
		NSSYM(right),				        NewtMakeInt30(730)
	};
	newtRef windowRect = NewtMakeFrame2(sizeof(windowRectA) / (sizeof(newtRefVar) * 2), windowRectA);
  
	newtRefVar ntkFrameA[] = {
		NSSYM(ntkPlatform),			    NewtMakeInt30(1),
		NSSYM(fileVersion),			    NewtMakeInt30(2),
		NSSYM(projectItems),		    projectItems,
		NSSYM(projectSettings),		  projectSettings,
		NSSYM(outputSettings),		  outputSettings,
		NSSYM(packageSettings),		  packageSettings,
		NSSYM(profilerSettings),	  profilerSettings,
		NSSYM(windowRect),			    windowRect
	};
	newtRef ntkFrame = NewtMakeFrame2(sizeof(ntkFrameA) / (sizeof(newtRefVar) * 2), ntkFrameA);
  
	newtRef rcvr = kNewtRefNIL;
	newtRef nsof = NsMakeNSOF(rcvr, ntkFrame, NewtMakeInt30(2));
  
	int size = NewtBinaryLength(nsof);
	uint8_t *data = NewtRefToBinary(nsof);
  
  // Open a file as a destination for our project
	FILE *f = fopen(filename_, "wb");
  if (!f) {
    return -1;
  }
  // Write everything in a single block
  if (fwrite(data, 1, size, f)!=(size_t)size) {
    fclose(f);
    return -1;
  }
  // Close the file and return indicating no error
  fclose(f);
	AddPrevProj(filename_);
  
	return 0;
}


/*---------------------------------------------------------------------------*/
/** 
 * Create a Frame with the default boilerplate InstallScript()
 */
newtRef makeInstallScript() {
  
	static uint8_t installScriptBin[] = {
		0x20, 0xa4, 0x70, 0x19, 0x91, 0xa5, 0x7d, 0x1a, 0x29, 0x6f, 0x00, 0x49, 0x7d, 0x22, 0xc7, 0x00, 
		0x11, 0xa7, 0x00, 0x07, 0x5f, 0x00, 0x3c, 0x7f, 0x00, 0x07, 0x24, 0xc2, 0xa6, 0x7e, 0x1b, 0x91,
		0x7b, 0x1b, 0x91, 0xc4, 0x6f, 0x00, 0x38, 0x27, 0x00, 0x0c, 0x1c, 0x1d, 0x1e, 0x28, 0x1f, 0x00, 
		0x07, 0x3b, 0x00, 0x22, 0x5f, 0x00, 0x44, 0x00, 0x7f, 0x00, 0x07, 0x05, 0x7f, 0x00, 0x07, 0x06,
		0x6f, 0x00, 0x17, 0x22, 0x22, 0xa7, 0x00, 0x07, 0x00, 0x7b, 0x1f, 0x00, 0x08, 0x1f, 0x00, 0x09,
		0x2a, 0x6f, 0x00, 0x61, 0x7b, 0x7b, 0x1f, 0x00, 0x08, 0x39, 0x00, 0x7b, 0x1f, 0x00, 0x08, 0x22, 
		0x98, 0x7b, 0x1f, 0x00, 0x0a, 0x22, 0x99, 0x02,
	};
	newtRef instructions = NewtMakeBinary(NSSYM(instructions), installScriptBin, sizeof(installScriptBin), false);
  
	newtRefVar literalsA[] = {
		NSSYM(vars), NSSYM(extras), NSSYM(IsArray), NSSYM(app), NewtMakeString("Extras Drawer", false), 
		NewtMakeString("The application you just installed conflicts with another application."
                   " Please contact the application vendor for an updated version.", false),
		NSSYM(GetRoot), NSSYM(Notify), NSSYM(devInstallScript), NSSYM(HasSlot), NSSYM(installScript)
	};
	newtRef literals = NewtMakeArray2(NSSYM(literals), sizeof(literalsA)/sizeof(newtRefVar), literalsA);
  
	newtRefVar nextArgFrameA[] = {
		NSSYM(_nextArgFrame),	kNewtRefNIL,
		NSSYM(_parent),			kNewtRefNIL,
		NSSYM(_implementor),	kNewtRefNIL,
	};
	newtRef nextArgFrame = NewtMakeFrame2(sizeof(nextArgFrameA) / (sizeof(newtRefVar) * 2), nextArgFrameA);
  
	newtRefVar argFrameA[] = {
		NSSYM(_nextArgFrame),	nextArgFrame,
		NSSYM(_parent),			kNewtRefNIL,
		NSSYM(_implementor),	kNewtRefNIL,
		NSSYM(p),				kNewtRefNIL,
		NSSYM(i),				kNewtRefNIL,
		NSSYM(extras),			kNewtRefNIL,
		NSSYM(one),				kNewtRefNIL,
		NSSYM(one|iter),		kNewtRefNIL,
	};
	newtRef argFrame = NewtMakeFrame2(sizeof(argFrameA) / (sizeof(newtRefVar) * 2), argFrameA);
  
	newtRefVar scriptA[] = {
		NS_CLASS,				NSSYM(CodeBlock),
		NSSYM(instructions),	instructions,
		NSSYM(literals),		literals,
		NSSYM(argFrame),		argFrame,
		NSSYM(numArgs),			NewtMakeInt30(1),
		NSSYM(DebuggerInfo),	kNewtRefNIL
	};
	newtRef script = NewtMakeFrame2(sizeof(scriptA) / (sizeof(newtRefVar) * 2), scriptA);
	return script;
}


/*---------------------------------------------------------------------------*/
/** 
 * Create a Frame with the default uninstall script for packages.
 */
newtRef makeRemoveScript() {
  
	static uint8_t removeScriptBin[] = {
		0x7b, 0x18, 0x19, 0x2a, 0x6f, 0x00, 0x0e, 0x7b, 0x7b, 
		0x18, 0x39, 0x5f, 0x00, 0x0f, 0x22, 0x02, 
	};
	newtRef instructions = NewtMakeBinary(NSSYM(instructions), removeScriptBin, sizeof(removeScriptBin), false);
  
	newtRefVar literalsA[] = {
		NSSYM(devRemoveScript), NSSYM(HasSlot)
	};
	newtRef literals = NewtMakeArray2(NSSYM(literals), sizeof(literalsA)/sizeof(newtRefVar), literalsA);
  
	newtRefVar nextArgFrameA[] = {
		NSSYM(_nextArgFrame),	kNewtRefNIL,
		NSSYM(_parent),			kNewtRefNIL,
		NSSYM(_implementor),	kNewtRefNIL
	};
	newtRef nextArgFrame = NewtMakeFrame2(sizeof(nextArgFrameA) / (sizeof(newtRefVar) * 2), nextArgFrameA);
  
	newtRefVar argFrameA[] = {
		NSSYM(_nextArgFrame),	nextArgFrame,
		NSSYM(_parent),			kNewtRefNIL,
		NSSYM(_implementor),	kNewtRefNIL,
		NSSYM(p),				kNewtRefNIL
	};
	newtRef argFrame = NewtMakeFrame2(sizeof(argFrameA) / (sizeof(newtRefVar) * 2), argFrameA);
	// FIXME argFrame->map[0] = 0x0a000002 (probably marking the fact that the slot must not be sorted?)
	// FIXME seems to work anyways though
  
	newtRefVar scriptA[] = {
		NS_CLASS,				NSSYM(CodeBlock),
		NSSYM(instructions),	instructions,
		NSSYM(literals),		literals,
		NSSYM(argFrame),		argFrame,
		NSSYM(numArgs),			NewtMakeInt30(1),
		NSSYM(DebuggerInfo),	kNewtRefNIL
	};
	newtRef script = NewtMakeFrame2(sizeof(scriptA) / (sizeof(newtRefVar) * 2), scriptA);
	return script;
}


/*---------------------------------------------------------------------------*/
/**
 * We stumble to hack together a simple package file more or less from scratch... .
 */
int Dtk_Project::buildPackage()
{
  InspectorPrintf("Building package\n");
	//int32_t ix;
  
	newtRefVar theForm = kNewtRefNIL;
	newtRefVar theBase = kNewtRefNIL;
  
  //dtkPlatform->loadConstFile();
  
  // the new way which currently requires a script file on disk
  // write the package into a file
  ExportPackageToText();
  // read it back into memory
  struct stat st;
  stat("testing_script_writer.txt", &st);
  FILE *f = fopen("testing_script_writer.txt", "rb");
	char *script = (char*)malloc(st.st_size);
  fread(script, 1, st.st_size, f);
  fclose(f);
  // compile and run ir
  newtErr	err;
  int prevDumpPC = NEWT_DUMPBC; NEWT_DUMPBC = 0;
  //NEWT_DEBUG = 0;
  //NEWT_TRACE = 1;
  theForm = NVMInterpretStr(script, &err);
  NEWT_DUMPBC = prevDumpPC;
  // release the memory taken by the script
  free(script);
  // very simple error code output
	if (theForm==kNewtRefUnbind) {
		InspectorPrintf("**** ERROR while compiling or interpreting\n");
    newtRef a = NVMSelf();
    NewtPrintObject(stdout, a);
    a = NVMCurrentFunction();
    NewtPrintObject(stdout, a);
    a = NVMCurrentException();
    NewtPrintObject(stdout, a);
/*
newtRef		NVMSelf(void);
newtRef		NVMCurrentFunction(void);
newtRef		NVMCurrentImplementor(void);
bool		NVMHasVar(newtRefArg name);
void		NVMThrowData(newtRefArg name, newtRefArg data);
void		NVMThrow(newtRefArg name, newtRefArg data);
void		NVMRethrow(void);
newtRef		NVMCurrentException(void);
void		NVMClearException(void);
*/
    if (err)
      InspectorPrintf("**** %s: %s\n", newt_error_class(err), newt_error(err));
    else 
      InspectorPrintf("**** unknow error\n");
    return kNewtRefUnbind;
	} else {
	}
  
	//NewtPrintObject(stdout, theForm);
  
	theForm = kNewtRefUnbind;
	theForm = NcGetGlobalVar(NSSYM(theForm));
	theBase = NcGetGlobalVar(NSSYM(theBase));
  if (theForm==kNewtRefUnbind || theBase==kNewtRefUnbind) {
    InspectorPrintf("Error building project (no base or form found)!\n");
    return -1;
  }
  
	// the following little hack removes all local variables from the 
	// base object to avoid a recursion when writing the package
	newtObjRef obj = NewtRefToPointer(theBase);
	uint32_t i, index, len = NewtObjSlotsLength(obj);
	for (i=len-1; i>2; i--) {
		newtRefVar slot = NewtGetMapIndex(obj->as.map, i, &index);	
		//printf("Slot %d = <%s>\n", i, NewtRefToSymbol(slot)->name);
		NcRemoveSlot(theBase, slot);
	}
  
	//NewtPrintObject(stdout, theForm);
	//NewtPrintObject(stdout, theBase);
  
	// create the package
  newtRef icon = Fldtk_Icon_Dropbox::buildIcon(dtkProjSettings->icon->wIcon1, dtkProjSettings->icon->wMask, 0);

	newtRefVar dataA[] = {
		NSSYM(app),				NewtMakeSymbol(dtkProjSettings->app->symbol->get()),
		NSSYM(text),			NewtMakeString(dtkProjSettings->app->name->get(), false),
		NSSYM(icon),			icon,
		NSSYM(theForm),			theForm, //+
		NSSYM(installScript),	makeInstallScript(), //+
		NSSYM(removeScript),	makeRemoveScript()  //+
	};
	newtRef data = NewtMakeFrame2(sizeof(dataA) / (sizeof(newtRefVar) * 2), dataA);
  
	newtRefVar part0A[] = {
		NSSYM(class),			NSSYM(PackagePart),
		NSSYM(info),			NewtMakeBinary(NSSYM(binary), (uint8_t*)"A Newton Toolkit application", /*28*/24, false),
		NSSYM(flags),			NewtMakeInt30(129),
		NSSYM(type),			NewtMakeInt32('form'),
		NSSYM(data),			data
	};
	newtRef part0 = NewtMakeFrame2(sizeof(part0A) / (sizeof(newtRefVar) * 2), part0A);
	
	newtRef parts = NewtMakeArray(kNewtRefUnbind, 1);
	NewtSetArraySlot(parts, 0, part0);
  
	newtRefVar packageA[] = {
		NSSYM(class),			NSSYM(PackageHeader),
		NSSYM(type),			NewtMakeInt32('xxxx'),
		NSSYM(pkg_version),		NewtMakeInt32(0),
		NSSYM(version),			NewtMakeInt32(atoi(dtkProjSettings->package->version->get())),
		NSSYM(copyright),		NewtMakeString(dtkProjSettings->package->copyright->get(), false),
		NSSYM(name),			NewtMakeString(dtkProjSettings->package->name->get(), false),
		NSSYM(flags),			NewtMakeInt32(0x10000000),
		NSSYM(parts),			parts
	};
	newtRef package = NewtMakeFrame2(sizeof(packageA) / (sizeof(newtRefVar) * 2), packageA);
  
	newtRef rcvr = kNewtRefNIL;
  
	//NewtPrintObject(stdout, package);
	package_ = NsMakePkg(rcvr, package);
	//NewtPrintObject(stdout, package_);
  
  if (NewtRefIsBinary(package_)) {
    InspectorPrintf("Package build successfully (%dkBytes)\n", NewtBinaryLength(package_)/1024+1);
  } else {
    InspectorPrintf("Error assembling package\n");
    return -1;
  }
  
  return 0;
}


/*---------------------------------------------------------------------------*/
/**
 * Save the package into the project path, naming it after the project,
 */
int Dtk_Project::savePackage()
{
  InspectorPrintf("Writing package to %s\n", getPackageName());
  if (!NewtRefIsBinary(package_)) {
    InspectorPrintf("Error: no package to write\n");
    return -1;
  }
  
  int size = NewtBinaryLength(package_);
  uint8_t *data = NewtRefToBinary(package_);
  if (!size || !data) {
    InspectorPrintf("Error: package is empty\n");
    return -1;
  }
  
  FILE *f = fopen(getPackageName(), "wb");
  if (!f) {
    InspectorPrintf("Error opening file: %s\n", Flmm_Message::system_message());
    return -1;
  }
  if (fwrite(data, 1, size, f)!=(size_t)size) {
    InspectorPrintf("Error writing file: %s\n", Flmm_Message::system_message());
    fclose(f);
    return -1;
  }
  fclose(f);
  InspectorPrintf("Done.\n");
  
  return 0;
}

/* Prologue, predefined constants:
 home         The path name of the folder containing the open project file
 kAppName     The application name you specify through the Output section 
              of the Application tab of the Settings dialog box
 kAppString		The application symbol, which you specify through the Output
              section of the Application tab of the Settings dialog box 
              stored as a string instead of as a symbol
 kAppSymbol		The application symbol you specify through the Output 
              section of the Application tab of the Settings dialog box 
 kDebugOn     True if Compile for Debugging is checked in the Project 
              tab of the Settings dialog box
 kIgnoreNativeKeyword	True if Ignore Native Keyword is checked in the 
              Project tab of the Settings dialog box
 kPackageName	The package name you specify through the Package tab of 
              the Settings dialog box
 kProfileOn		True if Compile for Profiling is checked in the Project 
              tab of the Settings dialog box
 language     The Language string specified through the Project tab of 
              the Settings dialog box
 layout_/filename/	A reference to the view hierarchy of the processed layout
              file named filename
 streamFile_/filename/	A reference to the contents of a processed stream 
              file named filename
 */

/*---------------------------------------------------------------------------*/
void Dtk_Project::close()
{
}

/*---------------------------------------------------------------------------*/
int Dtk_Project::isDirty()
{
  return 0;
}

/*----------------  -----------------------------------------------------------*/
/**
 * Save the collected documents as a single Newt Script.
 */
int Dtk_Project::write(Dtk_Script_Writer &sw)
{
  char buf[1024], fn[2048];
  fl_filename_absolute(fn, 2047, filename());
# ifdef  WIN32
  char dbuf[10], tbuf[10];
  sprintf(buf, "// Text of project %s written on: %s %s\n", 
          fn, strdate(dbuf), strtime(tbuf));
# else
  time_t now;
  time(&now);
  sprintf(buf, "// Text of project %s written on: %s\n", fn, ctime(&now));
# endif
  sw.put(buf);
  
  dtkPlatform->writeConstants(sw);
  
  int i, n = documentList_->size();
  for (i=0; i<n; ++i) {
    documentList_->at(i)->write(sw);
  }
  
  Dtk_Document *mm = documentList_->getMainDocument();
  if (mm) {
    mm->writeTheForm(sw);
  }
  sw.put("dtkHelperFunc := func() begin end;\n");
  sw.put("DefGlobalVar('theBase, dtkHelperFunc.argFrame._nextArgFrame);\n");
  sw.put("return dtkHelperFunc;\n");
  
  return 0;
}


//
// End of "$Id$".
//
