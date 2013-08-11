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

#ifndef PXALLOCSYSTEM_H
#include "pxAllocSystem.h"
#endif

#ifndef PXFREE_H
#include "pxFree.h"
#endif

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


static void testpxAllocDebug()
{
    pxAlloc *const pAllocSystem = pxAllocSystemGet();
    pxAlloc *const pAlloc0 = pxAllocDebugCreate(pAllocSystem, NULL);
    pxAlloc *const pAlloc = pxAllocDebugCreate(pAlloc0, NULL);
    pxFree *const pFree = PXINTERFACE_getInterface(pAlloc, pxFree);
    pxAllocDebug *const pAllocDebug =
        PXINTERFACE_getInterface(pAlloc, pxAllocDebug);

    if (!PXALLOCDEBUG_isEmpty(pAllocDebug))
        fprintf(stderr, "pxAllocDebug should start out empty\n");

    void *p0, *p1;
    p0 = PXALLOC_alloc(pAlloc, 1, 0);
    if (PXALLOCDEBUG_isEmpty(pAllocDebug))
        fprintf(stderr, "pxAllocDebug should not be empty (%d)\n", __LINE__);
    PXFREE_free(pFree, p0);
    if (!PXALLOCDEBUG_isEmpty(pAllocDebug))
        fprintf(stderr, "pxAllocDebug should be empty (%d)\n", __LINE__);

    p0 = PXALLOC_alloc(pAlloc, 1, 0);
    p1 = PXALLOC_alloc(pAlloc, 10, 0);
    PXFREE_free(pFree, p0);
    if (PXALLOCDEBUG_isEmpty(pAllocDebug))
        fprintf(stderr, "pxAllocDebug should not be empty (%d)\n", __LINE__);
    PXFREE_free(pFree, p1);
    if (!PXALLOCDEBUG_isEmpty(pAllocDebug))
        fprintf(stderr, "pxAllocDebug should be empty (%d)\n", __LINE__);

    p0 = PXALLOC_alloc(pAlloc, 1, 0);
    p1 = PXALLOC_alloc(pAlloc, 10, 0);
    pxObject *const pObject = PXINTERFACE_getInterface(pAlloc, pxObject);
    PXOBJECT_destroy(pObject);

    pxAllocDebug *const pAllocDebug0 =
        PXINTERFACE_getInterface(pAlloc0, pxAllocDebug);
    if (!PXALLOCDEBUG_isEmpty(pAllocDebug0))
        fprintf(stderr, "pxAllocDebug_destroy failed to free everything\n");
}

int main(void)
{
    pxAllocSystemInit();
    testpxAllocDebug();
    return 0;
}
