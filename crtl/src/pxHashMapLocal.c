
#ifndef PXHASHMAPLOCAL_H
#include "pxHashMapLocal.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXALLOCLOCAL_H
#include "pxAllocLocal.h"
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
    int rawHash;
    pxComparable *pKeyCmp;
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
    pxAlloc *pAlloc;

    const pxHashMapVt *pHashMapVt;
    pxObjectStruct objectStruct;
} pxHashMapLocal_s;


static bool pxHashMapLocal_isEmpty(pxHashMap *pI)
{
    pxHashMapLocal_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHashMapLocal_s, pHashMapVt);

    return pThis->nEntries == 0;
}

static int pxHashMapLocal_hash(pxHashable *const pKey)
{
    pxAlloc *pAllocLocal;
    pxHasher *pHasher;
    PXALLOCLOCAL_SPACE(96) stackSpace;

    pAllocLocal = PXALLOCLOCAL_INIT(&stackSpace);
    pHasher = pxHasherCreate(pAllocLocal, NULL);

    PXHASHABLE_hash(pKey, pHasher);
    int rawHash = PXHASHER_getHash(pHasher);

    // clean up
    pxObject *const pAllocObject =
        PXINTERFACE_getInterface(pAllocLocal, pxObject);
    PXOBJECT_destroy(pAllocObject);

    return rawHash;
}

static pxHashMapLocal_entry **pxHashMapLocal_find(
    pxHashMapLocal_s *const pThis, pxHashable *const pKey, bool create)
{
    const int rawHash = pxHashMapLocal_hash(pKey);
    const int iBucket = rawHash & (pThis->nBuckets - 1); // mask power of 2
    pxHashMapLocal_bucket *const pBucket = pThis->pBucket + iBucket;

    pxHashMapLocal_entry **ppEntry;
    for(ppEntry = &pBucket->pEntryList; *ppEntry; ppEntry = &(*ppEntry)->pNext)
    {
        const int thisHash = (*ppEntry)->rawHash;
        if ((rawHash == thisHash))
        {
            const int keyCmp =
                PXCOMPARABLE_compare((*ppEntry)->pKeyCmp, (pxInterface *)pKey);
            if (keyCmp == 0)
                return ppEntry;
            else if (keyCmp > 0)
                break; // we've passed where it would go, so stop looking
        }
        else if (rawHash > thisHash)
            break; // we've passed where it would go, so stop looking
    }

    // we didn't find it, but this is where it would go
    if (!create)
        return NULL;

    pxHashMapLocal_entry *const pEntry =
        PXALLOC_alloc(pThis->pAlloc, sizeof(pxHashMapLocal_entry),
                      PXALLOC_F_DIRTY);
    pEntry->rawHash = rawHash;
    // TODO clone key
    pEntry->pKeyCmp = PXINTERFACE_getInterface(pKey, pxComparable);
    pEntry->pO = NULL;

    pEntry->pNext = *ppEntry;
    *ppEntry = pEntry;
    ++pThis->nEntries;
    return ppEntry;
}

static pxInterface *pxHashMapLocal_get(pxHashMap *pI, pxHashable *pKey)
{
    pxHashMapLocal_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHashMapLocal_s, pHashMapVt);

    pxHashMapLocal_entry **ppEntry =
        pxHashMapLocal_find(pThis, pKey, false);
    if (ppEntry)
        return (*ppEntry)->pO;

    return NULL;
}

static pxInterface *pxHashMapLocal_put(
    pxHashMap *pI, pxHashable *pKey, pxInterface *pO)
{
    pxHashMapLocal_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHashMapLocal_s, pHashMapVt);

    pxHashMapLocal_entry **ppEntry =
        pxHashMapLocal_find(pThis, pKey, true);
    pxInterface *const pOld = (*ppEntry)->pO;
    (*ppEntry)->pO = pO;
    return pOld;
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
    pThis->pAlloc = pAlloc;

    return (pxHashMap *)&pThis->pHashMapVt;
}
