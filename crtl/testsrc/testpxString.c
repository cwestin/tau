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

#ifndef PXALLOCLOCAL_H
#include "pxAllocLocal.h"
#endif

#ifndef PXALLOCSYSTEM_H
#include "pxAllocSystem.h"
#endif

#ifndef PXCOMPARABLE_H
#include "pxComparable.h"
#endif

#ifndef PXHASHABLE_H
#include "pxHashable.h"
#endif

#ifndef PXHASHER_H
#include "pxHasher.h"
#endif

#ifndef PXSTRING_H
#include "pxString.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


static void testpxString()
{
    pxAlloc *const pAllocS = pxAllocSystemGet();
    pxAlloc *const pAllocD = pxAllocDebugCreate(pAllocS, NULL);

    pxString *pS;
    pxObject *pO;

    // test creating a NULL string
    pS = pxStringCreate(pAllocD, NULL);
    if (pS)
        fprintf(stderr, "testpxString: should have created NULL string\n");

    // test creation and destruction
    pS = pxStringCreate(pAllocD, "foo");
    if (!pS)
        fprintf(stderr, "testpxString: should have created non-NULL string\n");
    pO = PXINTERFACE_getInterface(pS, pxObject);
    PXOBJECT_destroy(pO);

    // test comparisons
    pxString *const pFoo1 = pxStringCreate(pAllocD, "foo");
    pxString *const pFoo2 = pxStringCreate(pAllocD, "foo");

    pxComparable *const pC1 = PXINTERFACE_getInterface(pFoo1, pxComparable);
    pxComparable *const pC2 = PXINTERFACE_getInterface(pFoo2, pxComparable);

    if (PXCOMPARABLE_compare(pC1, (pxInterface *)pFoo2))
        fprintf(stderr, "testpxString: incorrect comparison (%d)\n", __LINE__);
    if (PXCOMPARABLE_compare(pC2, (pxInterface *)pC1))
        fprintf(stderr, "testpxString: incorrect comparison (%d)\n", __LINE__);

    pxString *const pBar = pxStringCreate(pAllocD, "bar");
    if (PXCOMPARABLE_compare(pC1, (pxInterface *)pBar) <= 0)
        fprintf(stderr, "testpxString: incorrect comparison (%d)\n", __LINE__);

    // try hashing pBar using a local hasher
    pxHashable *const pHashable = PXINTERFACE_getInterface(pBar, pxHashable);
    PXALLOCLOCAL_SPACE(40) hashSpace;
    pxAlloc *const pAllocL = PXALLOCLOCAL_INIT(&hashSpace);
    pxHasher *const pHasher = pxHasherCreate(pAllocL, NULL);
    PXHASHABLE_hash(pHashable, pHasher);

    // try cloning pBar into another allocator
    pxAlloc *const pAllocD2 = pxAllocDebugCreate(pAllocS, NULL);
    pxObject *const pBarO = PXINTERFACE_getInterface(pBar, pxObject);
    pxObjectCloner cloner;
    pxObjectClonerInitSingle(&cloner, pAllocD2);
    pxString *const pBar2 =
        (pxString *)PXOBJECT_clone(pBarO, pxStringName, &cloner);
    pxObjectClonerCleanup(&cloner);

    if (pBar2 == pBar)
        fprintf(stderr, "testpxString: cloning didn't give new string\n");
    pxComparable *const pBarC = PXINTERFACE_getInterface(pBar, pxComparable);
    if (PXCOMPARABLE_compare(pBarC, (pxInterface *)pBar2))
        fprintf(stderr, "testpxString: cloned string was different\n");

    pO = PXINTERFACE_getInterface(pBar2, pxObject);
    PXOBJECT_destroy(pO);
    pO = PXINTERFACE_getInterface(pAllocD2, pxObject);
    PXOBJECT_destroy(pO);

    // clean up the strings
    pO = PXINTERFACE_getInterface(pFoo1, pxObject);
    PXOBJECT_destroy(pO);
    pO = PXINTERFACE_getInterface(pC2, pxObject);
    PXOBJECT_destroy(pO);
    pO = PXINTERFACE_getInterface(pBar, pxObject);
    PXOBJECT_destroy(pO);

    // check the debug allocator
    pxAllocDebug *const pAllocDebug =
        PXINTERFACE_getInterface(pAllocD, pxAllocDebug);
    const unsigned remainingPieces = PXALLOCDEBUG_countPieces(pAllocDebug);
    if (remainingPieces)
        fprintf(stderr, "there is still memory in use (%u pieces)\n",
                remainingPieces);
}

int main(void)
{
    pxAllocSystemInit();
    testpxString();
    return 0;
}
