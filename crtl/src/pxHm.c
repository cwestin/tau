/*
  tau - http://github.com/cwestin/tau
  Copyright 2013 Chris Westin

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

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


/*
  Each bucket is a linked list of entries, kept sorted in ascending order of
  raw hash value.
*/
typedef struct pxHmBucket
{
    pxHmEntry *pEntryList;
} pxHmBucket;

/**
   Resize the hash map.

   This will double the size of the bucket array. Assumes the bucket array size
   is already a power of two. For each existing bucket, will traverse the
   bucket and move those elements which should now be in the newly created
   higher power bucket to that bucket.

   @param pMap the hash map.
 */
static void pxHmMapResize(pxHmMap *pMap)
{
    pxHmBucket *const pOldBucket = pMap->pBucket;
    pxHmBucket *const pNewBucket =
        PXALLOC_alloc(pMap->pAlloc,
                      (sizeof(pxHmBucket) * pMap->nBuckets) << 1, 0);

    // copy the list heads for the old buckets
    pxHmBucket *pOld = pOldBucket;
    pxHmBucket *pNew = pNewBucket;
    for(int n = pMap->nBuckets; n; ++pOld, ++pNew, --n)
    {
        pNew->pEntryList = pOld->pEntryList;
    }

    // move items from existing buckets to new if their hash dictates it
    pOld = pNewBucket;
    pNew = pNewBucket + pMap->nBuckets;
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
            {
                // this entry is already where it belongs, skip over it
                ppOldEntry = &pMoveEntry->pNext;
            }
            else
            {
                // remove it from the original bucket
                // (but keep emptying the old entry list)
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
            pxExit("pxHmMapFind: key in newly created entry is wrong\n");

        // set our parts
        pEntry->rawHash = rawHash;
        pEntry->pNext = *ppEntry;
        *ppEntry = pEntry;
        ++pMap->nEntries;

        // before leaving, check to see if we need to rehash
        const size_t avgBucket = pMap->pDope->avgBucket;
        if ((pMap->nEntries + avgBucket - 1) / pMap->nBuckets > avgBucket)
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
