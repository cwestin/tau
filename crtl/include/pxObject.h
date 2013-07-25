
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

struct pxObject;
typedef struct pxObjectVt
{
    pxInterfaceVt interfaceVt;

    size_t nLookup;
    const pxObjectLookup *pLookup;

    void (*destroy)(struct pxObject *pObject);
    void (*addMixin)(struct pxObject *pObject,
                     pxInterface *const pOther);
} pxObjectVt;

typedef struct pxObject
{
    const pxObjectVt *const pVt;
} pxObject;

extern const char pxObjectName[];

#define PXOBJECT_destroy(pI) \
    ((*(pI)->pVt->destroy)(pI))

#define PXOBJECT_addMixin(pI, pOther) \
    ((*(pI)->pVt->addMixin)(pI, pOther))


pxInterface *pxObject_getInterface(
    pxInterface *pI, const char *const pName);

void pxObject_destroy(pxObject *const pI);

void pxObject_addMixin(pxObject *const pI, pxInterface *const pOther);

typedef struct pxObjectStruct
{
    const pxObjectVt *pObjectVt;
    struct pxObjectStruct *pNextMixin;
} pxObjectStruct;

static inline void pxObjectStructInit(
    pxObjectStruct *pOS, const pxObjectVt *pVt)
{
    pOS->pObjectVt = pVt;
    pOS->pNextMixin = NULL;
}

#endif // PXOBJECT_H
