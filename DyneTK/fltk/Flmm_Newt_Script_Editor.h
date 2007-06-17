
#ifndef FLMM_NEWT_SCRIPT_EDITOR_H
#define FLMM_NEWT_SCRIPT_EDITOR_H

#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>

class Flmm_Newt_Script_Editor : public Fl_Text_Editor
{
public:
    Flmm_Newt_Script_Editor(int xx, int yy, int ww, int hh);
private:
    Fl_Text_Buffer  * textbuf;
    Fl_Text_Buffer  * stylebuf;
    int loading;
    int changed;
    static void style_update_(int pos, int nInserted,
        int nDeleted, int,	const char *, void *cbArg);
    void style_update(int pos, int nInserted,
        int nDeleted);
    static void changed_cb_(int, int nInserted, int nDeleted,int, const char*, void* v);
    void changed_cb(int nInserted, int nDeleted);

};

#endif