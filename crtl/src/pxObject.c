
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
