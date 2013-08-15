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


static inline pxObjectStruct *pxObjectCloner_cloneThis(
    pxObjectCloner *pCloner, pxObjectStruct *pO)
{
    // allocate and copy the object
    void *const pNew = PXALLOC_alloc(
        pCloner->pAlloc, pO->pObjectVt->size, PXALLOC_F_DIRTY);
    memcpy(pNew, ((char *)pO) - pO->pObjectVt->objectOffset,
           pO->pObjectVt->size);

    return (pxObjectStruct *)(((char *)pNew) + pO->pObjectVt->objectOffset);
}

/**
   Clone the first object.

   This starts out assuming that we haven't created the cloned object map
   because this is a simple stand-alone object. once cloned, if we discover
   that not to be the case, we set things up to process more objects.

   This does handle mixins, but not members that point to other objects; those
   must be handled in a second pass in order to avoid the call stack getting
   as deep as the longest chain of object references in the graph.
 */
static pxObjectStruct *pxObjectCloner_cloneSingle(
    pxObjectCloner *pCloner, pxObjectStruct *pO)
{
    // check for any mixins or members that point to other objects
    if (pO->pMixinList || pO->pObjectVt->nMember)
    {
        // we'll have to do this with full context
        pxObjectClonerInitGraph(pCloner, pCloner->pAlloc);

        // try again with the full setup
        return (*pCloner->clone)(pCloner, pO);
    }

    // allocate and copy the object
    return pxObjectCloner_cloneThis(pCloner, pO);
}

typedef struct pxObjectCloner_Item
{
    pxObjectStruct *pNewO; // cloned object
    pxObjectStruct *pOldO; // original object

    struct pxObjectCloner_Item *pNextItem; // link on list to process

    pxHmEntry hmEntry;
} pxObjectCloner_Item;

static const pxHmDope pxObjectCloner_hmDope =
{
    /* avgBucket */ 3,
    /* keyOffset */ offsetof(pxObjectCloner_Item, pOldO),
    /* hash */ pxHashStructStar,
    /* cmp */ pxCmpStructStar,
};

typedef struct
{
    pxObjectCloner *pCloner; // the cloner
    pxObjectStruct *pTarget; // the object to be cloned
} pxObjectCloner_CreateCtx;

static pxHmEntry *pxObjectCloner_create(void *const pctx, pxAlloc *const pAlloc)
{
    pxObjectCloner_CreateCtx *const pCtx = (pxObjectCloner_CreateCtx *)pctx;

    // set up and return the new hash map entry
    pxObjectCloner_Item *const pItem =
        PXALLOC_alloc(pAlloc, sizeof(pxObjectCloner_Item), PXALLOC_F_DIRTY);
    pItem->pOldO = pCtx->pTarget;

    // clone the object
    pItem->pNewO = pxObjectCloner_cloneThis(pCtx->pCloner, pItem->pOldO);

    // if necessary, add it to the list of things to attend to
    if (!pItem->pOldO->pMixinList && !pItem->pOldO->pObjectVt->nMember)
        pItem->pNextItem = NULL;
    else
    {
        pItem->pNextItem = pCtx->pCloner->pItemList;
        pCtx->pCloner->pItemList = pItem;
    }

    return &pItem->hmEntry;
}

static pxObjectStruct *pxObjectCloner_cloneGraph(
    pxObjectCloner *pCloner, pxObjectStruct *pO)
{
    // if this isn't the root object, start with that instead
    if (pO->pOwner)
        return pxObjectCloner_cloneGraph(pCloner, pO->pOwner);

    pxObjectCloner_CreateCtx ctx; // local variables for closure
    ctx.pCloner = pCloner;
    ctx.pTarget = pO;

    // find the cloned object in the context hash map, creating it if necessary
    pxHmEntry *const pEntry =
        pxHmMapFind(pCloner->pMap, &pO, pxObjectCloner_create, &ctx);
    pxObjectCloner_Item *const pItem =
        PXHM_STRUCT(pEntry, pxObjectCloner_Item, hmEntry);

    return pItem->pNewO;
}

void pxObjectClonerInitSingle(
    pxObjectCloner *const pCloner, pxAlloc *const pAlloc)
{
    pCloner->clone = pxObjectCloner_cloneSingle;
    pCloner->pAlloc = pAlloc;
}

