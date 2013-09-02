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

#ifndef PXALLOCEXTENT_H
#include "pxAllocExtent.h"
#endif

#ifndef PXALIGN_H
#include "pxAlign.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
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


typedef struct pxAllocExtent_extent
{
    struct pxAllocExtent_extent *pNext;

    size_t size;
    char *pAvail;
    pxAlignAll data[1]; // must be last
} pxAllocExtent_extent;

static void pxAllocExtent_extentInit(pxAllocExtent_extent *pE, size_t size)
{
    pE->pNext = NULL;
    pE->size = size;
    pE->pAvail = (char *)&pE->data[0];
}

typedef struct
{
    pxAlloc *pAlloc; // underlying allocator
    pxAllocExtent_extent *pExtents; // list of extents

    const pxAllocVt *pAllocVt;
    pxObjectStruct objectStruct;

    pxAllocExtent_extent firstExtent;
} pxAllocExtent_s;


static void *pxAllocExtent_alloc(pxAlloc *pI, size_t size, int flag)
{
    if (size <= 0)
        pxExit("pxAllocExtent_alloc: request to allocate size <= zero");

    pxAllocExtent_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxAllocExtent_s, pAllocVt);

    // check for space in the first extent
    pxAllocExtent_extent *pE = pThis->pExtents;
    size_t remains = pE->size - (pE->pAvail - (char *)&pE->data[0]);
    if (remains >= size)
    {
        void *const p = pE->pAvail;
        pE->pAvail += PXALIGN_SIZE(size);
        return p;
    }

    // allocate a new extent
    
    // what would the next size be?
    size_t nextSize = PXALIGN_SIZE(2 * pE->size);
    if (nextSize < size)
        nextSize = size;

    // allocate and initialize the new extent
    pE = (pxAllocExtent_extent *)PXALLOC_alloc(pThis->pAlloc,
            offsetof(pxAllocExtent_extent, data) + nextSize, PXALLOC_F_DIRTY);
    pxAllocExtent_extentInit(pE, nextSize);

    // add the new extent to the list of extents
    pE->pNext = pThis->pExtents;
    pThis->pExtents = pE;

    // the request is satisfied by the first available space
    void *const p = &pE->data[0];
    pE->pAvail = ((char *)p) + PXALIGN_SIZE(size);
    return p;
}

static void pxAllocExtent_destroy(pxObject *pI)
{
    pxAllocExtent_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxAllocExtent_s, objectStruct.pObjectVt);

    // Anything that has been mixed in to this object should have come
    // from the allocator itself, so we need to destroy all of those first.
    // Note this order is unusual.
    pxObject_destroy(pI);

    // If the underlying allocator supports deallocation, then use it
    // here on our extents. Take care not to free the last extent, as that
    // points into our own structure.
    pxFree *const pFree = PXINTERFACE_getInterface(pThis->pAlloc, pxFree);
    if (pFree)
    {
        pxAllocExtent_s *const pThis =
            PXINTERFACE_STRUCT(pI, pxAllocExtent_s, objectStruct.pObjectVt);

        pxAllocExtent_extent *pE;
        while((pE = pThis->pExtents) != &pThis->firstExtent)
        {
            // remove it from the list
            pThis->pExtents = pE->pNext;

            // free it
            PXFREE_free(pFree, pE);
        }

        // free ourself
        PXFREE_free(pFree, pThis);
    }
}

static const pxAllocVt pxAllocExtentAllocVt =
{
    {
        offsetof(pxAllocExtent_s, objectStruct.pObjectVt) -
        offsetof(pxAllocExtent_s, pAllocVt),
        pxObject_getInterface,
    },
    pxAllocExtent_alloc,
};

static const pxObjectInterface pxAllocExtent_interfaces[] =
{
    {pxAllocName, offsetof(pxAllocExtent_s, objectStruct.pObjectVt) - offsetof(pxAllocExtent_s, pAllocVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxAllocExtentObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxAllocExtent_destroy,
    pxObject_cloneForbidden,
    pxObjectStruct_dupThis,

    sizeof(pxAllocExtent_interfaces)/sizeof(pxAllocExtent_interfaces[0]),
    pxAllocExtent_interfaces,

    sizeof(pxAllocExtent_s),
    offsetof(pxAllocExtent_s, objectStruct),
    0,
    NULL,
};

pxAlloc *pxAllocExtentCreate(
    pxAlloc *const pAlloc, size_t initSize, pxInterface *const pOwner)
{
    // ensure minimum size to make this worthwhile
    if (initSize < sizeof(pxAlignAll))
        initSize = sizeof(pxAlignAll);

    pxAllocExtent_s *const pAE =
        PXALLOC_alloc(pAlloc,
                      offsetof(pxAllocExtent_s, firstExtent.data) + initSize,
                      PXALLOC_F_DIRTY);
    pAE->pAllocVt = &pxAllocExtentAllocVt;
    pxObjectStructInit(&pAE->objectStruct, &pxAllocExtentObjectVt, pOwner);
    pAE->pAlloc = pAlloc;
    pxAllocExtent_extentInit(&pAE->firstExtent, initSize);
    pAE->pExtents = &pAE->firstExtent;

    return (pxAlloc *)&pAE->pAllocVt;
}
