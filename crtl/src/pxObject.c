
#ifndef PXOBJECT_H
#include "pxObject.h"
#endif

#ifndef PX_STDLIB_H
#include <stdlib.h> // only for exit()
#define PX_STDLIB_H
#endif


const char pxObjectName[] = "pxObject";

pxInterface *pxObject_getInterface(pxInterface *pI, const char *const pName)
{
    pxObject *const ppxObject =
        (pxObject *)(((char *)pI) + pI->pVt->pxObjectOffset);
    const pxObjectLookup *pLookup = ppxObject->pVt->pLookup;
    for(int i = ppxObject->pVt->nLookup; i; ++pLookup, --i)
    {
        if (pLookup->pName == pName)
        {
            return (pxInterface *)
                (((char *)ppxObject) - pLookup->interfaceOffset);
        }
    }

    // if we got here, there was no such interface
    return NULL;
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

void pxObject_addMixin(pxObject *pI, pxInterface *pOther)
{
/* TODO(cwestin)
    pxObjectStruct *const pThis =
        PXINTERFACE_STRUCT(pI, pxObjectStruct, pObjectVt);
*/

    // add to end of list
    // modify controlling interface
    // TODO
    exit(-1);
}
