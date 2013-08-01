
#ifndef PXHASHABLEVAR_H
#include "pxHashableVar.h"
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

#ifndef PXCOMPARABLE_H
#include "pxComparable.h"
#endif

#ifndef PXHASHABLE_H
#include "pxHashable.h"
#endif

#ifndef PXHASHER_H
#include "pxHasher.h"
#endif

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif


static int testpxHashableVarGetHash(pxHashable *pH, pxAlloc *pA)
{
    pxHasher *pHasher = pxHasherCreate(pA, NULL);
    PXHASHABLE_hash(pH, pHasher);
    return PXHASHER_getHash(pHasher);
}

static void testpxHashableVarInt()
{
    pxAlloc *const pAS = pxAllocSystemGet();
    pxAlloc *const pAlloc = pxAllocExtentCreate(pAS, 1024, NULL);

    int x1 = 11;
    pxHashableVar_s v1;
    pxHashable *const pH1 = pxHashableVarInit(&v1, &x1, sizeof(x1), NULL);
    pxComparable *const pC1 = PXINTERFACE_getInterface(pH1, pxComparable);
    const int hash1 = testpxHashableVarGetHash(pH1, pAlloc);

    int x2 = 5;
    pxHashableVar_s v2;
    pxHashable *const pH2 = pxHashableVarInit(&v2, &x2, sizeof(x2), NULL);
    pxComparable *const pC2 = PXINTERFACE_getInterface(pH2, pxComparable);
    const int hash2 = testpxHashableVarGetHash(pH2, pAlloc);

    int x3 = 11;
    pxHashable *const pH3 = pxHashableVarCreate(pAlloc, &x3, sizeof(x3), NULL);
    pxComparable *const pC3 = PXINTERFACE_getInterface(pH3, pxComparable);
    const int hash3 = testpxHashableVarGetHash(pH3, pAlloc);

    if (hash1 == hash2)
        fprintf(stderr, "hashes are the same, but shouldn't be\n");
    if (hash1 != hash3)
        fprintf(stderr, "hashes are different, but shouldn't be\n");
    if (PXCOMPARABLE_compare(pC1, (pxInterface *)pH2) <= 0)
        fprintf(stderr, "incorrect comparison (%d)\n", __LINE__);
    if (PXCOMPARABLE_compare(pC2, (pxInterface *)pH1) >= 0)
        fprintf(stderr, "incorrect comparison (%d)\n", __LINE__);
    if (PXCOMPARABLE_compare(pC1, (pxInterface *)pH3) != 0)
        fprintf(stderr, "incorrect comparison (%d)\n", __LINE__);
    if (PXCOMPARABLE_compare(pC3, (pxInterface *)pC1) != 0)
        fprintf(stderr, "incorrect comparison (%d)\n", __LINE__);
    
    // clean up everything
    pxObject *const pAllocObject = PXINTERFACE_getInterface(pAlloc, pxObject);
    PXOBJECT_destroy(pAllocObject);
}

int main(void)
{
    pxAllocSystemInit();
    testpxHashableVarInt();

    return 0;
}
