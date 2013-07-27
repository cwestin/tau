
#ifndef PXALLOCDEBUG_H
#include "pxAllocDebug.h"
#endif

#ifndef PXALIGN_H
#include "pxAlign.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXDLL_H
#include "pxDll.h"
#endif

#ifndef PXFREE_H
#include "pxFree.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif


typedef struct
{
    pxAlloc *pAlloc;
    pxDllHead list;
    unsigned sequence;

    const pxAllocVt *pAllocVt;
    const pxFreeVt *pFreeVt;
    const pxAllocDebugVt *pAllocDebugVt;
    pxObjectStruct objectStruct;
} pxAllocDebug_s;

typedef struct
{
    pxDllLink link;
    int sequence;
    pxAllocDebug_s *pOwner;

    pxAlignAll data;
} pxAllocDebug_item;


static void *pxAllocDebug_alloc(pxAlloc *pAlloc, size_t size, int flag)
{
    pxAllocDebug_s *const pThis =
        PXINTERFACE_STRUCT(pAlloc, pxAllocDebug_s, pAllocVt);

    pxAllocDebug_item *const pItem = (pxAllocDebug_item *)
        PXALLOC_alloc(pThis->pAlloc,
                      offsetof(pxAllocDebug_item, data) + size, flag);
    if (!pItem)
        return NULL;

    pxDllInit(&pItem->link);
    pItem->sequence = pThis->sequence++;
    pItem->pOwner = pThis;

    pxDllAddLast(&pThis->list, &pItem->link);

    return &pItem->data;
}

static void pxAllocDebug_free(pxFree *pFree, void *p)
{
    if (p == NULL)
    {
        fprintf(stderr, "pxAllocDebug_free: Attempt to free NULL\n");
        exit(-1);
    }

    pxAllocDebug_s *const pThis =
        PXINTERFACE_STRUCT(pFree, pxAllocDebug_s, pFreeVt);
    pxAllocDebug_item *const pItem = (pxAllocDebug_item *)
        (((char *)p) - offsetof(pxAllocDebug_item, data));

    if (pItem->pOwner != pThis)
    {
        fprintf(stderr, "pxAllocDebug_free: Attempt to free non-member\n");
        exit(-1);
    }

    // remove it from the list
    pxDllRemove(&pItem->link);

    // if the allocator supports freeing, use it
    pxFree *const pAllocFree = PXINTERFACE_getInterface(pThis->pAlloc, pxFree);
    if (pAllocFree)
        PXFREE_free(pAllocFree, pItem);
}

static bool pxAllocDebug_isEmpty(pxAllocDebug *pAllocDebug)
{
    pxAllocDebug_s *const pThis =
        PXINTERFACE_STRUCT(pAllocDebug, pxAllocDebug_s, pAllocDebugVt);

    return pxDllIsEmpty(&pThis->list);
}

static const pxAllocVt pxAllocDebugAllocVt =
{
    {
        offsetof(pxAllocDebug_s, objectStruct.pObjectVt) - offsetof(pxAllocDebug_s, pAllocVt),
        pxObject_getInterface,
    },
    pxAllocDebug_alloc,
};

static const pxFreeVt pxAllocDebugFreeVt =
{
    {
        offsetof(pxAllocDebug_s, objectStruct.pObjectVt) - offsetof(pxAllocDebug_s, pFreeVt),
        pxObject_getInterface,
    },
    pxAllocDebug_free,
};

static const pxAllocDebugVt pxAllocDebugAllocDebugVt =
{
    {
        offsetof(pxAllocDebug_s, objectStruct.pObjectVt) - offsetof(pxAllocDebug_s, pAllocDebugVt),
        pxObject_getInterface,
    },
    pxAllocDebug_isEmpty,
};

static const pxObjectLookup pxAllocDebug_lookup[] =
{
    {pxAllocName, offsetof(pxAllocDebug_s, objectStruct.pObjectVt) - offsetof(pxAllocDebug_s, pAllocVt)},
    {pxFreeName, offsetof(pxAllocDebug_s, objectStruct.pObjectVt) - offsetof(pxAllocDebug_s, pFreeVt)},
    {pxAllocDebugName, offsetof(pxAllocDebug_s, objectStruct.pObjectVt) - offsetof(pxAllocDebug_s, pAllocDebugVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxAllocDebugObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    sizeof(pxAllocDebug_lookup)/sizeof(pxAllocDebug_lookup[0]),
    pxAllocDebug_lookup,
    pxObject_destroy,
    pxObject_addMixin,
};

pxAlloc *pxAllocDebugCreate(pxAlloc *pAlloc)
{
    pxAllocDebug_s *const pAL =
        PXALLOC_alloc(pAlloc, sizeof(pxAllocDebug_s), PXALLOC_F_DIRTY);

    pAL->pAlloc = pAlloc;
    pxDllInit(&pAL->list);
    pAL->sequence = 0;

    pAL->pAllocVt = &pxAllocDebugAllocVt;
    pAL->pFreeVt = &pxAllocDebugFreeVt;
    pAL->pAllocDebugVt = &pxAllocDebugAllocDebugVt;
    pxObjectStructInit(&pAL->objectStruct, &pxAllocDebugObjectVt);

    return (pxAlloc *)&pAL->pAllocVt;
}
