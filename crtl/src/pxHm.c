
#ifndef PXHM_H
#include "pxHm.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXALLOCREUSE_H
#include "pxAllocReuse.h"
#endif

#ifndef PXEXIT_H
#include "pxExit.h"
#endif

#ifndef PXFREE_H
#include "pxFree.h"
#endif


typedef struct pxHmBucket
{
    pxHmEntry *pEntryList;
} pxHmBucket;

static void pxHmMapResize(pxHmMap *pMap)
{
    pxHmBucket *const pOldBucket = pMap->pBucket;
    pxHmBucket *const pNewBucket =
        PXALLOC_alloc(pMap->pAlloc,
                      (sizeof(pxHmBucket) * pMap->nBuckets) << 1, 0);

    pxHmBucket *pOld = pOldBucket;
    pxHmBucket *pNew = pNewBucket + pMap->nBuckets;
    for(int n = pMap->nBuckets; n; ++pOld, ++pNew, --n)
    {
        pxHmEntry **ppOldEntry = &pOld->pEntryList;
        pxHmEntry **ppNewEntry = &pNew->pEntryList;

        // while there are entries in the bucket
        pxHmEntry *pMoveEntry;
        while((pMoveEntry = *ppOldEntry))
        {
            // check if the newly used hash bit is on
            if (!(pMoveEntry->rawHash & pMap->nBuckets))
                ppOldEntry = &pMoveEntry->pNext;
            else
            {
                // remove it from the original bucket
                *ppOldEntry = pMoveEntry->pNext;

                // add it to end of the new bucket
                *ppNewEntry = pMoveEntry;
                ppNewEntry = &pMoveEntry->pNext;
            }
        }

        // terminate the new list
        *ppNewEntry = NULL;
    }

    // replace the bucket array
    pMap->pBucket = pNewBucket;
    pMap->nBuckets <<= 1;

    // free the old bucket array (so that it gets a second chance)
    pxFree *const pFree = PXINTERFACE_getInterface(pMap->pAlloc, pxFree);
    if (pFree)
        PXFREE_free(pFree, pOldBucket);
}

pxHmEntry *pxHmMapFind(pxHmMap *pMap, const void *pKey,
                       pxHmEntry *(*create)(void *pCtx, pxAlloc *pAlloc),
                       void *pCtx)
{
    pxHashValue rawHash;

    // hash the key
    pxHashInit(&rawHash);
    (*pMap->pDope->hash)(&rawHash, pKey);

    const int iBucket = rawHash & (pMap->nBuckets - 1); // mask power of 2
    pxHmBucket *const pBucket = pMap->pBucket + iBucket;

    pxHmEntry **ppEntry;
    for(ppEntry = &pBucket->pEntryList; *ppEntry; ppEntry = &(*ppEntry)->pNext)
    {
        const int thisHash = (*ppEntry)->rawHash;
        if (rawHash == thisHash)
        {
            const void *const pThisKey = 
                ((char *)(*ppEntry)) + pMap->pDope->keyOffset;
            const int cmp = (*pMap->pDope->cmp)(pKey, pThisKey);
            if (!cmp)
                return *ppEntry;
            else if (cmp > 0)
                break; // we've passed where it would go, so stop looking
        }
        else if (rawHash > thisHash)
            break; // we've passed where it would go, so stop looking
    }

    // if we got here, we didn't find the key, but we're pointing at the
    // insertion point
    if (!create)
        return NULL;
    else
    {
        // call the user to create the new entry
        pxHmEntry *const pEntry = (*create)(pCtx, pMap->pAlloc);

        // verify the key is correct
        const void *const pThisKey = ((char *)pEntry) + pMap->pDope->keyOffset;
        const int cmp = (*pMap->pDope->cmp)(pKey, pThisKey);
        if (cmp)
            pxExit("pxHmMapFind: key in newly created entry is wrong");

        // set our parts
        pEntry->rawHash = rawHash;
        pEntry->pNext = *ppEntry;
        *ppEntry = pEntry;
        ++pMap->nEntries;

        // before leaving, check to see if we need to rehash
        if (pMap->nEntries / pMap->nBuckets > pMap->pDope->avgBucket)
            pxHmMapResize(pMap);

        return pEntry;
    }
}

void pxHmMapInit(pxHmMap *pMap, const pxHmDope *pDope,
                 pxAlloc *pAlloc, size_t initCap)
{
    // don't aim for less than one entry per bucket
    if (pDope->avgBucket < 1)
        pxExit("pxHmMapInit: avgBucket must be >= 1");

    // don't aim for fewer than 5 elements to start
    if (initCap < 5)
        initCap = 5;

    // figure out the initial number of buckets
    // use the first power of two greater than or equal to that requested
    size_t initBucket0 = (initCap + pDope->avgBucket - 1) / pDope->avgBucket;
    size_t initBucket = 1;
    for(; initBucket < initBucket0; initBucket <<= 1)
        ;

    pMap->pBucket = PXALLOC_alloc(pAlloc, sizeof(pxHmBucket) * initBucket, 0);
    pMap->nBuckets = initBucket;
    pMap->nEntries = 0;
    pMap->pDope = pDope;

    // wrap the allocator with pxAllocReuse in order to reuse the bucket array
    pMap->pAlloc = pxAllocReuseCreate(pAlloc);
}
