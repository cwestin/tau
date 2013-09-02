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

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
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


static size_t pxObjectStruct_countMembers(pxObjectStruct *const pO)
{
    size_t n = pO->pObjectVt->nMember;
    if (!n)
        return 0;

    size_t nonNullMembers = 0;
    const char *const p = ((char *)pO) - pO->pObjectVt->objectOffset;
    for(const pxObjectMember *pMember = pO->pObjectVt->pMember; n;
        ++pMember, --n)
    {
        pxInterface *const pI = *(pxInterface **)(p + pMember->memberOffset);
        if (pI)
            ++nonNullMembers;
    }

    return nonNullMembers;
}

static pxObjectStruct *pxObjectCloner_cloneThis(
    pxObjectCloner *const pCloner, pxObjectStruct *const pO,
    size_t *const pNonNullMembers)
{
    pxObjectStruct *const pNewO =
        (*pO->pObjectVt->dupThis)(pO, pCloner->pAlloc);
    *pNonNullMembers += pxObjectStruct_countMembers(pO);

    // we need to clone mixins first so that any members which point
    // to an interface on a mixin can find that interface
    if (pO->pMixinList)
    {
        // Clone my mixins in the order in which they appear
        //
        // Since we always start with the owner, we know none of these will
        // be in the map. We assume the number of mixins is relatively
        // small so that we don't have to resort to pointer reversal to
        // avoid stack depth issues.
        pxObjectStruct **ppNewMixin = &pNewO->pMixinList;
        for(pxObjectStruct *pMixin = pO->pMixinList; pMixin;
            pMixin = pMixin->pNextMixin)
        {
            pxObjectStruct *const pNewMixin =
                pxObjectCloner_cloneThis(pCloner, pMixin, pNonNullMembers);

            // patch this, since cloneThis uses memcpy on the whole structure
            pNewMixin->pOwner = pNewO;

            // add this to the new owner's list of mixins
            *ppNewMixin = pNewMixin;
            ppNewMixin = &pNewMixin->pNextMixin;
        }
        
        // terminate the new mixin list
        *ppNewMixin = NULL;
    }

    return pNewO;
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
    /* keyOffset */ offsetof(pxObjectCloner_Item, pOldO) -
                        offsetof(pxObjectCloner_Item, hmEntry),
    /* hash */ pxHashStructStar,
    /* cmp */ pxCmpStructStar,
};


typedef struct
{
    pxObjectStruct *pNewO;
    pxObjectCloner *pCloner;
    pxObjectStruct *pOldO;
} pxObjectCloner_InsertCtx;

static pxHmEntry *pxObjectCloner_insert(void *const pctx, pxAlloc *const pAlloc)
{
    pxObjectCloner_InsertCtx *const pCtx = (pxObjectCloner_InsertCtx *)pctx;

    // set up and return the new hash map entry
    pxObjectCloner_Item *const pItem =
        PXALLOC_alloc(pAlloc, sizeof(pxObjectCloner_Item), PXALLOC_F_DIRTY);
    pItem->pOldO = pCtx->pOldO;
    pItem->pNewO = pCtx->pNewO;

    // if we're here, we already know there are members to process, so add
    // this to the list of items requiring post-processing
    pItem->pNextItem = pCtx->pCloner->pItemList;
    pCtx->pCloner->pItemList = pItem;

    return &pItem->hmEntry;
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
    // clone this object
    size_t nMembers = 0;
    pxObjectCloner_InsertCtx ctx;
    ctx.pNewO = pxObjectCloner_cloneThis(pCloner, pO, &nMembers);

    // if there were non-NULL member pointers, we're going to have to do more
    if (nMembers)
    {
        // we'll have to do this with full context
        pxObjectClonerInitGraph(pCloner, pCloner->pAlloc);

        // add the cloned item to the cloner's hashmap
        ctx.pOldO = pO;
        ctx.pCloner = pCloner;
        pxHmMapFind(pCloner->pMap, &pO, pxObjectCloner_insert, &ctx);
    }

    return ctx.pNewO;
}


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
    size_t nMembers = 0;
    pItem->pNewO = pxObjectCloner_cloneThis(
        pCtx->pCloner, pItem->pOldO, &nMembers);

    // if necessary, add it to the list of things to attend to
    if (!nMembers)
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
    {
        // find the root-most owner (keeps the stack depth down)
        do
            pO = pO->pOwner;
        while(pO->pOwner);

        // start at the top
        return pxObjectCloner_cloneGraph(pCloner, pO);
    }

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

