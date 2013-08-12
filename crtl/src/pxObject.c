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

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif

#ifndef PX_STRING_H
#include <string.h>
#define PX_STRING_H
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXALLOCEXTENT_H
#include "pxAllocExtent.h"
#endif

#ifndef PXALLOCSYSTEM_H
#include "pxAllocSystem.h"
#endif

#ifndef PXCMP_H
#include "pxCmp.h"
#endif

#ifndef PXEXIT_H
#include "pxExit.h"
#endif

#ifndef PXHASH_H
#include "pxHash.h"
#endif

#ifndef PXHM_H
#include "pxHm.h"
#endif


const char pxObjectName[] = "pxObject";

static pxInterface *pxObjectStruct_getInterface(
    const pxObjectStruct *const pObjectStruct, const char *const pName)
{
    // check through interfaces this object implements directly
    const pxObjectInterface *pInterface = pObjectStruct->pObjectVt->pInterface;
    for(int i = pObjectStruct->pObjectVt->nInterface; i; ++pInterface, --i)
    {
        if (pInterface->pName == pName)
        {
            return (pxInterface *)
                (((char *)&pObjectStruct->pObjectVt) -
                 pInterface->interfaceOffset);
        }
    }

    // check on our mixins
    for(const pxObjectStruct *pMixin = pObjectStruct->pMixinList; pMixin;
        pMixin = pMixin->pNextMixin)
    {
        // For this lookup, we use the direct implementation, otherwise
        // the mixin will use the owner pointer, causing infinite recursion
        pxInterface *const pI = pxObjectStruct_getInterface(pMixin, pName);
        if (pI)
            return pI;
    }

    // if we got here, we didn't find it
    return NULL;
}

pxInterface *pxObject_getInterface(pxInterface *pI, const char *const pName)
{
    pxObject *const pObject =
        (pxObject *)(((char *)pI) + pI->pVt->interfaceVt.pxObjectOffset);
    pxObjectStruct *const pObjectStruct =
        PXINTERFACE_STRUCT(pObject, pxObjectStruct, pObjectVt);
    
    // if we have an owner, i.e., are part of an aggregate, delegate to owner
    if (pObjectStruct->pOwner)
    {
        // we can't use the macro here because it relies on symbol manipulation
        return pObjectStruct->pOwner->pObjectVt->interfaceVt.getInterface(
            (pxInterface*)&pObjectStruct->pOwner->pObjectVt, pName);
    }

    // check this object and its mixins
    return pxObjectStruct_getInterface(pObjectStruct, pName);
}

static void pxObject_destroy_mixin(pxObjectStruct *pO)
{
    // recursively destroy mixins on the list from last to first
    if (pO->pNextMixin)
    {
        pxObject_destroy_mixin(pO->pNextMixin);

        // remove them from the list as we go (prevents interface lookups on
        // now-dysfunctional objects)
        pO->pNextMixin = NULL;
    }

    // destroy this mixin itself
    PXOBJECT_destroy((pxObject *)&pO->pObjectVt);
}

void pxObject_destroy(pxObject *pI)
{
    pxObjectStruct *const pThis =
        PXINTERFACE_STRUCT(pI, pxObjectStruct, pObjectVt);

    // mixins have to be destroyed in the reverse order, and need to be kept
    // around until they are destroyed, in case they depend on each other
    if (pThis->pMixinList)
    {
        pxObject_destroy_mixin(pThis->pMixinList);
    }
}


typedef struct
{
    pxObject *pNew;
    pxObject *pOld;

    pxHmEntry hmEntry;
} pxObject_clone_Item;

static const pxHmDope pxObject_clone_hmDope =
{
    /* avgBucket */ 3,
    /* keyOffset */ offsetof(pxObject_clone_Item, pOld),
    /* hash */ pxHashStructStar,
    /* cmp */ pxCmpStructStar,
};

pxHmMap *pxObject_clone_mapInit(pxHmMap *pMap, pxAlloc *pAlloc)
{
    if (pMap != NULL)
        return pMap;

    if (pAlloc == NULL)
    {
        pxAlloc *const pAllocS = pxAllocSystemGet();
        pAlloc = pxAllocExtentCreate(pAllocS, 1024, NULL);
    }

    pMap = (pxHmMap *)PXALLOC_alloc(pAlloc, sizeof(*pMap), PXALLOC_F_DIRTY);
    pxHmMapInit(pMap, &pxObject_clone_hmDope, pAlloc, 8);
    return pMap;
}

