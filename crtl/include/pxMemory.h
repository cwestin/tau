
#ifndef PXMEMORY_H
#define PXMEMORY_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


struct pxMemory;

typedef struct
{
    PXINTERFACE_GET(pxMemory);
} pxMemoryVt;

typedef struct pxMemory
{
    const pxMemoryVt *pVt;
} pxMemory;

extern const char pxMemoryName[];

#endif // PXMEMORY_H
