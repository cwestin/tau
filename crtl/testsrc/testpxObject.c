/*
  tau - http://github.com/cwestin/tau
  Copyright 2013 Chris Westin

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


// this is how you declare an interface
struct pxFoo;
typedef struct pxFooVt
{
    pxInterfaceVt interfaceVt;
    void (*f)(struct pxFoo *pFoo, int i);
} pxFooVt;

typedef struct pxFoo
{
    const pxFooVt *const pVt;
} pxFoo;

static const char pxFooName[] = "pxFooName";

#define PXFOO_f(pI, i) \
    ((*(pI)->pVt->f)(pI, i))


// this is how you build an object that implements that interface
typedef struct MyObject
{
    const pxFooVt *pFooVt;
    pxObjectStruct objectStruct;
    int i;
} MyObject;

static void MyObject_f(pxFoo *pFoo, int i)
{
    MyObject *const pThis = PXINTERFACE_STRUCT(pFoo, MyObject, pFooVt);

    pThis->i += i;
}

static const pxFooVt fooVt =
{
    {
        offsetof(MyObject, objectStruct.pObjectVt) - offsetof(MyObject, pFooVt),
        pxObject_getInterface,
    },
    MyObject_f,
};


static const pxObjectLookup interfaceTable[] =
{
    {pxFooName, offsetof(MyObject, objectStruct.pObjectVt) - offsetof(MyObject, pFooVt)},
    {pxObjectName, 0},
};

static const pxObjectVt objectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    sizeof(interfaceTable)/sizeof(interfaceTable[0]),
    interfaceTable,
    pxObject_destroy,
};

static void testpxObject()
{
    // initialize the object
    MyObject *const pM = (MyObject *)malloc(sizeof(MyObject));
    pM->pFooVt = &fooVt;
    pxObjectStructInit(&pM->objectStruct, &objectVt, NULL);
    pM->i = 0;

    // try calling a method
    pxFoo *const pFoo = (pxFoo *)&pM->pFooVt;
    PXFOO_f(pFoo, 1);
    if (pM->i != 1)
        fprintf(stderr, "foo interface increment failed\n");

    // check on the object interface implementation
    pxObject *const pObject = PXINTERFACE_getInterface(pFoo, pxObject);
    if (!pObject)
        fprintf(stderr, "pxObject interface request failure\n");

    pxObject *const pObject2 = PXINTERFACE_getInterface(pObject, pxObject);
    if (pObject2 != pObject)
        fprintf(stderr, "pxObject interface recovery failure\n");

    pxFoo *const pFoo2 = PXINTERFACE_getInterface(pObject, pxFoo);
    if (pFoo2 != pFoo)
        fprintf(stderr, "pxFoo interface recovery failure\n");

    PXOBJECT_destroy(pObject);

    free(pM);
}

int main(void)
{
    testpxObject();
    return 0;
}
