
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

    pxHashMap *pHashMap = pxHashMapLocalCreate(pAlloc, NULL);

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
