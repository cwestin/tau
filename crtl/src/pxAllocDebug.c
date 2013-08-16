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

#ifndef PX_STRING_H
#include <string.h>
#define PX_STRING_H
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
    size_t size;

    pxAlignAll data; // user data begins here
} pxAllocDebug_Item;


static void *pxAllocDebug_alloc(pxAlloc *pAlloc, size_t size, int flag)
{
    pxAllocDebug_s *const pThis =
        PXINTERFACE_STRUCT(pAlloc, pxAllocDebug_s, pAllocVt);

    pxAllocDebug_Item *const pItem = (pxAllocDebug_Item *)
        PXALLOC_alloc(pThis->pAlloc,
                      offsetof(pxAllocDebug_Item, data) + size, flag);
    if (!pItem)
        return NULL;

    pxDllInit(&pItem->link);
    pItem->sequence = pThis->sequence++;
    pItem->pOwner = pThis;
    pItem->size = size;

    pxDllAddLast(&pThis->list, &pItem->link);

    return &pItem->data;
}

static inline void pxAllocDebug_shred(pxAllocDebug_Item *const pItem)
{
    // wipe out the content
    memset(pItem, 0xca, offsetof(pxAllocDebug_Item, data) + pItem->size);
}

static void pxAllocDebug_free(pxFree *pFree, void *p)
{
    if (p == NULL)
        pxExit("pxAllocDebug_free: Attempt to free NULL\n");

    pxAllocDebug_s *const pThis =
        PXINTERFACE_STRUCT(pFree, pxAllocDebug_s, pFreeVt);
    pxAllocDebug_Item *const pItem = (pxAllocDebug_Item *)
        (((char *)p) - offsetof(pxAllocDebug_Item, data));

    if (pItem->pOwner != pThis)
        pxExit("pxAllocDebug_free: Attempt to free non-member\n");

    // remove it from the list
    pxDllRemove(&pItem->link);

    pxAllocDebug_shred(pItem);

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
            pxAllocDebug_Item *const pItem =
                PXDLL_STRUCT(pLink, pxAllocDebug_Item, link);
            pxDllRemove(&pItem->link);
            pxAllocDebug_shred(pItem);
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

static const pxObjectInterface pxAllocDebug_interfaces[] =
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
    pxAllocDebug_destroy,
    pxObject_cloneForbidden,

    sizeof(pxAllocDebug_interfaces)/sizeof(pxAllocDebug_interfaces[0]),
    pxAllocDebug_interfaces,

    sizeof(pxAllocDebug_s),
    offsetof(pxAllocDebug_s, objectStruct.pObjectVt),
    0,
    NULL,
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
