
#ifndef PXHASHER_H
#include "pxHasher.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXHASH_H
#include "pxHash.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


const char pxHasherName[] = "pxHasher";

typedef struct
{
    pxHashValue hashValue;
    pxObjectStruct objectStruct;
} pxHasher_s;


static const pxObjectLookup pxHasher_lookup[] =
{
    {pxHasherName, 0},
    {pxObjectName, 0},
};

static const pxObjectVt pxHasherObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    sizeof(pxHasher_lookup)/sizeof(pxHasher_lookup[0]),
    pxHasher_lookup,
    pxObject_destroy,
};

pxHasher *pxHasherCreate(pxAlloc *pAlloc, pxInterface *pOwner)
{
    pxHasher_s *pThis =
        PXALLOC_alloc(pAlloc, sizeof(pxHasher_s), PXALLOC_F_DIRTY);
    pxObjectStructInit(&pThis->objectStruct, &pxHasherObjectVt, pOwner);
    pxHashInit(&pThis->hashValue);

    return (pxHasher *)&pThis->objectStruct.pObjectVt;
}

int PXHASHER_getHash(pxHasher *pI)
{
    pxHasher_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHasher_s, objectStruct.pObjectVt);

    return pThis->hashValue;
}

void PXHASHER_hashBytes(pxHasher *pI, const void *p, size_t length)
{
    pxHasher_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHasher_s, objectStruct.pObjectVt);

    pxHashVoid(&pThis->hashValue, p, length);
}
