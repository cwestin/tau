
#ifndef PXMEMORY_H
#define PXMEMORY_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


struct pxMemory;

typedef struct
{
    PXINTERFACE_GET(pxMemory);

#define PXMEMORY_F_DIRTY      0x0001 // don't clear memory before returning
#define PXMEMORY_F_RETURN_OOM 0x0002 // return NULL on OOM
    void *(*alloc)(struct pxMemory *pMemory, size_t size, int flag);

    void (*free)(struct pxMemory *pMemory, void *p);
} pxMemoryVt;

typedef struct pxMemory
{
    const pxMemoryVt *pVt;
} pxMemory;

extern const char pxMemoryName[];

#define PXMEMORY_alloc(pM, size, flag) \
    ((*(pM)->pVt->alloc)(pM, size, flag))
#define PXMEMORY_free(pM, p) \
    ((*(pM)->pVt->free)(pM, p))


pxMemory *pxMemorySystemGet();

#endif // PXMEMORY_H
