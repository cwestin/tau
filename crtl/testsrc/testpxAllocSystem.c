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

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
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


static void testpxAllocSystem()
{
    pxAllocSystemInit();
    pxAlloc *const pAlloc = pxAllocSystemGet();

    pxAlloc *const pA2 = PXINTERFACE_getInterface(pAlloc, pxAlloc);
    if (pA2 != pAlloc)
        fprintf(stderr, "pxAlloc interface recovery failure\n");

    pxObject *const pObject = PXINTERFACE_getInterface(pAlloc, pxObject);
    if (pObject == NULL)
        fprintf(stderr, "pxObject interface request failure\n");

    pxAlloc *const pA3 = PXINTERFACE_getInterface(pObject, pxAlloc);
    if (pA3 != pAlloc)
        fprintf(stderr, "pxAlloc interface recovery failure\n");

    const size_t nBytes = 15;
    void *p = PXALLOC_alloc(pAlloc, nBytes, 0);
    for(int i = 0; i < nBytes; ++i)
    {
        if (((char *)p)[i] != 0)
            fprintf(stderr, "pxAlloc allocation wasn't zeroed\n");
    }

    pxFree *const pFree = PXINTERFACE_getInterface(pAlloc, pxFree);
    if (!pFree)
        fprintf(stderr, "pxFree interface request failure\n");
    PXFREE_free(pFree, p);

    // this should work without complaint, because destroying the system
    // allocator does nothing
    PXOBJECT_destroy(pObject);
    if (pxAllocSystemGet() != pAlloc)
        fprintf(stderr, "destroying the system allocator did something\n");
    p = PXALLOC_alloc(pAlloc, nBytes, 0);
    PXFREE_free(pFree, p);
}

int main(void)
{
    testpxAllocSystem();
    return 0;
}
