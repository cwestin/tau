
#ifndef PXALLOCLOCAL_H
#define PXALLOCLOCAL_H

#ifndef PXALIGN_H
#include "pxAlign.h"
#endif


#define PXALLOCLOCAL_SPACE(size) \
    struct { pxAlignAll space[((size) + sizeof(pxAlignAll) - 1) / sizeof(pxAlignAll)]; }

#define PXALLOCLOCAL_INIT(ps) \
    pxAllocLocalInit((ps)->space, sizeof(*ps))

struct pxAlloc *pxAllocLocalInit(pxAlignAll *pSpace, size_t size);

#endif // PXALLOCLOCAL_H
