
#ifndef PXALLOCEXTENT_H
#define PXALLOCEXTENT_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif


struct pxAlloc *pxAllocExtentCreate(struct pxAlloc *pAlloc, size_t initSize);

#endif // PXALLOCEXTENT_H
