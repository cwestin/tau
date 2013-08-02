
#ifndef PXALLOCLOCAL_H
#include "pxAllocLocal.h"
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

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


typedef struct
{
    char *p; // pointer to the next available space
    size_t avail; // the amount of space left

    const pxAllocVt *pAllocVt;
    pxObjectStruct objectStruct;

    pxAlignAll space[1]; // available space trails off the end
} pxAllocLocal_s;

static void *pxAllocLocal_alloc(pxAlloc *pI, size_t size, int flag)
{
    if (size <= 0)
    {
        fprintf(stderr, "pxAllocLocal_alloc: requested size is <= 0\n");
        exit(-1);
    }

    pxAllocLocal_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxAllocLocal_s, pAllocVt);
    if (pThis->avail < size)
    {
        if (flag & PXALLOC_F_RETURN_OOM)
            return NULL;

        fprintf(stderr, "pxAllocLocal_alloc: insufficient space available; ");
        fprintf(stderr, "avail = %lu, size = %lu\n", pThis->avail, size);
        exit(-1);
    }

    const size_t alignedSize = PXALIGN_SIZE(size);
    void *p = pThis->p;
    pThis->p += alignedSize;
    if (alignedSize >= pThis->avail)
        pThis->avail = 0;
    else
        pThis->avail -= alignedSize;

    if (!(flag & PXALLOC_F_DIRTY))
        bzero(p, size);

    return p;
}

static const pxAllocVt pxAllocLocalAllocVt =
{
    {
        offsetof(pxAllocLocal_s, objectStruct.pObjectVt) -
        offsetof(pxAllocLocal_s, pAllocVt),
        pxObject_getInterface,
    },
    pxAllocLocal_alloc,
};

static const pxObjectLookup pxAllocLocal_lookup[] =
{
    {pxAllocName, offsetof(pxAllocLocal_s, objectStruct.pObjectVt) - offsetof(pxAllocLocal_s, pAllocVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxAllocLocalObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    sizeof(pxAllocLocal_lookup)/sizeof(pxAllocLocal_lookup[0]),
    pxAllocLocal_lookup,
    pxObject_destroy,
};

pxAlloc *pxAllocLocalInit(pxAlignAll *pSpace, size_t size)
{
    if (size < sizeof(pxAllocLocal_s))
    {
        fprintf(stderr, "can't initialize local allocator\n");
        exit(1);
    }

    pxAllocLocal_s *const pThis = (pxAllocLocal_s *)pSpace;
    pThis->pAllocVt = &pxAllocLocalAllocVt;
    pxObjectStructInit(&pThis->objectStruct, &pxAllocLocalObjectVt, NULL);
    pThis->p = (char *)pThis->space;
    pThis->avail = size - offsetof(pxAllocLocal_s, space);

    return (pxAlloc *)&pThis->pAllocVt;
}
