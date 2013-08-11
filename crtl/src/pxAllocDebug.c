/*
  tau - http://github.com/cwestin/tau
  Copyright 2013 Chris Westin

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

#ifndef PXALLOCDEBUG_H
#include "pxAllocDebug.h"
#endif

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
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

#ifndef PXEXIT_H
#include "pxExit.h"
#endif

#ifndef PXFREE_H
#include "pxFree.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


const char pxAllocDebugName[] = "pxAllocDebug";


typedef struct
{
    pxAlloc *pAlloc; // underlying allocator
    pxDllHead list; // list of allocated pxAllocDebug_item
    unsigned sequence; // allocation sequence number

    const pxAllocVt *pAllocVt;
    const pxFreeVt *pFreeVt;
    const pxAllocDebugVt *pAllocDebugVt;
    pxObjectStruct objectStruct;
} pxAllocDebug_s;

typedef struct
{
    pxDllLink link; // link on list of allocated pieces
    int sequence; // immutable once set
    pxAllocDebug_s *pOwner; // owning allocator

    pxAlignAll data; // user data begins here
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
        pxExit("pxAllocDebug_free: Attempt to free NULL\n");

    pxAllocDebug_s *const pThis =
        PXINTERFACE_STRUCT(pFree, pxAllocDebug_s, pFreeVt);
    pxAllocDebug_item *const pItem = (pxAllocDebug_item *)
        (((char *)p) - offsetof(pxAllocDebug_item, data));

    if (pItem->pOwner != pThis)
        pxExit("pxAllocDebug_free: Attempt to free non-member\n");

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

static unsigned pxAllocDebug_countPieces(pxAllocDebug *pAllocDebug)
{
    pxAllocDebug_s *const pThis =
        PXINTERFACE_STRUCT(pAllocDebug, pxAllocDebug_s, pAllocDebugVt);

    unsigned count = 0;
    for(pxDllLink *pLink = pxDllGetFirst(&pThis->list); pLink;
        pLink = pxDllGetNext(&pThis->list, pLink))
    {
        ++count;
    }

    return count;
}

static void pxAllocDebug_destroy(pxObject *pObject)
{
    pxAllocDebug_s *const pThis =
        PXINTERFACE_STRUCT(pObject, pxAllocDebug_s, objectStruct.pObjectVt);

    // if the underlying allocator supports freeing, free everything
    pxFree *const pFree = PXINTERFACE_getInterface(pThis->pAlloc, pxFree);
    if (pFree)
    {
        pxDllLink *pLink;
        while((pLink = pxDllGetFirst(&pThis->list)))
        {
            pxAllocDebug_item *const pItem =
                PXDLL_STRUCT(pLink, pxAllocDebug_item, link);
            pxDllRemove(&pItem->link);
            PXFREE_free(pFree, pItem);
        }
    }

    pxObject_destroy(pObject);

    if (pFree)
        PXFREE_free(pFree, pThis);
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
    pxAllocDebug_countPieces,
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
    pxAllocDebug_destroy,
};

pxAlloc *pxAllocDebugCreate(pxAlloc *pAlloc, pxInterface *pOwner)
{
    pxAllocDebug_s *const pAL =
        PXALLOC_alloc(pAlloc, sizeof(pxAllocDebug_s), PXALLOC_F_DIRTY);

    pAL->pAlloc = pAlloc;
    pxDllInit(&pAL->list);
    pAL->sequence = 0;

    pAL->pAllocVt = &pxAllocDebugAllocVt;
    pAL->pFreeVt = &pxAllocDebugFreeVt;
    pAL->pAllocDebugVt = &pxAllocDebugAllocDebugVt;
    pxObjectStructInit(&pAL->objectStruct, &pxAllocDebugObjectVt, pOwner);

    return (pxAlloc *)&pAL->pAllocVt;
}
