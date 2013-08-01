
#ifndef PXHASHABLE_H
#define PXHASHABLE_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


struct pxHasher;

struct pxHashable;
typedef struct pxHashableVt
{
    pxInterfaceVt interfaceVt;

    void (*hash)(struct pxHashable *pI, struct pxHasher *pHasher);
} pxHashableVt;

typedef struct pxHashable
{
    const pxHashableVt *pVt;
} pxHashable;

extern const char pxHashableName[];

#define PXHASHABLE_hash(pI, pHasher) \
    ((*(pI)->pVt->hash)(pI, pHasher))

#endif // PXHASHABLE_H
