
#ifndef PXHASHMAP_H
#define PXHASHMAP_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


struct pxHashMap;
typedef struct
{
    pxInterfaceVt interfaceVt;
} pxHashMapVt;

typedef struct pxHashMap
{
    const pxHashMapVt *pxHashMapVt;
} pxHashMap;

extern const char pxHashMapName[];


#endif // PXHASHMAP_H
