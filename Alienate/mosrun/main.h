//
//  main.h
//  mosrun
//
//  Created by Matthias Melcher on 30.07.13.
//  Copyright (c) 2013 Matthias Melcher. All rights reserved.
//

#ifndef mosrun_main_h
#define mosrun_main_h


#define MOS_BOUNDS_CHECK 1

#define MOS_O_RDONLY      0x00      /* Open for reading only.            */
#define MOS_O_WRONLY      0x01      /* Open for writing only.            */
#define MOS_O_RDWR        0x02      /* Open for reading & writing.       */
#define MOS_O_APPEND      0x08      /* Write to the end of the file.     */
#define MOS_O_RSRC        0x10      /* Open the resource fork.           */
#define MOS_O_ALIAS       0x20      /* Open alias file.                  */
#define MOS_O_CREAT      0x100      /* Open or create a file.            */
#define MOS_O_TRUNC      0x200      /* Open and truncate to zero length. */
#define MOS_O_EXCL       0x400      /* Create file only; fail if exists. */
#define MOS_O_BINARY     0x800      /* Open as a binary stream.          */
#define MOS_O_NRESOLVE  0x4000      /* Don't resolve any aliases.        */


typedef unsigned int mosPtr;

typedef unsigned int mosHandle;

typedef unsigned int uint;

typedef void (*mosTrap)(unsigned short);

#endif
