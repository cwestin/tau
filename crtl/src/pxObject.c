
#ifndef PXOBJECT_H
#include "pxObject.h"
#endif

const char pxObjectName[] = "pxObject";

pxInterface *const pxObject_getInterface(
    pxInterface *const pI, const char *const pName)
{
    const pxObject *const ppxObject =
        (const pxObject *const)(((char *)pI) + (*pI)->pxObjectOffset);
    const pxObjectLookup *pLookup = (*ppxObject)->pLookup;
    for(int i = (*ppxObject)->nLookup; i; ++pLookup, --i)
    {
        if (pLookup->pName == pName)
        {
            return (pxInterface *)(((char *)ppxObject) - pLookup->interfaceOffset);
        }
    }

    // if we got here, there was no such interface
    return NULL;
}

void pxObject_destroy(pxObject *const pI)
{
    pxObjectStruct *const pO = PXINTERFACE_STRUCT(pI, pxObjectStruct, pObjectVt);

    // mixins have to be destroyed in the reverse order, so reverse the list
    pxObjectStruct *pMixinList = NULL;
    pxObjectStruct *pNextMixin;
    while((pNextMixin = pO->pNextMixin))
    {
        // move it from this object's list to the local list
        pO->pNextMixin = pNextMixin->pNextMixin;
        pNextMixin->pNextMixin = pMixinList;
        pMixinList = pNextMixin;
    }

    // destroy the mixins
    while((pNextMixin = pMixinList))
    {
        PXOBJECT_destroy(&pNextMixin->pObjectVt);

        // remove the destroyed one from the list
        pMixinList = pNextMixin->pNextMixin;
    }
}

void pxObject_addMixin(pxObject *const pI, pxInterface *pOther)
{
    pxObjectStruct *const pO = PXINTERFACE_STRUCT(pI, pxObjectStruct, pObjectVt);

    // add to end of list
    // modify controlling interface
    // TODO
    exit(-1);
}
