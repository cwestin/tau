// intrusive hashmap for high-performance applications

#ifndef PXHM_H
#define PXHM_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif

#ifndef PXHASH_H
#include "pxHash.h"
#endif


struct pxAlloc;

typedef struct pxHmEntry
{
// private:    
    struct pxHmEntry *pNext;
    pxHashValue rawHash;
} pxHmEntry;

#define PXHM_STRUCT(pEntry, sname, ename) \
    ((sname *)(((char *)(pEntry)) - offsetof(sname, ename)))

typedef struct
{
    size_t avgBucket;
    ptrdiff_t keyOffset;
    pxHashValue (*hash)(const void *p);
    int (*cmp)(const void *pL, const void *pR);
} pxHmDope;

typedef struct
{
// private:    
    struct pxHmBucket *pBucket;
    size_t nBuckets;
    size_t nEntries;

    const pxHmDope *pDope;

    struct pxAlloc *pAlloc;
} pxHmMap;

void pxHmMapInit(pxHmMap *pMap, const pxHmDope *pDope,
                 struct pxAlloc *pAlloc, size_t initCap);

pxHmEntry *pxHmMapFind(pxHmMap *pMap, const void *pKey,
               pxHmEntry *(*create)(void *pCtx, struct pxAlloc *pAlloc),
               void *pCtx);

#endif // PXHM_H
