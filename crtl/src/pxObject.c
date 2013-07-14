
#ifndef PXOBJECT_H
#include "pxObject.h"
#endif

const char pxObjectName[] = "pxObject";

pxInterface *pxObjectGetInterface(
    const pxInterface *const pI, const char *const pName)
{
    const pxObjectMeta *const pInterfaceMeta =
        (pxObjectMeta *)((char *)pI->pVt - offsetof(pxObjectMeta, vt));
    const pxObject *const pObject =
        (pxObject *)((char *)pI + pInterfaceMeta->prefix.objectOffset);
    const pxObjectObjectMeta *const pMeta =
        (pxObjectObjectMeta *)((char *)pObject->pVt - offsetof(pxObjectObjectMeta, objectMeta.vt));

    const pxObjectLookup *pLookup = pMeta->pLookup;
    for(int i = pMeta->nLookup; i; ++pLookup, --i)
    {
        if (pLookup->pName == pName)
        {
            return (pxInterface *)(((char *)pObject - pLookup->offset));
        }
    }

    // if we got here, there was no such interface
    return NULL;
}

