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

#ifndef PX_STRING_H
#include <string.h>
#define PX_STRING_H
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXALLOCSYSTEM_H
#include "pxAllocSystem.h"
#endif

#ifndef PXALLOCDEBUG_H
#include "pxAllocDebug.h"
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
    int (*get)(struct pxFoo *pFoo);
#define PXFOO_get(pI) \
    ((*(pI)->pVt->get)(pI))

    void (*increment)(struct pxFoo *pFoo, int i);
#define PXFOO_increment(pI, i) \
    ((*(pI)->pVt->increment)(pI, i))

} pxFooVt;

typedef struct pxFoo
{
    const pxFooVt *const pVt;
} pxFoo;

static const char pxFooName[] = "pxFooName";


// this is how you build an object that implements that interface
typedef struct MyObject
{
    const pxFooVt *pFooVt;
    pxObjectStruct objectStruct;
    int i;
} MyObject;

static int MyObject_get(pxFoo *pFoo)
{
    MyObject *const pThis = PXINTERFACE_STRUCT(pFoo, MyObject, pFooVt);

    return pThis->i;
}

static void MyObject_increment(pxFoo *pFoo, int i)
{
    MyObject *const pThis = PXINTERFACE_STRUCT(pFoo, MyObject, pFooVt);

    pThis->i += i;
}

static const pxFooVt pxFoo_FooVt =
{
    {
        offsetof(MyObject, objectStruct.pObjectVt) - offsetof(MyObject, pFooVt),
        pxObject_getInterface,
    },
    MyObject_get,
    MyObject_increment,
};


static const pxObjectInterface pxFoo_interfaces[] =
{
    {pxFooName, offsetof(MyObject, objectStruct.pObjectVt) - offsetof(MyObject, pFooVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxFooObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxObject_destroy,
    pxObject_clone,
    sizeof(pxFoo_interfaces)/sizeof(pxFoo_interfaces[0]),
    pxFoo_interfaces,
    sizeof(MyObject), 
    offsetof(MyObject, objectStruct),
    0,
    NULL,
};

static void testpxObject()
{
    // initialize the object
    MyObject *const pM = (MyObject *)malloc(sizeof(MyObject));
    memset(pM, 0, sizeof(MyObject));
    pM->pFooVt = &pxFoo_FooVt;
    pxObjectStructInit(&pM->objectStruct, &pxFooObjectVt, NULL);
    pM->i = 0;

    // try calling a method
    pxFoo *const pFoo = (pxFoo *)&pM->pFooVt;
    PXFOO_increment(pFoo, 1);
    if (pM->i != 1)
        fprintf(stderr, "foo interface increment failed\n");
    if (PXFOO_get(pFoo) != 1)
        fprintf(stderr, "foo interface get failed\n");

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

static pxFoo *MyObjectCreate(pxAlloc *const pAlloc, pxInterface *const pOwner)
{
    MyObject *const pM =
        (MyObject *)PXALLOC_alloc(pAlloc, sizeof(MyObject), PXALLOC_F_DIRTY);
    pM->pFooVt = &pxFoo_FooVt;
    pxObjectStructInit(&pM->objectStruct, &pxFooObjectVt, pOwner);
    pM->i = 0;

    return (pxFoo *)&pM->pFooVt;
}

static void testpxObjectCloning()
{
    pxAlloc *const pAllocS = pxAllocSystemGet();
    pxAlloc *const pAllocD1 = pxAllocDebugCreate(pAllocS, NULL);

    pxFoo *pFoo1 = MyObjectCreate(pAllocD1, NULL);
    PXFOO_increment(pFoo1, 17);
    MyObject *const pThis1 = PXINTERFACE_STRUCT(pFoo1, MyObject, pFooVt);
    if (pThis1->i != 17)
        fprintf(stderr, "testpxObjectCloning: increment failed\n");

    // clone the object
    pxAlloc *const pAllocD2 = pxAllocDebugCreate(pAllocS, NULL);
    pxObject *pFoo1O = PXINTERFACE_getInterface(pFoo1, pxObject);
    pxObjectCloner cloner;
    pxObjectClonerInitSingle(&cloner, pAllocD2);
    pxFoo *const pFoo2 = (pxFoo *)PXOBJECT_clone(pFoo1O, pxFooName, &cloner);
    pxObjectClonerCleanup(&cloner);

    pxObject *pAllocO = PXINTERFACE_getInterface(pAllocD1, pxObject);
    PXOBJECT_destroy(pAllocO);
    if (pThis1->i == 17)
        fprintf(stderr, "testpxObjectCloning: allocator did not shred\n");

    if (PXFOO_get(pFoo2) != 17)
        fprintf(stderr, "testpxObjectCloning: clone value incorrect\n");
}

int main(void)
{
    testpxObject();

    pxAllocSystemInit();
    testpxObjectCloning();

    return 0;
}
