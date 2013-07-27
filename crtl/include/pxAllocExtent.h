
#ifndef PXALLOCEXTENT_H
#define PXALLOCEXTENT_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif


struct pxAlloc;
struct pxInterface;

struct pxAlloc *pxAllocExtentCreate(
    struct pxAlloc *pAlloc, size_t initSize, struct pxInterface *pOwner);

#endif // PXALLOCEXTENT_H
