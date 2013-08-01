
#ifndef PXHASHABLEVAR_H
#define PXHASHABLEVAR_H

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


struct pxComparable;
struct pxHashable;

// this is exposed so that it can be used as a local variable for now
// eventually these should come from stack frame allocators
typedef struct
{
    const void *p;
    size_t size;

    const struct pxComparableVt *pComparableVt;
    const struct pxHashableVt *pHashableVt;
    pxObjectStruct objectStruct;
} pxHashableVar_s;


struct pxHashable *pxHashableVarInit(
    pxHashableVar_s *pThis, const void *p, size_t size,
    pxInterface *pOwner);

#endif // PXHASHABLEVAR_H
