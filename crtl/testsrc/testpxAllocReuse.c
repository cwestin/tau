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

