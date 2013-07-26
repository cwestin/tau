
#ifndef PXALLOC_H
#define PXALLOC_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif


struct pxAlloc;
typedef struct pxAllocVt
{
    pxInterfaceVt interfaceVt;

#define PXALLOC_F_DIRTY      0x0001 // don't clear memory before returning
#define PXALLOC_F_RETURN_OOM 0x0002 // return NULL on OOM
    void *(*alloc)(struct pxAlloc *pAlloc, size_t size, int flag);
} pxAllocVt;

typedef struct pxAlloc
{
    const pxAllocVt *const pVt;
} pxAlloc;

extern const char pxAllocName[];

#define PXALLOC_alloc(pI, size, flag) \
    ((*(pI)->pVt->alloc)((pI), (size), (flag)))

#endif // PXALLOC_H
