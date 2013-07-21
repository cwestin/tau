
#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

#ifndef PXMEMORY_H
#include "pxMemory.h"
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


static void testpxMemorySystem()
{
    pxMemorySystemInit();
    pxMemory *const pMemory = pxMemorySystemGet();

    pxMemory *const pM2 = PXINTERFACE_getInterface(pMemory, pxMemory);
    if (pM2 != pMemory)
        fprintf(stderr, "pxMemory interface recovery failure\n");

    pxObject *const pObject = PXINTERFACE_getInterface(pMemory, pxObject);
    if (pObject == NULL)
        fprintf(stderr, "pxObject interface request failure\n");
    pxMemory *const pM3 = PXINTERFACE_getInterface(pObject, pxMemory);
    if (pM3 != pMemory)
        fprintf(stderr, "pxMemory interface recovery failure\n");

    const size_t nBytes = 15;
    void *p = PXMEMORY_alloc(pMemory, nBytes, 0);
    for(int i = 0; i < nBytes; ++i)
    {
        if (((char *)p)[i] != 0)
            fprintf(stderr, "pxMemory allocation wasn't zeroed\n");
    }

    PXMEMORY_free(pMemory, p);
}

int main(void)
{
    testpxMemorySystem();
    return 0;
}
