
#ifndef PX_STDBOOL_H
#include <stdbool.h>
#define PX_STDBOOL_H
#endif

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXALLOCEXTENT_H
#include "pxAllocExtent.h"
#endif

#ifndef PXALLOCSYSTEM_H
#include "pxAllocSystem.h"
#endif

#ifndef PXCMP_H
#include "pxCmp.h"
#endif

#ifndef PXHASH_H
#include "pxHash.h"
#endif

#ifndef PXHM_H
#include "pxHm.h"
#endif


typedef struct
{
    int i;

    pxHmEntry hmEntry;
} MyStruct;

static const pxHmDope MyStructDope =
{
    /* avgBucket */ 2, 
    /* keyOffset */ offsetof(MyStruct, i) - offsetof(MyStruct, hmEntry),
    /* hash */ pxHashInt,
    /* cmp */ pxCmpInt,
};

typedef struct
{
    int key;
    bool createCalled;
} MyStruct_createCtx;

static inline void MyStruct_createInit(MyStruct_createCtx *pCtx, int key)
{
    pCtx->key = key;
    pCtx->createCalled = false;
}

static pxHmEntry *MyStruct_create(void *pctx, pxAlloc *pAlloc)
{
    MyStruct_createCtx *const pCtx = (MyStruct_createCtx *)pctx;
    MyStruct *const pMyStruct = PXALLOC_alloc(pAlloc, sizeof(MyStruct), 0);
    pMyStruct->i = pCtx->key;
    pCtx->createCalled = true;
    return &pMyStruct->hmEntry;
}

static void testpxHm()
{
    pxAlloc *const pAllocS = pxAllocSystemGet();
    pxAlloc *const pAllocE = pxAllocExtentCreate(pAllocS, 1024, NULL);

    pxHmMap hmMap;
    pxHmMap *const pMap = &hmMap;
    pxHmMapInit(pMap, &MyStructDope, pAllocE, 1);

    const int nTrials = 57;
    MyStruct *pStruct[nTrials];
    for(int i = 0; i < nTrials; ++i)
    {
        pxHmEntry *pEntry;

        pEntry = pxHmMapFind(pMap, &i, NULL, NULL);
        if (pEntry)
            fprintf(stderr, "found entry when it shouldn't exist (%d, %d)\n",
                    i, __LINE__);

        if (pxHmMapCount(pMap) != i)
            fprintf(stderr, "incorrect count (%d, %d)\n", i, __LINE__);

        MyStruct_createCtx ctx;
        MyStruct_createInit(&ctx, i);
        pEntry = pxHmMapFind(pMap, &i, MyStruct_create, &ctx);
        if (!pEntry)
            fprintf(stderr, "didn't find entry when it should exist (%d, %d)\n",
                    i, __LINE__);
        if (!ctx.createCalled)
            fprintf(stderr, "didn't call creation function when should have\n");

        pStruct[i] = PXHM_STRUCT(pEntry, MyStruct, hmEntry);

        if (pxHmMapCount(pMap) != i + 1)
            fprintf(stderr, "incorrect count after creation (%d, %d)\n", i, __LINE__);

        MyStruct_createInit(&ctx, i);
        pEntry = pxHmMapFind(pMap, &i, MyStruct_create, &ctx);
        if (!pEntry)
            fprintf(stderr, "didn't find entry when it should exist (%d, %d)\n",
                    i, __LINE__);
        if (ctx.createCalled)
            fprintf(stderr, "called creation function when shouldn't have (%d, %d)\n", i, __LINE__);

        if (pEntry != &pStruct[i]->hmEntry)
            fprintf(stderr, "found entry doesn't match original (%d, %d)\n", i, __LINE__);

        if (pxHmMapCount(pMap) != i + 1)
            fprintf(stderr, "incorrect count after creation (%d, %d)\n", i, __LINE__);
    }

    for(int i = 0; i < nTrials; ++i)
    {
        pxHmEntry *pEntry;

        pEntry = pxHmMapFind(pMap, &i, NULL, NULL);
        if (!pEntry)
            fprintf(stderr, "didn't find entry when it should exist (%d, %d)\n",
                    i, __LINE__);
    }
}

int main(void)
{
    pxAllocSystemInit();
    testpxHm();

    return 0;
}
