
#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

#ifndef PXALIGN_H
#include "pxAlign.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXALLOCDEBUG_H
#include "pxAllocDebug.h"
#endif

#ifndef PXALLOCEXTENT_H
#include "pxAllocExtent.h"
#endif

#ifndef PXALLOCREUSE_H
#include "pxAllocReuse.h"
#endif

#ifndef PXALLOCSYSTEM_H
#include "pxAllocSystem.h"
#endif


static unsigned testpxAllocReuse_finishExtent(
    pxAlloc *pAlloc, pxAllocDebug *pAllocDebug)
{
    const unsigned startPieces = PXALLOCDEBUG_countPieces(pAllocDebug);
    unsigned count = 0;
    while(true)
    {
        PXALLOC_alloc(pAlloc, sizeof(pxAlignAll), 0);
        ++count;
        const unsigned nowPieces = PXALLOCDEBUG_countPieces(pAllocDebug);
        if (nowPieces > startPieces)
            return count;
    }
}

static void testpxAllocReuse()
{
    pxAlloc *const pAllocS = pxAllocSystemGet();

    pxAlloc *pAllocD;
    pxAllocDebug *pAllocDebug;
    pxAlloc *pAllocE;
    pxAlloc *pAlloc;
    pxAllocReuse *pAllocReuse;

    // count how many additional allocations we can allocate without reuse
    // before we allocate a second extent
    pAllocD = pxAllocDebugCreate(pAllocS, NULL);
    pAllocDebug = PXINTERFACE_getInterface(pAllocD, pxAllocDebug);
    pAllocE = pxAllocExtentCreate(pAllocD, 1024, NULL);
    pAlloc = pxAllocReuseCreate(pAllocE);

    if (PXALLOCDEBUG_countPieces(pAllocDebug) != 1)
        fprintf(stderr,
                "testpxAllocReuse: there should only be one piece (%d)\n",
                __LINE__);

    PXALLOC_alloc(pAlloc, 512, 0);

    if (PXALLOCDEBUG_countPieces(pAllocDebug) != 1)
        fprintf(stderr,
                "testpxAllocReuse: there should only be one piece (%d)\n",
                __LINE__);

    unsigned const nAllocs = testpxAllocReuse_finishExtent(pAlloc, pAllocDebug);

    // repeat that with reuse; the number of pieces had better be greater
    pAllocD = pxAllocDebugCreate(pAllocS, NULL);
    pAllocDebug = PXINTERFACE_getInterface(pAllocD, pxAllocDebug);
    pAllocE = pxAllocExtentCreate(pAllocD, 1024, NULL);
    pAlloc = pxAllocReuseCreate(pAllocE);
    pAllocReuse = PXINTERFACE_getInterface(pAlloc, pxAllocReuse);

    void *p = PXALLOC_alloc(pAlloc, 512, 0);
    PXALLOCREUSE_reuse(pAllocReuse, p, 512);

    unsigned const nAllocsReuse =
        testpxAllocReuse_finishExtent(pAlloc, pAllocDebug);
    if (nAllocsReuse <= nAllocs)
        fprintf(stderr, "didn't reuse any space\n");
}

int main(void)
{
    pxAllocSystemInit();
    testpxAllocReuse();

    return 0;
}

