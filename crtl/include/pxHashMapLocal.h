
#ifndef PXHASHMAPLOCAL_H
#define PXHASHMAPLOCAL_H

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif


struct pxAlloc;
struct pxInterface;
struct pxHashMap *pxHashMapLocalCreate(
    struct pxAlloc *pAlloc, struct pxInterface *pOwner,
    size_t initCap, size_t avgBucket);

#endif // PXHASHMAPLOCAL_H
