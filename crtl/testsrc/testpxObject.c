
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

typedef struct fooInterfaceVt
{
    void (*f)(struct fooInterface *const pFoo, int i);
} fooInterfaceVt;

typedef struct fooInterface
{
    const fooInterfaceVt *pVt;
} fooInterface;

typedef struct myObject
{
    const fooInterfaceVt *pFooVt;
    const pxObjectVt *pObjectVt;
    int i;
} myObject;

static void myObject_f(fooInterface *const pFoo, int i)
{
    myObject *const pThis = pxInterfaceObject(pFoo, myObject, pFooVt);
    pThis->i += i;
}

static const fooInterfaceVt myObject_fooVt =
{
    myObject_f
};

static void testpxObject()
{
    myObject *pM = (myObject *)malloc(sizeof(myObject));
    pM->i = 0;
    pM->pFooVt = &myObject_fooVt;

    fooInterface *pFoo = (fooInterface *)&pM->pFooVt;
    (*pFoo->pVt->f)(pFoo, 1);
    if (pM->i != 1)
        fprintf(stderr, "foo interface increment failed\n");

    free(pM);
}

int main(void)
{
    testpxObject();
    return 0;
}
