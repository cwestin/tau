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

    struct pxFoo *(*setFoo)(struct pxFoo *pFoo, struct pxFoo *pOtherFoo);
#define PXFOO_setFoo(pI, pOtherFoo) \
    ((*(pI)->pVt->setFoo)(pI, pOtherFoo))

    struct pxFoo *(*getFoo)(struct pxFoo *pFoo);
#define PXFOO_getFoo(pI) \
    ((*(pI)->pVt->getFoo)(pI))
} pxFooVt;

typedef struct pxFoo
{
    const pxFooVt *const pVt;
} pxFoo;

static const char pxFooName[] = "pxFoo";


// this is how you build an object that implements that interface
typedef struct MyObject
{
    int i;
    pxFoo *pOther;

    const pxFooVt *pFooVt;
    pxObjectStruct objectStruct;
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

static pxFoo *MyObject_setFoo(pxFoo *pFoo, pxFoo *pOtherFoo)
{
    MyObject *const pThis = PXINTERFACE_STRUCT(pFoo, MyObject, pFooVt);

    pxFoo *const pPreviousFoo = pThis->pOther;
    pThis->pOther = pOtherFoo;
    return pPreviousFoo;
}

static pxFoo *MyObject_getFoo(pxFoo *pFoo)
{
    MyObject *const pThis = PXINTERFACE_STRUCT(pFoo, MyObject, pFooVt);

    return pThis->pOther;
}

static const pxFooVt MyObjectFooVt =
{
    {
        offsetof(MyObject, objectStruct.pObjectVt) - offsetof(MyObject, pFooVt),
        pxObject_getInterface,
    },
    MyObject_get,
    MyObject_increment,
    MyObject_setFoo,
    MyObject_getFoo,
};


static const pxObjectInterface MyObject_interfaces[] =
{
    {pxFooName, offsetof(MyObject, objectStruct.pObjectVt) - offsetof(MyObject, pFooVt)},
    {pxObjectName, 0},
};

static const pxObjectMember MyObject_members[] =
{
    {0, offsetof(MyObject, pOther), pxFooName},
};

static const pxObjectVt MyObjectObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxObject_destroy,
    pxObject_clone,
    pxObjectStruct_dupThis,
    sizeof(MyObject_interfaces)/sizeof(MyObject_interfaces[0]),
    MyObject_interfaces,
    sizeof(MyObject), 
    offsetof(MyObject, objectStruct),
    sizeof(MyObject_members)/sizeof(MyObject_members[0]),
    MyObject_members,
};

static void testpxObject()
{
    // initialize the object
    MyObject *const pM = (MyObject *)malloc(sizeof(MyObject));
    memset(pM, 0, sizeof(MyObject));
    pM->pFooVt = &MyObjectFooVt;
    pxObjectStructInit(&pM->objectStruct, &MyObjectObjectVt, NULL);
    pM->i = 0;
    pM->pOther = NULL;

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
    pM->pFooVt = &MyObjectFooVt;
    pxObjectStructInit(&pM->objectStruct, &MyObjectObjectVt, pOwner);
    pM->i = 0;
    pM->pOther = NULL;

    return (pxFoo *)&pM->pFooVt;
}


struct pxBar;
typedef struct pxBarVt
{
    pxInterfaceVt interfaceVt;

    int (*get)(struct pxBar *pBar);
#define PXBAR_get(pI) \
    ((*(pI)->pVt->get)(pI))

    int (*set)(struct pxBar *pBar, int i);
#define PXBAR_set(pI, i) \
    ((*(pI)->pVt->set)(pI, i))
} pxBarVt;

typedef struct pxBar
{
    const pxBarVt *const pVt;
} pxBar;

static const char pxBarName[] = "pxBar";


typedef struct YourObject
{
    int i;

    const pxBarVt *pBarVt;
    pxObjectStruct objectStruct;
} YourObject;


static int YourObject_get(pxBar *pBar)
{
    YourObject *const pThis = PXINTERFACE_STRUCT(pBar, YourObject, pBarVt);

    return pThis->i;
}

static int YourObject_set(pxBar *pBar, int i)
{
    YourObject *const pThis = PXINTERFACE_STRUCT(pBar, YourObject, pBarVt);

    const int oldi = pThis->i;
    pThis->i = i;
    return oldi;
}

static const pxBarVt YourObjectBarVt =
{
    {
        offsetof(YourObject, objectStruct.pObjectVt) - offsetof(YourObject, pBarVt),
        pxObject_getInterface,
    },
    YourObject_get,
    YourObject_set,
};

static const pxObjectInterface YourObject_interfaces[] =
{
    {pxBarName, offsetof(YourObject, objectStruct.pObjectVt) - offsetof(YourObject, pBarVt)},
    {pxObjectName, 0},
};

static const pxObjectVt YourObjectObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxObject_destroy,
    pxObject_clone,
    pxObjectStruct_dupThis,
    sizeof(YourObject_interfaces)/sizeof(YourObject_interfaces[0]),
    YourObject_interfaces,
    sizeof(YourObject), 
    offsetof(YourObject, objectStruct),
    0,
    NULL,
};


