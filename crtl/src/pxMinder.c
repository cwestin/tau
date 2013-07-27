
#ifndef PXMINDER_H
#include "pxMinder.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXDLL_H
#include "pxDll.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


const char pxMinderName[] = "pxMinder";

typedef struct
{
    const pxMinderVt *pMinderVt;
    pxObjectStruct objectStruct;

    pxAlloc *pAlloc; // the arena to use to allocate registered items
    pxDllHead list; // the list of items to mind
} pxMinder_s;


typedef struct
{
    pxObjectStruct objectStruct;
    pxObject *pManaged;
    pxDllLink link;
} minderObject;

static void minderObject_destroy(pxObject *pI)
{
    // destroying a minder object is the way to deregister a managed object
    minderObject *const pMO =
        PXINTERFACE_STRUCT(pI, minderObject, objectStruct.pObjectVt);
    pxDllRemove(&pMO->link);
    pMO->pManaged = NULL;

    // forward to the base class
    pxObject_destroy(pI);
}

static const pxObjectLookup minderObjectObjectTable[] =
{
    {pxObjectName, 0},
};

static const pxObjectVt minderObjectObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    sizeof(minderObjectObjectTable)/sizeof(minderObjectObjectTable[0]),
    minderObjectObjectTable,
    minderObject_destroy,
};

static pxObject *pxMinder_register(pxMinder *pI, pxObject *pManaged)
{
    pxMinder_s *const pThis = PXINTERFACE_STRUCT(pI, pxMinder_s, pMinderVt);

    // allocate a place to hang on to this
    minderObject *const pMO = (minderObject *)
        PXALLOC_alloc(pThis->pAlloc, sizeof(*pMO), PXALLOC_F_DIRTY);

    // initialize
    pxObjectStructInit(&pMO->objectStruct, &minderObjectObjectVt, NULL);
    pMO->pManaged = pManaged;
    pxDllAddAfter(&pMO->link, &pThis->list);

    return (pxObject *)&pMO->objectStruct.pObjectVt;
}

static void minder_destroy(pxObject *pI)
{
    pxMinder_s *const pThis = PXINTERFACE_STRUCT(pI, pxMinder_s, objectStruct.pObjectVt);

    // run through the list from back to front
    pxDllLink *pLink;
    while((pLink = pxDllGetLast(&pThis->list)))
    {
        minderObject *const pMO = PXDLL_STRUCT(pLink, minderObject, link);

        // remove it from the list
        pxDllRemove(&pMO->link);

        // destroy the managed object
        PXOBJECT_destroy(pMO->pManaged);
        pMO->pManaged = NULL;
    }

    // forward call to base class
    pxObject_destroy(pI);
}

static const pxMinderVt minderVt =
{
    {
        offsetof(pxMinder_s, objectStruct.pObjectVt) - offsetof(pxMinder_s, pMinderVt),
        pxObject_getInterface,
    },
    pxMinder_register,
};

static const pxObjectLookup minderObjectTable[] =
{
    {pxMinderName, offsetof(pxMinder_s, objectStruct.pObjectVt) - offsetof(pxMinder_s, pMinderVt)},
    {pxObjectName, 0},
};

static const pxObjectVt minderObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    sizeof(minderObjectTable)/sizeof(minderObjectTable[0]),
    minderObjectTable,
    minder_destroy,
};


pxMinder *pxMinderCreate(pxAlloc *pAlloc, pxInterface *pOwner)
{
    pxMinder_s *const pMinder =
        PXALLOC_alloc(pAlloc, sizeof(*pMinder), PXALLOC_F_DIRTY);

    pMinder->pMinderVt = &minderVt;
    pxObjectStructInit(&pMinder->objectStruct, &minderObjectVt, pOwner);

    pMinder->pAlloc = pAlloc;
    pxDllInit(&pMinder->list);

    return (pxMinder *)&pMinder->pMinderVt;
}
