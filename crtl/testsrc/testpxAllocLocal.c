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

#ifndef PXALLOCLOCAL_H
#include "pxAllocLocal.h"
#endif

#ifndef PXHASHER_H
#include "pxHasher.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


static void testpxAllocLocal()
{
    pxAlloc *pAlloc;
    pxHasher *pHasher;
    const int x = 5;
    PXALLOCLOCAL_SPACE(48) stackSpace;
    long guard = 0xdeadbeef;

    pAlloc = PXALLOCLOCAL_INIT(&stackSpace);
    pHasher = pxHasherCreate(pAlloc, NULL);

    PXHASHER_hashBytes(pHasher, &x, sizeof(x));
    PXHASHER_hashBytes(pHasher, &x, sizeof(x));
    PXHASHER_getHash(pHasher);

    // clean up
    pxObject *const pAllocObject = PXINTERFACE_getInterface(pAlloc, pxObject);
    PXOBJECT_destroy(pAllocObject);

    if (guard != 0xdeadbeef)
        fprintf(stderr, "testpxAllocLocal: overwrote the guard!\n");
}

int main(void)
{
    testpxAllocLocal();

    return 0;
}
