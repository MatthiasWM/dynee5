//
// "$Id$"
//
// Dtk_Script_Writer header file for the Dyne Toolkit.
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

#ifndef DTK_SCRIPT_WRITER_H
#define DTK_SCRIPT_WRITER_H


#include <stdio.h>

class Dtk_Project;

extern "C" {
#include "NewtType.h"
}

/** Helper class to write the final source code to disk.
 */
class Dtk_Script_Writer
{
public:

    /** Initialize the script code writer
     */
                    Dtk_Script_Writer(Dtk_Project *proj);

    /** Remove the script code writer.
     */
                    ~Dtk_Script_Writer();

    /** Close the currently open destination.
     */
    void            close();

    /** Open the script destination as a text file.
     *
     * \retval  0, if successful
     * \retval  negative if any part of the operation failed  
     */
    int             open(const char *filename);

    /** Copy the C-string verbatim into the destination stream.
     *
     * \retval  0, if successful
     * \retval  negative if any part of the operation failed  
     */
    int             put(const char *text, int n = -1);

    int             viewCount;

private:

    /// we must be part of a project
    Dtk_Project     * project_;

    /// write all text to this file
    FILE            * file_;
};


#endif

//
// End of "$Id$".
//
