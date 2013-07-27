
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

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif


static void testpxAllocExtent()
{
    pxAlloc *const pAllocS = pxAllocSystemGet();
    pxAlloc *const pAllocD = pxAllocDebugCreate(pAllocS);
    pxAllocDebug *const pAllocDebug =
        PXINTERFACE_getInterface(pAllocD, pxAllocDebug);
    pxAlloc *pAlloc;
    pxObject *pObject;

    pAlloc = pxAllocExtentCreate(pAllocD, 1024);
    pObject = PXINTERFACE_getInterface(pAlloc, pxObject);
    PXOBJECT_destroy(pObject);
    if (!PXALLOCDEBUG_isEmpty(pAllocDebug))
        fprintf(stderr, "basic creation and destruction failed\n");
}

int main(void)
{
    pxAllocSystemInit();
    testpxAllocExtent();
    return 0;
}