void pxObjectClonerInitGraph(
    pxObjectCloner *const pCloner, pxAlloc *const pAlloc)
{
    pCloner->clone = pxObjectCloner_cloneGraph;
    pCloner->pAlloc = pAlloc;

    // create the map
    pxAlloc *const pAllocS = pxAllocSystemGet();
    pxAlloc *const pAllocE = pxAllocExtentCreate(pAllocS, 1024, NULL);
    pCloner->pMap = (pxHmMap *)
        PXALLOC_alloc(pAllocE, sizeof(pxHmMap), PXALLOC_F_DIRTY);
    pxHmMapInit(pCloner->pMap, &pxObjectCloner_hmDope, pAllocE, 8);

    // initialize the list
    pCloner->pItemList = NULL;
}

static void pxObjectClonerProcess(pxObjectCloner *const pCloner)
{
    // go hrough all the items that need the 2nd phase of processing
    pxObjectCloner_Item *pItem;
    while((pItem = pCloner->pItemList))
    {
        // remove this item from the list of things to be processed
        pCloner->pItemList = pItem->pNextItem;
        pItem->pNextItem = NULL;

        // we need to clone mixins first so that any members which point
        // to an interface on a mixin need to be able to find that
        if (pItem->pOldO->pMixinList)
        {
            // clone my mixins in the order in which they appear
            // Since we always start with the owner, we know none of these will
            // be in the map. We assume the number of mixins is relatively
            // small so that we don't have to resort to pointer reversal to
            // avoid stack issues.
            pxObjectStruct **ppNewMixin = &pItem->pNewO->pMixinList;
            for(pxObjectStruct *pMixin = pItem->pOldO->pMixinList; pMixin;
                pMixin = pMixin->pNextMixin)
            {
                pxObjectStruct *const pNewMixin =
                    pxObjectCloner_cloneGraph(pCloner, pMixin);

                // add this to the new owner's list of mixins
                *ppNewMixin = pNewMixin;
                ppNewMixin = &pNewMixin->pNextMixin;
            }
        
            // terminate the new mixin list
            *ppNewMixin = NULL;
        }

        const pxObjectVt *const pObjectVt = pItem->pNewO->pObjectVt;
        if (pObjectVt->nMember)
        {
            // clone any other objects referenced by pointer members
            const char *const pOldO =
                ((char *)pItem->pOldO) + pObjectVt->objectOffset;
            const char *const pNewO =
                ((char *)pItem->pNewO) + pObjectVt->objectOffset;
            size_t n = pObjectVt->nMember;
            for(const pxObjectMember *pMember = pObjectVt->pMember; n;
                ++pMember, --n)
            {
                // get the old interface pointer from the original object
                pxInterface *const pIOld =
                    *(pxInterface **)(pOldO + pMember->memberOffset);

                // get the pxObject interface on the old interface
                pxObject *const pOOld =
                    (pxObject *)(((char *)pIOld) +
                                 pIOld->pVt->interfaceVt.pxObjectOffset);

                pxObjectStruct *const pSOld =
                    PXINTERFACE_STRUCT(pOOld, pxObjectStruct, pObjectVt);

                // clone (or find) the object
                pxObjectStruct *const pSNew =
                    pxObjectCloner_cloneGraph(pCloner, pSOld);

                // obtain the proper interface
                pxInterface *const pINew =
                    (*pSNew->pObjectVt->interfaceVt.getInterface)(
                        (pxInterface *)&pSNew->pObjectVt, pMember->pName);

                // put the interface pointer in the proper place
                pxInterface **const ppINew =
                    (pxInterface **)(pNewO + pMember->memberOffset);
                *ppINew = pINew;
            }
        }
    }
}

void pxObjectClonerCleanup(pxObjectCloner *const pCloner)
{
    if (pCloner->clone == pxObjectCloner_cloneGraph)
    {
        // do any final post-processing necessary
        pxObjectClonerProcess(pCloner);
    
        pxAlloc *const pMapAlloc = pxHmMapGetAlloc(pCloner->pMap);
        pxObject *const pAllocO = PXINTERFACE_getInterface(pMapAlloc, pxObject);
        PXOBJECT_destroy(pAllocO);
    }
}

pxInterface *pxObject_clone(
    pxObject *const pI, const char *const pIName, pxObjectCloner *pCloner)
{
    pxObjectStruct *pThis = PXINTERFACE_STRUCT(pI, pxObjectStruct, pObjectVt);
    pxObjectStruct *const pO = (*pCloner->clone)(pCloner, pThis);
    return pO->pObjectVt->interfaceVt.getInterface(
        (pxInterface *)&pO->pObjectVt->interfaceVt, pIName);
}

pxInterface *pxObject_cloneForbidden(
    pxObject *pI, const char *const pIName, pxObjectCloner *pCloner)
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
