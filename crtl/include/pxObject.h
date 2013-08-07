
#ifndef PXOBJECT_H
#define PXOBJECT_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


typedef struct
{
    const char *const pName;
    ptrdiff_t interfaceOffset; // offset from pxObject interface to other interface
} pxObjectLookup;

struct pxObject;
typedef struct pxObjectVt
{
    pxInterfaceVt interfaceVt;

    size_t nLookup;
    const pxObjectLookup *pLookup;

    void (*destroy)(struct pxObject *pObject);
} pxObjectVt;

typedef struct pxObject
{
    const pxObjectVt *const pVt;
} pxObject;

extern const char pxObjectName[];

#define PXOBJECT_destroy(pI) \
    ((*(pI)->pVt->destroy)((pI)))


pxInterface *pxObject_getInterface(
    pxInterface *pI, const char *const pName);

void pxObject_destroy(pxObject *const pI);

typedef struct pxObjectStruct
{
    const pxObjectVt *pObjectVt;
    struct pxObjectStruct *pNextMixin;
    struct pxObjectStruct *pOwner;
} pxObjectStruct;

void pxObjectStructInit(
    pxObjectStruct *pObjectStruct, const pxObjectVt *pVt,
    pxInterface *pOwner);

#endif // PXOBJECT_H
