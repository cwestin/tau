
#ifndef PXOBJECT_H
#define PXOBJECT_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


typedef struct
{
    const char *const pName;
    int interfaceOffset; // offset from Object interface to other interface
} pxObjectLookup;

typedef struct pxObjectVt
{
    pxInterfaceVt interfaceVt;

    size_t nLookup;
    const pxObjectLookup *pLookup;

    // TODO
    void (*addMixin)(struct pxObjectVt *const *const pObject,
                     const pxInterface *const pOther);
} pxObjectVt, *const pxObject;

extern const char pxObjectName[];


pxInterface *const pxObject_getInterface(
    pxInterface *const pI, const char *const pName);

#endif // PXOBJECT_H
