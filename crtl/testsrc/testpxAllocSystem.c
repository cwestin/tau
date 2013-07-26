
#ifndef PXINTERFACE_H
#include "pxInterface.h"
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

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif


static void testpxAllocSystem()
{
    pxAllocSystemInit();
    pxAlloc *const pAlloc = pxAllocSystemGet();

    pxAlloc *const pM2 = PXINTERFACE_getInterface(pAlloc, pxAlloc);
    if (pM2 != pAlloc)
        fprintf(stderr, "pxAlloc interface recovery failure\n");

    pxObject *const pObject = PXINTERFACE_getInterface(pAlloc, pxObject);
    if (pObject == NULL)
        fprintf(stderr, "pxObject interface request failure\n");

    pxAlloc *const pM3 = PXINTERFACE_getInterface(pObject, pxAlloc);
    if (pM3 != pAlloc)
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
}

int main(void)
{
    testpxAllocSystem();
    return 0;
}
