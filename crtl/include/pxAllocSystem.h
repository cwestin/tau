
#ifndef PXALLOCSYSTEM_H
#define PXALLOCSYSTEM_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


void pxAllocSystemInit();

// also supports pxFree
struct pxAlloc *pxAllocSystemGet();

#endif // PXALLOCSYSTEM_H
