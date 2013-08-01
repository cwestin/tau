
#ifndef PXHASHABLEVAR_H
#include "pxHashableVar.h"
#endif

#ifndef PXCOMPARABLE_H
#include "pxComparable.h"
#endif

#ifndef PXHASHABLE_H
#include "pxHashable.h"
#endif

#ifndef PXHASHER_H
#include "pxHasher.h"
#endif

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

#ifndef PX_STRING_H
#include <string.h>
#define PX_STRING_H
#endif


static void pxHashableVar_hash(pxHashable *pI, pxHasher *pHasher)
{
    const pxHashableVar_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHashableVar_s, pHashableVt);

    PXHASHER_hashBytes(pHasher, pThis->p, pThis->size);
}

static const pxHashableVt pxHashableVarHashableVt =
{
    {offsetof(pxHashableVar_s, objectStruct.pObjectVt) -
     offsetof(pxHashableVar_s, pHashableVt),
     pxObject_getInterface,
    },
    pxHashableVar_hash,
};

static int pxHashableVar_compare(pxComparable *pI, pxInterface *pOther)
{
    const pxHashableVar_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHashableVar_s, pComparableVt);

    // the two objects have to be of the same class
    pxObject *const pObject =
        PXINTERFACE_getInterface((pxObject *)pOther, pxObject);
    if (pObject->pVt != pThis->objectStruct.pObjectVt)
    {
        fprintf(stderr, "pxHashableVar_compare: objects of different class\n");
        exit(1);
    }

    // same class: they must have the same length or be variable
    const pxHashableVar_s *const pOtherThis =
        PXINTERFACE_STRUCT(pObject, pxHashableVar_s, objectStruct.pObjectVt);
    // TODO allow variable size
    if ((pThis->size != pOtherThis->size))
    {
        fprintf(stderr, "pxHashableVar_compare: objects of different class\n");
        exit(1);
    }
    
    size_t size;
    int sizecmp;
    if (pThis->size < pOtherThis->size)
    {
        size = pThis->size;
        sizecmp = -1;
    }
    else
    {
        size = pOtherThis->size;
        sizecmp = (pThis->size == pOtherThis->size) ? 0 : 1;
    }

    const int cmp = memcmp(pThis->p, pOtherThis->p, size);
    if (cmp == 0)
        return sizecmp;
    return cmp;
}

static const pxComparableVt pxHashableVarComparableVt =
{
    {offsetof(pxHashableVar_s, objectStruct.pObjectVt) -
     offsetof(pxHashableVar_s, pComparableVt),
     pxObject_getInterface,
    },
    pxHashableVar_compare,
};


static const pxObjectLookup pxHashableVar_lookup[] =
{
    {pxComparableName,
     offsetof(pxHashableVar_s, objectStruct.pObjectVt) -
     offsetof(pxHashableVar_s, pComparableVt)},
    {pxHashableName,
     offsetof(pxHashableVar_s, objectStruct.pObjectVt) -
     offsetof(pxHashableVar_s, pHashableVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxHashableVarObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    sizeof(pxHashableVar_lookup)/sizeof(pxHashableVar_lookup[0]),
    pxHashableVar_lookup,
    pxObject_destroy,
};

pxHashable *pxHashableVarInit(
    pxHashableVar_s *const pThis, const void *const p, const size_t size,
    pxInterface *pOwner)
{
    pThis->pComparableVt = &pxHashableVarComparableVt;
    pThis->pHashableVt = &pxHashableVarHashableVt;
    pxObjectStructInit(&pThis->objectStruct, &pxHashableVarObjectVt, pOwner);

    pThis->p = p;
    pThis->size = size;

    return (pxHashable *)&pThis->pHashableVt;
}
