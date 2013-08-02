
#ifndef PXALLOCLOCAL_H
#include "pxAllocLocal.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
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
    PXALLOCLOCAL_SPACE(96) stackSpace;

    pAlloc = PXALLOCLOCAL_INIT(&stackSpace);
    pHasher = pxHasherCreate(pAlloc, NULL);

    PXHASHER_hashBytes(pHasher, &x, sizeof(x));
    PXHASHER_hashBytes(pHasher, &x, sizeof(x));
    PXHASHER_getHash(pHasher);

    // clean up
    pxObject *const pAllocObject = PXINTERFACE_getInterface(pAlloc, pxObject);
    PXOBJECT_destroy(pAllocObject);
}

int main(void)
{
    testpxAllocLocal();

    return 0;
}
