
#ifndef PXOBJECT_H
#define PXOBJECT_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

struct pxObject;

typedef struct
{
    PXINTERFACE_GET(pxObject);
    void (*addMixin)(const struct pxObject *const pThis,
                     const pxInterface *const pOther);
} pxObjectVt;

typedef struct pxObject
{
    const pxObjectVt *pVt;
} pxObject;

extern const char pxObjectName[];


typedef struct
{
    size_t objectOffset;
} pxObjectInterfacePrefix;

typedef struct
{
    pxObjectInterfacePrefix prefix;
    pxObjectVt vt;
} pxObjectMeta;

pxInterface *pxObjectGetInterface(
    const pxInterface *const pI, const char *const pName);

#define PXOBJECT_GETINTERFACE(iName) \
    ((pxInterface *(*)(const iName *const, const char *const))pxObjectGetInterface)

typedef struct
{
    const char *const pName;
    size_t offset;
} pxObjectLookup;

typedef struct
{
    const pxObjectLookup *pLookup;
    pxObjectMeta objectMeta;
} pxObjectObjectMeta;

#endif // PXOBJECT_H
