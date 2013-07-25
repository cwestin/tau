
#ifndef PXMINDER_H
#include "pxMinder.h"
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


static void testpxMinder()
{
    pxMemory *const pMemory = pxMemorySystemGet();
    pxMinder *pMinder;
    pxObject *pObject;

    pMinder = pxMinderCreate(pMemory);
    pObject = PXINTERFACE_getInterface(pMinder, pxObject);
    PXOBJECT_destroy(pObject);
}

int main(void)
{
    pxMemorySystemInit();
    testpxMinder();
    return 0;
}
