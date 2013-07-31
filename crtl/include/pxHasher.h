
#ifndef PXHASHER_H
#define PXHASHER_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif


struct pxHasher;
typedef struct
{
    pxInterfaceVt interfaceVt;

    // methods on here are not virtual (i.e., they are final)
} pxHasherVt;

typedef struct pxHasher
{
    const pxHasherVt *pHasherVt;
} pxHasher;

extern const char pxHasherName[];

int PXHASHER_getHash(pxHasher *pI);
void PXHASHER_hashBytes(pxHasher *pI, const void *p, size_t length);

struct pxAlloc;
pxHasher *pxHasherCreate(struct pxAlloc *pAlloc, pxInterface *pOwner);

#endif // PXHASHER_H
