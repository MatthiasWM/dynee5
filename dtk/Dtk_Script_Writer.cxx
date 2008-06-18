//
// "$Id$"
//
// Dtk_Script_Writer implementation for the Dyne Toolkit.
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

#include "Dtk_Script_Writer.h"
#include "Dtk_Project.h"


#include <string.h>


/*---------------------------------------------------------------------------*/
Dtk_Script_Writer::Dtk_Script_Writer(Dtk_Project *proj)
:   project_(proj),
    file_(0L)
{
}


/*---------------------------------------------------------------------------*/
Dtk_Script_Writer::~Dtk_Script_Writer()
{
    close();
}


/*---------------------------------------------------------------------------*/
void Dtk_Script_Writer::close()
{
    if (file_) {
        fclose(file_);
        file_ = 0L;
    }
}


/*---------------------------------------------------------------------------*/
int Dtk_Script_Writer::open(const char *filename)
{
    viewCount = 0;

    if (file_)
        close();
    file_ = fopen(filename, "wb");
    if (!file_)
        return -1;
    return 0;
}

/*---------------------------------------------------------------------------*/
int Dtk_Script_Writer::put(const char *text, int n)
{
    int ret = -1;
    if (n==-1)
        n = strlen(text);
    if (file_) {
        if (fwrite(text, 1, n, file_)==(size_t)n)
            ret = -1;
    }
    return ret;
}

//
// End of "$Id$".
//