typedef struct
{
    pxObjectStruct *pThis; // the object to be cloned
    pxAlloc *pAlloc; // the allocator to use for the cloning
    pxHmMap *pMap; // the context map with objects that are already cloned
} pxObject_clone_create_ctx;

static pxHmEntry *pxObject_clone_create(void *const pctx, pxAlloc *const pAlloc)
{
    pxObject_clone_create_ctx *const pCtx = (pxObject_clone_create_ctx *)pctx;
    const pxObjectVt *const pObjectVt = pCtx->pThis->pObjectVt;

    // allocate a new instance
    void *const pOld =
        ((char *)&pCtx->pThis->pObjectVt) - pObjectVt->objectOffset;
    void *const pNew =
        PXALLOC_alloc(pCtx->pAlloc, pObjectVt->size, PXALLOC_F_DIRTY);

    // raw copy
    memcpy(pNew, pOld, pObjectVt->size);

    // adjust object state
    pxObjectStruct *const pNewThis =
        (pxObjectStruct *)(((char *)pNew) + pObjectVt->objectOffset);

    if (pCtx->pThis->pMixinList)
    {
        // TODO clone my own mixins
        pxExit("pxObject_clone: mixin cloning unimplemented\n");
    }

    // copy any other objects referenced by pointer members
    if (pObjectVt->nMember)
    {
        // TODO clone my members
        pxExit("pxObject_clone: member pointer copy unimplemented\n");
    }

    // set up and return the new hash map entry
    pxObject_clone_Item *const pItem =
        PXALLOC_alloc(pAlloc, sizeof(pxObject_clone_Item), PXALLOC_F_DIRTY);
    pItem->pNew = (pxObject *)&pNewThis->pObjectVt;
    pItem->pOld = (pxObject *)&pCtx->pThis->pObjectVt;
    return &pItem->hmEntry;
}

pxInterface *pxObject_clone(pxObject *const pI, pxAlloc *pAlloc,
                            const char *const pIName, pxHmMap *const pMap)
{
    pxObject_clone_create_ctx ctx; // local variables for closure
    ctx.pThis = PXINTERFACE_STRUCT(pI, pxObjectStruct, pObjectVt);

    // if this isn't the root object, start with that instead
    if (ctx.pThis->pOwner)
    {
        return PXOBJECT_clone(
            (pxObject *)&ctx.pThis->pOwner->pObjectVt, pAlloc, pIName, pMap);
    }

    // find the map to use for the lookup; finish other closure setup
    ctx.pMap = pxObject_clone_mapInit(pMap, NULL);
    ctx.pAlloc = pAlloc;

    // find the cloned object in the context hash map, creating it if necessary
    pxHmEntry *const pEntry =
        pxHmMapFind(ctx.pMap, &pI, pxObject_clone_create, &ctx);
    pxObject_clone_Item *const pItem =
        PXHM_STRUCT(pEntry, pxObject_clone_Item, hmEntry);

    // if we created the local map, clean it up
    if (!pMap)
    {
        pxAlloc *const pMapAlloc = pxHmMapGetAlloc(ctx.pMap);
        pxObject *const pAllocO = PXINTERFACE_getInterface(pMapAlloc, pxObject);
        PXOBJECT_destroy(pAllocO);
    }

    // return the requested interface on the new object
    return pItem->pNew->pVt->interfaceVt.getInterface(
        (pxInterface *)&pItem->pNew->pVt->interfaceVt, pIName);
}

pxInterface *pxObject_cloneForbidden(pxObject *pI, pxAlloc *pAlloc,
                                     const char *const pIName, pxHmMap *pMap)
{
    pxExit("pxObject_clone: attempt to clone uncloneable object\n");

    return NULL;
}

void pxObjectStructInit(
    pxObjectStruct *pObjectStruct, const pxObjectVt *pVt,
    pxInterface *pIOwner)
{
    pObjectStruct->pObjectVt = pVt;
    pObjectStruct->pNextMixin = NULL;
    pObjectStruct->pMixinList = NULL;
    pObjectStruct->pOwner = NULL;

    // if there's an owner add this to it
    if (pIOwner)
    {
        pxObject *const pOOwner = PXINTERFACE_getInterface(pIOwner, pxObject);
        pxObjectStruct *const pOwner =
            PXINTERFACE_STRUCT(pOOwner, pxObjectStruct, pObjectVt);

        pObjectStruct->pOwner = pOwner;

        // this new object has to go on the end of the owner's mixin list
        pxObjectStruct **ppMixin;
        for(ppMixin = &pOwner->pMixinList; *ppMixin;
            ppMixin = &(*ppMixin)->pNextMixin)
            ;
        *ppMixin = pObjectStruct;
    }
}
