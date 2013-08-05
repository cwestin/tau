
#ifndef PXHASHMAPLOCAL_H
#include "pxHashMapLocal.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXALLOCSYSTEM_H
#include "pxAllocSystem.h"
#endif

#ifndef PXALLOCEXTENT_H
#include "pxAllocExtent.h"
#endif

#ifndef PXHASHABLEVAR_H
#include "pxHashableVar.h"
#endif

#ifndef PXHASHMAP_H
#include "pxHashMap.h"
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

#ifndef PX_STDIO_H
#include <stdio.h>
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
