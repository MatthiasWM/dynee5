//
//  CallGraph.h
//  Albert
//
//  Created by Matthias Melcher on 7/31/14.
//
//

#ifndef __Albert__CallGraph__
#define __Albert__CallGraph__

#include "db2src.h"

extern bool gCallGraphCreated;

extern void createCallGraphInformation();
extern void listAllEntryPoints();
extern void listAllEndPoints();
extern void writeCallsPerFunction();
extern void writeNumUniqueCallsPerFunction();
extern int writeCallGraph(FILE *f, unsigned int start, unsigned int depth);
extern void writeCallGraph(const char *filename, unsigned int start = 0xffffffff, unsigned int depth = 65536);

#endif /* defined(__Albert__CallGraph__) */