static pxBar *YourObjectCreate(pxAlloc *const pAlloc, pxInterface *const pOwner)
{
    YourObject *const pY =
        (YourObject *)PXALLOC_alloc(pAlloc, sizeof(YourObject),
                                    PXALLOC_F_DIRTY);
    pY->pBarVt = &YourObjectBarVt;
    pxObjectStructInit(&pY->objectStruct, &YourObjectObjectVt, pOwner);
    pY->i = 0;

    return (pxBar *)&pY->pBarVt;
}

static void testpxObjectCloning()
{
    pxAlloc *const pAllocS = pxAllocSystemGet();
    pxAlloc *const pAllocD1 = pxAllocDebugCreate(pAllocS, NULL);

    pxFoo *const pFoo1 = MyObjectCreate(pAllocD1, NULL);
    PXFOO_increment(pFoo1, 17);
    MyObject *const pThis1 = PXINTERFACE_STRUCT(pFoo1, MyObject, pFooVt);
    if (pThis1->i != 17)
        fprintf(stderr, "testpxObjectCloning: increment failed\n");

    // clone the object into a new arena
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
        fprintf(stderr,
                "testpxObjectCloning: clone value incorrect (%d)\n", __LINE__);

    pxFoo *const pFoo3 = MyObjectCreate(pAllocD2, NULL);
    PXFOO_increment(pFoo3, 42);
    PXFOO_setFoo(pFoo3, pFoo2);
    PXFOO_setFoo(pFoo2, pFoo3);

    // clone the two mutually referential objects into another arena
    pxAlloc *const pAllocD3 = pxAllocDebugCreate(pAllocS, NULL);
    pxObjectClonerInitSingle(&cloner, pAllocD3);
    pxObject *const pFoo3O = PXINTERFACE_getInterface(pFoo3, pxObject);
    pxFoo *const pFoo4 = (pxFoo *)PXOBJECT_clone(pFoo3O, pxFooName, &cloner);
    pxObject *const pFoo2O = PXINTERFACE_getInterface(pFoo2, pxObject);
    pxFoo *const pFoo5 = (pxFoo *)PXOBJECT_clone(pFoo2O, pxFooName, &cloner);
    pxObjectClonerCleanup(&cloner);

    // check the state of the latest clones
    pAllocO = PXINTERFACE_getInterface(pAllocD2, pxObject);
    PXOBJECT_destroy(pAllocO);
    if (PXFOO_get(pFoo5) != 17)
        fprintf(stderr,
                "testpxObjectCloning: clone value incorrect (%d)\n", __LINE__);
    if (PXFOO_getFoo(pFoo5) != pFoo4)
        fprintf(stderr,
                "testpxObjectCloning: clone member incorrect (%d)\n", __LINE__);
    if (PXFOO_get(pFoo4) != 42)
        fprintf(stderr,
                "testpxObjectCloning: clone value incorrect (%d)\n", __LINE__);
    if (PXFOO_getFoo(pFoo4) != pFoo5)
        fprintf(stderr,
                "testpxObjectCloning: clone member incorrect (%d)\n", __LINE__);
        
    // test cloning with a mixin
    pxBar *const pBar4 = YourObjectCreate(pAllocD3, (pxInterface *)pFoo4);
    PXBAR_set(pBar4, 911);
    pxAlloc *const pAllocD4 = pxAllocDebugCreate(pAllocS, NULL);
    pxObjectClonerInitGraph(&cloner, pAllocD4);
    pxObject *const pBar4O = PXINTERFACE_getInterface(pBar4, pxObject);
    pxBar *const pBar6 = (pxBar *)PXOBJECT_clone(pBar4O, pxBarName, &cloner);
    pxObjectClonerCleanup(&cloner);

    pAllocO = PXINTERFACE_getInterface(pAllocD3, pxObject);
    PXOBJECT_destroy(pAllocO);

    // check the state of the clones
    if (PXBAR_get(pBar6) != 911)
        fprintf(stderr,
                "testpxObjectCloning: clone value incorrect (%d)\n", __LINE__);

    pxFoo *const pFoo6 = PXINTERFACE_getInterface(pBar6, pxFoo);
    if (!pFoo6)
        fprintf(stderr, "testpxObjectCloning: mixin not cloned\n");
    if (PXFOO_get(pFoo6) != 42)
        fprintf(stderr,
                "testpxObjectCloning: clone value incorrect (%d)\n", __LINE__);
    pxFoo *const pFoo7 = PXFOO_getFoo(pFoo6);
    if (!pFoo7)
        fprintf(stderr,
                "testpxObjectCloning: clone member incorrect (%d)\n", __LINE__);
    if (PXFOO_get(pFoo7) != 17)
        fprintf(stderr,
                "testpxObjectCloning: clone value incorrect (%d)\n", __LINE__);

    if (PXFOO_getFoo(pFoo7) != pFoo6)
        fprintf(stderr,
                "testpxObjectCloning: clone member incorrect (%d)\n", __LINE__);
}

int main(void)
{
    testpxObject();

    pxAllocSystemInit();
    testpxObjectCloning();

    return 0;
}
