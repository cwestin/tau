
#ifndef PXOBJECT_H
#include "pxObject.h"
#endif

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif


const char pxObjectName[] = "pxObject";

static pxInterface *pxObjectStruct_getInterface(
    const pxObjectStruct *const pObjectStruct, const char *const pName)
{
    // check through interfaces this object implements directly
    const pxObjectLookup *pLookup = pObjectStruct->pObjectVt->pLookup;
    for(int i = pObjectStruct->pObjectVt->nLookup; i; ++pLookup, --i)
    {
        if (pLookup->pName == pName)
        {
            return (pxInterface *)
                (((char *)&pObjectStruct->pObjectVt) - pLookup->interfaceOffset);
        }
    }

    // check on our mixins
    for(const pxObjectStruct *pMixin = pObjectStruct->pNextMixin; pMixin;
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
        (pxObject *)(((char *)pI) + pI->pVt->pxObjectOffset);
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

void pxObject_destroy(pxObject *pI)
{
    pxObjectStruct *const pThis =
        PXINTERFACE_STRUCT(pI, pxObjectStruct, pObjectVt);

    // mixins have to be destroyed in the reverse order, so reverse the list
    pxObjectStruct *pMixinList = NULL;
    pxObjectStruct *pNextMixin;
    while((pNextMixin = pThis->pNextMixin))
    {
        // move it from this object's list to the local list
        pThis->pNextMixin = pNextMixin->pNextMixin;
        pNextMixin->pNextMixin = pMixinList;
        pMixinList = pNextMixin;
    }

    // destroy the mixins
    while((pNextMixin = pMixinList))
    {
        PXOBJECT_destroy((pxObject *)&pNextMixin->pObjectVt);

        // remove the destroyed one from the list
        pMixinList = pNextMixin->pNextMixin;
    }
}

void pxObjectStructInit(
    pxObjectStruct *pObjectStruct, const pxObjectVt *pVt,
    pxInterface *pIOwner)
{
    pObjectStruct->pObjectVt = pVt;
    pObjectStruct->pNextMixin = NULL;
    pObjectStruct->pOwner = NULL;

    // if there's an owner add this to it
    if (pIOwner)
    {
        pxObject *const pOOwner = PXINTERFACE_getInterface(
            (pxObject *)pIOwner, pxObject); // NB: wrong cast, but irrelevant
        pxObjectStruct *const pOwner =
            PXINTERFACE_STRUCT(pOOwner, pxObjectStruct, pObjectVt);

        pObjectStruct->pOwner = pOwner;

        // this new object has to go on the end of the owner's mixin list
        pxObjectStruct **ppMixin;
        for(ppMixin = &pOwner->pNextMixin; *ppMixin;
            ppMixin = &(*ppMixin)->pNextMixin)
            ;
        *ppMixin = pObjectStruct;
    }
}
