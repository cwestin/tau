
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
    size_t nEntries;
    size_t avgBucket;

    const pxHashMapVt *pHashMapVt;
    pxObjectStruct objectStruct;
} pxHashMapLocal_s;


static bool pxHashMapLocal_isEmpty(pxHashMap *pI)
{
    pxHashMapLocal_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHashMapLocal_s, pHashMapVt);

    return pThis->nEntries == 0;
}

static void pxHashMapLocal_find(
    pxHashMapLocal_s *const pThis, pxHashable *const pKey)
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
    pxHashMapLocal_s *const pThis =
        PXALLOC_alloc(pAlloc, sizeof(pxHashMapLocal_s), PXALLOC_F_DIRTY);
    pThis->pHashMapVt = &pxHashMapLocalHashMapVt;
    pxObjectStructInit(&pThis->objectStruct, &pxHashMapLocalObjectVt, pOwner);

    // don't aim for less than one entry per bucket
    if (avgBucket < 1)
        avgBucket = 1;

    // don't aim for fewer than 5 elements to start
    if (initCap < 5)
        initCap = 5;

    // figure out the initial number of buckets
    // use the first power of two greater than or equal to that requested
    size_t initBucket0 = (initCap + avgBucket - 1) / avgBucket;
    size_t initBucket = 1;
    for(; initBucket < initBucket0; initBucket <<= 1)
        ;

    pThis->pBucket =
        PXALLOC_alloc(pAlloc, sizeof(pxHashMapLocal_bucket) * initBucket, 0);
    pThis->nBuckets = initBucket;
    pThis->nEntries = 0;
    pThis->avgBucket = avgBucket;

    return (pxHashMap *)&pThis->pHashMapVt;
}
