
#ifndef PXHASHMAPLOCAL_H
#include "pxHashMapLocal.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXHASHABLE_H
#include "pxHashable.h"
#endif

#ifndef PXHASHER_H
#include "pxHasher.h"
#endif

#ifndef PXHASHMAP_H
#include "pxHashMap.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif


typedef struct pxHashMapLocal_entry
{
    struct pxHashMapLocal_entry *pNext;
    pxHashable *pKey;
    pxInterface *pO;
} pxHashMapLocal_entry;

typedef struct
{
    pxHashMapLocal_entry *pEntryList;
} pxHashMapLocal_bucket;

typedef struct
{
    pxHashMapLocal_bucket *pBucket;
    size_t nBuckets;
    unsigned nEntries;

    const pxHashMapVt *pHashMapVt;
    pxObjectStruct objectStruct;
} pxHashMapLocal_s;


static bool pxHashMapLocal_isEmpty(pxHashMap *pI)
{
    pxHashMapLocal_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHashMapLocal_s, pHashMapVt);

    return pThis->nEntries == 0;
}

static void pxHashMapLocal_find(pxHashMapLocal_s *const pThis,
                                pxHashable *pKey)
{
}

static pxInterface *pxHashMapLocal_get(pxHashMap *pI, pxHashable *pKey)
{
    pxHashMapLocal_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHashMapLocal_s, pHashMapVt);

    pxHashMapLocal_find(pThis, pKey);
    // TODO
    return NULL;
}

static pxInterface *pxHashMapLocal_put(
    pxHashMap *pI, pxHashable *pKey, pxInterface *pO)
{
    pxHashMapLocal_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHashMapLocal_s, pHashMapVt);

    pxHashMapLocal_find(pThis, pKey);
    // TODO
    return NULL;
}

static const pxHashMapVt pxHashMapLocalHashMapVt =
{
    {
        offsetof(pxHashMapLocal_s, objectStruct.pObjectVt) -
        offsetof(pxHashMapLocal_s, pHashMapVt),
        pxObject_getInterface,
    },
    pxHashMapLocal_isEmpty,
    pxHashMapLocal_get,
    pxHashMapLocal_put,
};

static const pxObjectLookup pxHashMapLocal_lookup[] =
{
    {pxHashMapName,
     offsetof(pxHashMapLocal_s, objectStruct.pObjectVt) -
     offsetof(pxHashMapLocal_s, pHashMapVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxHashMapLocalObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    sizeof(pxHashMapLocal_lookup)/sizeof(pxHashMapLocal_lookup[0]),
    pxHashMapLocal_lookup,
    pxObject_destroy,
};

pxHashMap *pxHashMapLocalCreate(
    pxAlloc *pAlloc, pxInterface *pOwner, size_t initCap, size_t avgBucket)
{
    pxHashMapLocal_s *const pO =
        PXALLOC_alloc(pAlloc, sizeof(pxHashMapLocal_s), PXALLOC_F_DIRTY);
    pO->pHashMapVt = &pxHashMapLocalHashMapVt;
    pxObjectStructInit(&pO->objectStruct, &pxHashMapLocalObjectVt, pOwner);

    // TODO

    return (pxHashMap *)&pO->pHashMapVt;
}
