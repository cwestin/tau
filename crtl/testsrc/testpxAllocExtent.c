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

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXALLOCDEBUG_H
#include "pxAllocDebug.h"
#endif

#ifndef PXALLOCEXTENT_H
#include "pxAllocExtent.h"
#endif

#ifndef PXALLOCSYSTEM_H
#include "pxAllocSystem.h"
#endif

#ifndef PXFREE_H
#include "pxFree.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


static void testpxAllocExtent()
{
    pxAlloc *const pAllocS = pxAllocSystemGet();
    pxAlloc *const pAllocD = pxAllocDebugCreate(pAllocS, NULL);
    pxAllocDebug *const pAllocDebug =
        PXINTERFACE_getInterface(pAllocD, pxAllocDebug);
    pxAlloc *pAlloc;
    pxObject *pObject;

    // test creation and destruction
    pAlloc = pxAllocExtentCreate(pAllocD, 1024, NULL);
    pObject = PXINTERFACE_getInterface(pAlloc, pxObject);
    PXOBJECT_destroy(pObject);
    if (PXALLOCDEBUG_countPieces(pAllocDebug) != 0)
        fprintf(stderr, "incorrect allocated piece count (%d)\n", __LINE__);
    if (!PXALLOCDEBUG_isEmpty(pAllocDebug))
        fprintf(stderr, "destruction failed to clean up (%d)\n", __LINE__);

    // test allocation within an extent and beyond
    pAlloc = pxAllocExtentCreate(pAllocD, 1024, NULL);
    pObject = PXINTERFACE_getInterface(pAlloc, pxObject);
    if (PXALLOCDEBUG_countPieces(pAllocDebug) != 1)
        fprintf(stderr, "incorrect allocated piece count (%d)\n", __LINE__);

    // consume most of the extent
    PXALLOC_alloc(pAlloc, 330, 0);
    PXALLOC_alloc(pAlloc, 330, 0);
    PXALLOC_alloc(pAlloc, 330, 0);
    if (PXALLOCDEBUG_countPieces(pAllocDebug) != 1)
        fprintf(stderr, "incorrect allocated piece count (%d)\n", __LINE__);

    // cause a second extent to be allocated
    PXALLOC_alloc(pAlloc, 330, 0);
    if (PXALLOCDEBUG_countPieces(pAllocDebug) != 2)
        fprintf(stderr, "incorrect allocated piece count (%d)\n", __LINE__);

    PXOBJECT_destroy(pObject);
    if (!PXALLOCDEBUG_isEmpty(pAllocDebug))
        fprintf(stderr, "destruction failed to clean up (%d)\n", __LINE__);
}

int main(void)
{
    pxAllocSystemInit();
    testpxAllocExtent();
    return 0;
}
