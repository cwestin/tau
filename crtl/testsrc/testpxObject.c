
#ifndef PXINTERFACE_H
#include "pxInterface.h"
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


struct fooInterface;

typedef struct
{
    PXINTERFACE_GET(fooInterface);
    void (*f)(struct fooInterface *const pFoo, int i);
} fooInterfaceVt;

typedef struct
{
    pxObjectInterfacePrefix prefix;
    fooInterfaceVt vt;
} fooInterfaceMeta;

typedef struct fooInterface
{
    const fooInterfaceVt *pVt;
} fooInterface;

static const char fooInterfaceName[] = "fooInterfaceName";

typedef struct myObject
{
    const fooInterfaceVt *pFooVt;
    const pxObjectVt *pObjectVt;
    int i;
} myObject;

static void myObject_f(fooInterface *const pFoo, int i)
{
    myObject *const pThis = PXINTERFACE_OBJECT(pFoo, myObject, pFooVt);

    pThis->i += i;
}

static const fooInterfaceMeta fooMeta =
{
    {offsetof(myObject, pObjectVt) - offsetof(myObject, pFooVt)},
    { 
        PXOBJECT_GETINTERFACE(fooInterface),
        myObject_f,
    }
};


static const pxObjectLookup interfaceTable[] =
{
    {fooInterfaceName, offsetof(myObject, pObjectVt) - offsetof(myObject, pFooVt)},
    {pxObjectName, 0},
};

static const pxObjectObjectMeta objectObjectMeta =
{
    sizeof(interfaceTable)/sizeof(interfaceTable[0]),
    interfaceTable,
    {
        {0},
        {
            PXOBJECT_GETINTERFACE(pxObject),
            NULL, // TODO
        },
    },
};

static void testpxObject()
{
    myObject *const pM = (myObject *)malloc(sizeof(myObject));
    pM->pFooVt = &fooMeta.vt;
    pM->pObjectVt = &objectObjectMeta.objectMeta.vt;

    pM->i = 0;

    fooInterface *const pFoo = (fooInterface *)&pM->pFooVt;
    (*pFoo->pVt->f)(pFoo, 1);
    if (pM->i != 1)
        fprintf(stderr, "foo interface increment failed\n");

    pxObject *const pObject = (pxObject *)&pM->pObjectVt;

    pxObject *const pObject2 = PXINTERFACE_getInterface(pObject, pxObject);
    if (pObject2 != pObject)
        fprintf(stderr, "pxObject interface recovery failure\n");

    pxObject *const pObject3 = PXINTERFACE_getInterface(pFoo, pxObject);
    if (pObject3 != pObject)
        fprintf(stderr, "pxObject interface request failure\n");

    fooInterface *const pFoo2 = PXINTERFACE_getInterface(pObject, fooInterface);
    if (pFoo2 != pFoo)
        fprintf(stderr, "fooInterface interface recovery failure\n");

    free(pM);
}

int main(void)
{
    testpxObject();
    return 0;
}
