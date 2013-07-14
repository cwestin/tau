
#ifndef PXOBJECT_H
#define PXOBJECT_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

struct pxObject;

typedef struct
{
    pxInterfaceVt pIVt;
    void (*addMixin)(const struct pxObject *const pThis,
                     const pxInterface *const pOther);
    
} pxObjectVt;

typedef struct pxObject
{
    const pxObjectVt *pVt;
} pxObject;

extern const char pxObjectName[];

#endif // PXOBJECT_H
