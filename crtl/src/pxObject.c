
#ifndef PXOBJECT_H
#include "pxObject.h"
#endif

const char pxObjectName[] = "pxObject";

pxInterface *pxObjectGetInterface(
    const pxInterface *const pI, const char *const pName)
{
    const pxObjectMeta *const pObjectMeta =
        (pxObjectMeta *)(((char *)pI) - offsetof(pxObjectMeta, vt));
    const char *const pObject =
        ((char *)pI) + pObjectMeta->prefix.objectOffset;

    return NULL; // TODO
}

