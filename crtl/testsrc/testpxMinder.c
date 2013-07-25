
#ifndef PXMINDER_H
#include "pxMinder.h"
#endif

#ifndef PXMEMORY_H
#include "pxMemory.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif

#ifndef PX_STDBOOL_H
#include <stdbool.h>
#define PX_STDBOOL_H
#endif

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

typedef struct
{
    pxObjectStruct objectStruct;
    bool isDestroyed;
} MyDestroyable;

static void MyDestroyable_destroy(pxObject *pI)
{
    MyDestroyable *pThis =
        PXINTERFACE_STRUCT(pI, MyDestroyable, objectStruct.pObjectVt);

    if (pThis->isDestroyed)
        fprintf(stderr, "testpxMinder: destroyable destroyed a second time\n");
    pThis->isDestroyed = true;
}

static const pxObjectLookup myDestroyableTable[] =
{
    {pxObjectName, 0},
};

static const pxObjectVt myDestroyableObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    sizeof(myDestroyableTable)/sizeof(myDestroyableTable[0]),
    myDestroyableTable,
    MyDestroyable_destroy,
    pxObject_addMixin,
};

static void MyDestroyableInit(MyDestroyable *pMD)
{
    pxObjectStructInit(&pMD->objectStruct, &myDestroyableObjectVt);
    pMD->isDestroyed = false;
}


static void testpxMinder()
{
    pxMemory *const pMemory = pxMemorySystemGet();
    pxMinder *pMinder;
    pxObject *pObject;

    // test creating and destroying a minder
    pMinder = pxMinderCreate(pMemory);
    pObject = PXINTERFACE_getInterface(pMinder, pxObject);
    PXOBJECT_destroy(pObject);

    MyDestroyable d1, d2;

    // test adding and destroying a single object
    pMinder = pxMinderCreate(pMemory);
    MyDestroyableInit(&d1);
    PXMINDER_register(pMinder, (pxObject *)&d1.objectStruct.pObjectVt);

    pObject = PXINTERFACE_getInterface(pMinder, pxObject);
    PXOBJECT_destroy(pObject);
    if (!d1.isDestroyed)
        fprintf(stderr, "testpxMinder: did not destroy d1 (%d)\n", __LINE__);

    // test adding and destroying more than one object
    pMinder = pxMinderCreate(pMemory);
    MyDestroyableInit(&d1);
    PXMINDER_register(pMinder, (pxObject *)&d1.objectStruct.pObjectVt);
    MyDestroyableInit(&d2);
    PXMINDER_register(pMinder, (pxObject *)&d2.objectStruct.pObjectVt);

    pObject = PXINTERFACE_getInterface(pMinder, pxObject);
    PXOBJECT_destroy(pObject);
    if (!d1.isDestroyed)
        fprintf(stderr, "testpxMinder: did not destroy d1 (%d)\n", __LINE__);
    if (!d2.isDestroyed)
        fprintf(stderr, "testpxMinder: did not destroy d2 (%d)\n", __LINE__);

    pxObject *pu1;

    // test adding multiple objects, but deregistering one before destroying
    pMinder = pxMinderCreate(pMemory);
    MyDestroyableInit(&d1);
    pu1 = PXMINDER_register(pMinder, (pxObject *)&d1.objectStruct.pObjectVt);
    MyDestroyableInit(&d2);
    PXMINDER_register(pMinder, (pxObject *)&d2.objectStruct.pObjectVt);

    PXOBJECT_destroy(pu1); // deregisters d1

    pObject = PXINTERFACE_getInterface(pMinder, pxObject);
    PXOBJECT_destroy(pObject);
    if (d1.isDestroyed)
        fprintf(stderr, "testpxMinder: incorrectly destroyed d1 (%d)\n",
                __LINE__);
    if (!d2.isDestroyed)
        fprintf(stderr, "testpxMinder: did not destroy d2 (%d)\n", __LINE__);
}

int main(void)
{
    pxMemorySystemInit();
    testpxMinder();
    return 0;
}
