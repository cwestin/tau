
#ifndef PXMEMORY_H
#define PXMEMORY_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


struct pxMemory;
typedef struct pxMemoryVt
{
    pxInterfaceVt interfaceVt;

#define PXMEMORY_F_DIRTY      0x0001 // don't clear memory before returning
#define PXMEMORY_F_RETURN_OOM 0x0002 // return NULL on OOM
    void *(*alloc)(struct pxMemory *pMemory, size_t size, int flag);

    void (*free)(struct pxMemory *pMemory, void *p);
} pxMemoryVt;

typedef struct pxMemory
{
    const pxMemoryVt *const pVt;
} pxMemory;

extern const char pxMemoryName[];

#define PXMEMORY_alloc(pI, size, flag) \
    ((*(pI)->pVt->alloc)((pI), (size), (flag)))
#define PXMEMORY_free(pI, p) \
    ((*(pI)->pVt->free)((pI), (p)))


void pxMemorySystemInit();
pxMemory *pxMemorySystemGet();

#endif // PXMEMORY_H