static void pxObjectCloner_setMembers(
    pxObjectCloner *const pCloner,
    pxObjectStruct *const pNewO, pxObjectStruct *const pOldO)
{
    const pxObjectVt *const pObjectVt = pNewO->pObjectVt;
    if (pObjectVt->nMember)
    {
        // clone any other objects referenced by pointer members
        const char *const pOldB =
            ((char *)pOldO) - pObjectVt->objectOffset;
        const char *const pNewB =
            ((char *)pNewO) - pObjectVt->objectOffset;
        size_t n = pObjectVt->nMember;
        for(const pxObjectMember *pMember = pObjectVt->pMember; n;
            ++pMember, --n)
        {
            // get the old interface pointer from the original object
            pxInterface *const pIOld =
                *(pxInterface **)(pOldB + pMember->memberOffset);

            if (pIOld)
            {
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
                    (pxInterface **)(pNewB + pMember->memberOffset);
                *ppINew = pINew;
            }
        }
    }

    // set the members on all the mixins
    // we walk the two mixin lists in parallel
    pxObjectStruct *pMixinO = pOldO->pMixinList;
    for(pxObjectStruct *pMixin = pNewO->pMixinList; pMixin;
        pMixinO = pMixinO->pNextMixin, pMixin = pMixin->pNextMixin)
    {
        pxObjectCloner_setMembers(pCloner, pMixin, pMixinO);
    }
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

        // set this item's members
        pxObjectCloner_setMembers(pCloner, pItem->pNewO, pItem->pOldO);
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
        (pxInterface *)&pO->pObjectVt, pIName);
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

pxObjectStruct *pxObjectStruct_dupThis(pxObjectStruct *pO, pxAlloc *pA)
{
    // allocate and copy the object
    void *const pNew = PXALLOC_alloc(pA, pO->pObjectVt->size, PXALLOC_F_DIRTY);
    memcpy(pNew, ((char *)pO) - pO->pObjectVt->objectOffset,
           pO->pObjectVt->size);
    return (pxObjectStruct *)(((char *)pNew) + pO->pObjectVt->objectOffset);
}

static void pxObjectStruct_indent(FILE *fp, unsigned depth)
{
    for(int i = depth; i; --i)
        fprintf(fp, "  ");
}

static void pxObjectStruct_dumpThis(
    const pxObjectStruct *const pO, FILE *fp, unsigned depth)
{
    pxObjectStruct_indent(fp, depth);
    fprintf(stderr, "pxObjectStruct %p\n", pO);
    pxObjectStruct_indent(fp, depth);
    fprintf(stderr, "implementation structure %p\n",
            ((char *)pO) - pO->pObjectVt->objectOffset);

    const char *const p = ((char *)pO) + offsetof(pxObjectStruct, pObjectVt);

    size_t n = pO->pObjectVt->nInterface;
    for(const pxObjectInterface *pOI = pO->pObjectVt->pInterface; n; ++pOI, --n)
    {
        pxObjectStruct_indent(fp, depth);
        fprintf(fp, "interface \"%s\" %p\n", pOI->pName,
            p - pOI->interfaceOffset);
    }

    if (pO->pMixinList)
    {
        fprintf(stderr, "\n");
        pxObjectStruct_indent(fp, depth);
        fprintf(stderr, "mixins:");

        for(const pxObjectStruct *pMixin = pO->pMixinList; pMixin;
            pMixin = pMixin->pNextMixin)
        {
            fprintf(stderr, "\n");
            pxObjectStruct_dumpThis(pMixin, fp, depth + 1);
        }
    }
}

void pxObject_dump(pxInterface *pI, const char *pLabel)
{
    pxObject *const pObject = PXINTERFACE_getInterface(pI, pxObject);
    pxObjectStruct *const pO =
        PXINTERFACE_STRUCT(pObject, pxObjectStruct, pObjectVt);

    if (pLabel)
        fprintf(stderr, "%s =================================\n", pLabel);

    pxObjectStruct_dumpThis(pO, stderr, 0);

    if (pLabel)
        fprintf(stderr, "====================================\n");
}
