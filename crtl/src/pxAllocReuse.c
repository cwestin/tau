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


#ifndef PXALLOCREUSE_H
#include "pxAllocReuse.h"
#endif

#ifndef PX_STRINGS_H
#include <strings.h>
#define PX_STRINGS_H
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

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


const char pxAllocReuseName[] = "pxAllocReuse";


typedef struct
{
    pxAlloc *pAlloc; // underlying allocator
    pxDllHead list; // list of pieces to try reusing

    const pxAllocVt *pAllocVt;
    const pxAllocReuseVt *pAllocReuseVt;
    pxObjectStruct objectStruct;
} pxAllocReuse_s;

typedef struct
{
    size_t size;
    pxDllLink link;
} pxAllocReuse_item;


static void *pxAllocReuse_alloc(pxAlloc *pAlloc, size_t size, int flag)
{
    pxAllocReuse_s *const pThis =
        PXINTERFACE_STRUCT(pAlloc, pxAllocReuse_s, pAllocVt);

    // try to find something on the list first
    pxDllLink *pLink;
    while((pLink = pxDllGetFirst(&pThis->list)))
    {
        pxAllocReuse_item *const pItem =
            PXDLL_STRUCT(pLink, pxAllocReuse_item, link);

        // remove it from the list
        pxDllRemove(&pItem->link);

        // if we can use it, take further steps
        if (pItem->size >= size)
        {
            const size_t alignedSize = PXALIGN_SIZE(size);

            // if there's room for another allocation, split the piece
            if (pItem->size >= alignedSize + sizeof(pxAllocReuse_item))
            {
                pxAllocReuse_item *const pNewItem =
                    (pxAllocReuse_item *)(((char *)pItem) + alignedSize);
                pNewItem->size = pItem->size - alignedSize;
                pxDllInit(&pNewItem->link);
                pxDllAddFirst(&pThis->list, &pNewItem->link);
            }

            if (!(flag & PXALLOC_F_DIRTY))
                bzero(pItem, alignedSize);

            return pItem;
        }
    }


    // if we got here, then we didn't find anything usable, so fall back
    return PXALLOC_alloc(pThis->pAlloc, size, flag);
}

static void pxAllocReuse_reuse(pxAllocReuse *pAllocReuse, void *p, size_t size)
{
    if (p == NULL)
        pxExit("pxAllocReuse_free: Attempt to free NULL\n");

    // if the piece is too small, there's nothing we can do with it
    if (size < sizeof(pxAllocReuse_item))
        return;

    pxAllocReuse_s *const pThis =
        PXINTERFACE_STRUCT(pAllocReuse, pxAllocReuse_s, pAllocReuseVt);

    // initialize the item; it is already worst-case aligned
    pxAllocReuse_item *const pItem = (pxAllocReuse_item *)p;
    pxDllInit(&pItem->link);
    pItem->size = size;

    // add it to the list
    pxDllAddFirst(&pThis->list, &pItem->link);
}

static const pxAllocVt pxAllocReuseAllocVt =
{
    {
        offsetof(pxAllocReuse_s, objectStruct.pObjectVt) - offsetof(pxAllocReuse_s, pAllocVt),
        pxObject_getInterface,
    },
    pxAllocReuse_alloc,
};

static const pxAllocReuseVt pxAllocReuseAllocReuseVt =
{
    {
        offsetof(pxAllocReuse_s, objectStruct.pObjectVt) - offsetof(pxAllocReuse_s, pAllocReuseVt),
        pxObject_getInterface,
    },
    pxAllocReuse_reuse,
};

static const pxObjectInterface pxAllocReuse_interfaces[] =
{
    {pxAllocName, offsetof(pxAllocReuse_s, objectStruct.pObjectVt) - offsetof(pxAllocReuse_s, pAllocVt)},
    {pxAllocReuseName, offsetof(pxAllocReuse_s, objectStruct.pObjectVt) - offsetof(pxAllocReuse_s, pAllocReuseVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxAllocReuseObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxObject_destroy,
    pxObject_cloneForbidden,
    pxObjectStruct_dupThis,

    sizeof(pxAllocReuse_interfaces)/sizeof(pxAllocReuse_interfaces[0]),
    pxAllocReuse_interfaces,

    sizeof(pxAllocReuse_s),
    offsetof(pxAllocReuse_s, objectStruct),
    0,
    NULL,
};

pxAlloc *pxAllocReuseCreate(pxAlloc *pAlloc)
{
    pxAllocReuse_s *const pThis =
        PXALLOC_alloc(pAlloc, sizeof(pxAllocReuse_s), PXALLOC_F_DIRTY);

    pThis->pAlloc = pAlloc;
    pxDllInit(&pThis->list);

    pThis->pAllocVt = &pxAllocReuseAllocVt;
    pThis->pAllocReuseVt = &pxAllocReuseAllocReuseVt;
    pxObjectStructInit(&pThis->objectStruct, &pxAllocReuseObjectVt, NULL);

    return (pxAlloc *)&pThis->pAllocVt;
}
