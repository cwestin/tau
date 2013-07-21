
#ifndef PXMEMORY_H
#define PXMEMORY_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


typedef struct pxMemoryVt
{
    pxInterfaceVt interfaceVt;

#define PXMEMORY_F_DIRTY      0x0001 // don't clear memory before returning
#define PXMEMORY_F_RETURN_OOM 0x0002 // return NULL on OOM
    void *(*alloc)(struct pxMemoryVt *const *const pMemory, size_t size, int flag);

    void (*free)(struct pxMemoryVt *const *const pMemory, void *p);
} pxMemoryVt, *const pxMemory;

extern const char pxMemoryName[];

#define PXMEMORY_alloc(pI, size, flag) \
    ((*(*(pI))->alloc)(pI, size, flag))
#define PXMEMORY_free(pI, p) \
    ((*(*(pI))->free)(pI, p))


pxMemory *pxMemorySystemGet();

#endif // PXMEMORY_H
