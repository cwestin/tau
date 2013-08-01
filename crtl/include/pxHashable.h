
#ifndef PXHASHABLE_H
#define PXHASHABLE_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


struct pxHasher;

struct pxHashable;
typedef struct pxHashableVt
{
    pxInterfaceVt intefaceVt;

    void (*hash)(struct pxHashable *pI, struct pxHasher *pHasher);
} pxHashableVt;

typedef struct pxHashable
{
    const pxHashableVt *pHashableVt;
} pxHashable;

extern const char pxHashableName[];

#endif // PXHASHABLE_H
