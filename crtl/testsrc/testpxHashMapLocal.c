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

#ifndef PXALLOCEXTENT_H
#include "pxAllocExtent.h"
#endif

#ifndef PXALLOCSYSTEM_H
#include "pxAllocSystem.h"
#endif

#ifndef PXHASHABLEVAR_H
#include "pxHashableVar.h"
#endif

#ifndef PXHASHMAP_H
#include "pxHashMap.h"
#endif

#ifndef PXHASHMAPLOCAL_H
#include "pxHashMapLocal.h"
#endif

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

#ifndef PXMINDER_H
#include "pxMinder.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


static void testpxHashMapLocal()
{
    pxAlloc *const pAS = pxAllocSystemGet();
    pxAlloc *const pAlloc = pxAllocExtentCreate(pAS, 1024, NULL);

    pxHashMap *pHashMap = pxHashMapLocalCreate(pAlloc, NULL, 3, 3);
    if (!PXHASHMAP_isEmpty(pHashMap))
        fprintf(stderr, "hashmap should be empty, but isn't (%d)\n", __LINE__);

    const int nTrials = 57;
    pxInterface *pObject[nTrials];
    for(int i = 0; i < nTrials; ++i)
    {
        pxHashableVar_s hv;
        struct pxHashable *const pHashable =
            pxHashableVarInit(&hv, &i, sizeof(i), NULL);

        pxMinder *const pO = pxMinderCreate(pAlloc, NULL);
        pObject[i] = (pxInterface *)pO;

        PXHASHMAP_put(pHashMap, pHashable, (pxInterface *)pO);

        pxInterface *pI = PXHASHMAP_get(pHashMap, pHashable);
        if (pI == NULL)
            fprintf(stderr, "didn't find newly inserted object\n");
        else if (pI != (pxInterface *)pO)
            fprintf(stderr, "newly inserted object didn't match original\n");
    }

    if (PXHASHMAP_isEmpty(pHashMap))
        fprintf(stderr, "hashmap shouldn't be empty, but is (%d)\n", __LINE__);

    // TODO

    // clean up
    pxObject *const pAllocObject = PXINTERFACE_getInterface(pAlloc, pxObject);
    PXOBJECT_destroy(pAllocObject);
}

int main(void)
{
    pxAllocSystemInit();
    testpxHashMapLocal();

    return 0;
}
