
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

    void (*destroy)(struct pxObjectVt *const *const pObject);
    void (*addMixin)(struct pxObjectVt *const *const pObject,
                     pxInterface *const pOther);
} pxObjectVt, *const pxObject;

extern const char pxObjectName[];

#define PXOBJECT_destroy(pI) \
    ((*(*(pI))->destroy)(pI))

#define PXOBJECT_addMixin(pI, pOther) \
    ((*(*(pI))->addMixin)(pI, pOther)

pxInterface *const pxObject_getInterface(
    pxInterface *const pI, const char *const pName);

void pxObject_destroy(pxObject *const pI);

void pxObject_addMixin(pxObject *const pI, pxInterface *const pOther);

typedef struct pxObjectStruct
{
    const pxObjectVt *pObjectVt;
    const struct pxObjectStruct *pNextMixin;
} pxObjectStruct;

static inline void pxObjectStructInit(pxObjectStruct *pOS, const pxObjectVt *pVt)
{
    pOS->pObjectVt = pVt;
    pOS->pNextMixin = NULL;
}

#endif // PXOBJECT_H
