
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
    /* keyOffset */ offsetof(MyStruct, i),
    /* hash */ pxHashInt,
    /* cmp */ pxCmpInt,
};

static void testpxHm()
{
    pxAlloc *const pAllocS = pxAllocSystemGet();
    pxAlloc *const pAllocE = pxAllocExtentCreate(pAllocS, 1024, NULL);

    pxHmMap hmMap;
    pxHmMapInit(&hmMap, &MyStructDope, pAllocE, 1);
}

int main(void)
{
    pxAllocSystemInit();
    testpxHm();

    return 0;
}
