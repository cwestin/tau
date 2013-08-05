
#ifndef PXALLOCREUSE_H
#define PXALLOCREUSE_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


// this interface can never be allowed to escape into tau library space
struct pxAllocReuse;
typedef struct
{
    pxInterfaceVt interfaceVt;

    void (*reuse)(struct pxAllocReuse *pAllocReuse, void *p, size_t size);
} pxAllocReuseVt;

typedef struct pxAllocReuse
{
    const pxAllocReuseVt *const pVt;
} pxAllocReuse;

extern const char pxAllocReuseName[];

#define PXALLOCREUSE_reuse(pI, p, size) \
    ((*(pI)->pVt->reuse)((pI), (p), (size)))


struct pxAlloc *pxAllocReuseCreate(struct pxAlloc *pAlloc);

#endif // PXALLOCREUSE_H
