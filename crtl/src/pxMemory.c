
#ifndef PXMEMORY_H
#include "pxMemory.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif

#ifndef PX_STRINGS_H
#include <strings.h>
#define PX_STRINGS_H
#endif


const char pxMemoryName[] = "pxMemory";

/*
There's only one instance of the System memory object
*/

typedef struct
{
    const pxMemoryVt *pMemoryVt;
    const pxObjectVt *pObjectVt;
} pxMemorySystem_s;

static void *pxMemorySystem_alloc(
    pxMemory *const pMemory, size_t size, int flag)
{
    void *p = malloc(size);
    if (p == NULL)
    {
        if (flag & PXMEMORY_F_RETURN_OOM)
            return NULL;

        fprintf(stderr, "pxMemorySystem_alloc: OOM\n");
        exit(-1);
    }

    if (!(flag & PXMEMORY_F_DIRTY))
        bzero(p, size);

    return p;
}

static void pxMemorySystem_free(pxMemory *const pMemory, void *p)
{
    if (p == NULL)
    {
        fprintf(stderr, "pxMemorySystem_free: Attempt to free NULL\n");
        exit(-1);
    }

    free(p);
}

static const pxMemoryVt memorySystemVt =
{
    {
        offsetof(pxMemorySystem_s, pObjectVt) - offsetof(pxMemorySystem_s, pMemoryVt),
        pxObject_getInterface,
    },
    pxMemorySystem_alloc,
    pxMemorySystem_free,
};

static const pxObjectLookup memorySystemTable[] =
{
    {pxMemoryName, offsetof(pxMemorySystem_s, pObjectVt) - offsetof(pxMemorySystem_s, pMemoryVt)},
    {pxObjectName, 0},
};

static const pxObjectVt memorySystemObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    sizeof(memorySystemTable)/sizeof(memorySystemTable[0]),
    memorySystemTable,
    NULL, // TODO
};

static const pxMemorySystem_s pxMemorySystem_i =
{
    &memorySystemVt,
    &memorySystemObjectVt,
};

pxMemory *pxMemorySystemGet()
{
    return (pxMemory *)&pxMemorySystem_i.pMemoryVt;
}
