
#ifndef PXHM_H
#include "pxHm.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXEXIT_H
#include "pxExit.h"
#endif


typedef struct pxHmBucket
{
    pxHmEntry *pEntryList;
} pxHmBucket;

pxHmEntry *pxHmMapFind(pxHmMap *pMap, const void *pKey)
{
    pxHashValue rawHash;

    // hash the key
    pxHashInit(&rawHash);
    (*pMap->pDope->hash)(pKey);

    const int iBucket = rawHash & (pMap->nBuckets - 1); // mask power of 2
    pxHmBucket *const pBucket = pMap->pBucket + iBucket;

    // TODO
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
    pMap->pAlloc = pAlloc;
}
