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

#ifndef PX_STDBOOL_H
#include <stdbool.h>
#define PX_STDBOOL_H
#endif

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

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

#ifndef PXMINDER_H
#include "pxMinder.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


typedef struct
{
    pxObjectStruct objectStruct;
    bool *pIsDestroyed;
} MyDestroyable;

static void MyDestroyable_destroy(pxObject *pI)
{
    MyDestroyable *pThis =
        PXINTERFACE_STRUCT(pI, MyDestroyable, objectStruct.pObjectVt);

    if (*pThis->pIsDestroyed)
        fprintf(stderr, "testpxMinder: destroyable destroyed a second time\n");
    *pThis->pIsDestroyed = true;
}

static const pxObjectInterface MyDestroyable_interfaces[] =
{
    {pxObjectName, 0},
};

static const pxObjectVt MyDestroyable_ObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    MyDestroyable_destroy,
    pxObject_cloneForbidden, // TODO
    sizeof(MyDestroyable_interfaces)/sizeof(MyDestroyable_interfaces[0]),
    MyDestroyable_interfaces,
    0, NULL,
};

static void MyDestroyableInit(MyDestroyable *pMD, bool *pBool)
{
    pxObjectStructInit(&pMD->objectStruct, &MyDestroyable_ObjectVt, NULL);
    pMD->pIsDestroyed = pBool;
    *pBool = false;
}


static void testpxMinder_basic()
{
    pxAlloc *const pAlloc = pxAllocSystemGet();
    pxMinder *pMinder;
    pxObject *pObject;

    // test creating and destroying a minder
    pMinder = pxMinderCreate(pAlloc, NULL);
    pObject = PXINTERFACE_getInterface(pMinder, pxObject);
    PXOBJECT_destroy(pObject);

    MyDestroyable d1, d2;
    bool b1, b2;

    // test adding and destroying a single object
    pMinder = pxMinderCreate(pAlloc, NULL);
    MyDestroyableInit(&d1, &b1);
    PXMINDER_register(pMinder, (pxObject *)&d1.objectStruct.pObjectVt);

    pObject = PXINTERFACE_getInterface(pMinder, pxObject);
    PXOBJECT_destroy(pObject);
    if (!b1)
        fprintf(stderr, "testpxMinder: did not destroy d1 (%d)\n", __LINE__);

    // test adding and destroying more than one object
    pMinder = pxMinderCreate(pAlloc, NULL);
    MyDestroyableInit(&d1, &b1);
    PXMINDER_register(pMinder, (pxObject *)&d1.objectStruct.pObjectVt);
    MyDestroyableInit(&d2, &b2);
    PXMINDER_register(pMinder, (pxObject *)&d2.objectStruct.pObjectVt);

    pObject = PXINTERFACE_getInterface(pMinder, pxObject);
    PXOBJECT_destroy(pObject);
    if (!b1)
        fprintf(stderr, "testpxMinder: did not destroy d1 (%d)\n", __LINE__);
    if (!b2)
        fprintf(stderr, "testpxMinder: did not destroy d2 (%d)\n", __LINE__);

    pxObject *pu1;

    // test adding multiple objects, but deregistering one before destroying
    pMinder = pxMinderCreate(pAlloc, NULL);
    MyDestroyableInit(&d1, &b1);
    pu1 = PXMINDER_register(pMinder, (pxObject *)&d1.objectStruct.pObjectVt);
    MyDestroyableInit(&d2, &b2);
    PXMINDER_register(pMinder, (pxObject *)&d2.objectStruct.pObjectVt);

    PXOBJECT_destroy(pu1); // deregisters d1

    pObject = PXINTERFACE_getInterface(pMinder, pxObject);
    PXOBJECT_destroy(pObject);
    if (b1)
        fprintf(stderr, "testpxMinder: incorrectly destroyed d1 (%d)\n",
                __LINE__);
    if (!b2)
        fprintf(stderr, "testpxMinder: did not destroy d2 (%d)\n", __LINE__);
}

static MyDestroyable *MyDestroyableCreate(pxAlloc *pArena, bool *pBool)
{
    pxMinder *pMinder = PXINTERFACE_getInterface(pArena, pxMinder);
    if (!pMinder)
        fprintf(stderr, "unable to find pxMinder\n");

    MyDestroyable *pM = PXALLOC_alloc(pArena, sizeof(MyDestroyable), 0);
    MyDestroyableInit(pM, pBool);
    PXMINDER_register(pMinder, (pxObject *)&pM->objectStruct.pObjectVt);

    return pM;
}

// this really tests mixins
static void testpxMinder_arena()
{
    pxAlloc *const pAllocS = pxAllocSystemGet();
    pxAlloc *const pAllocD = pxAllocDebugCreate(pAllocS, 0);
    pxAllocDebug *const pAllocDebug =
        PXINTERFACE_getInterface(pAllocD, pxAllocDebug);

    pxAlloc *pAlloc;
    pxMinder *pMinder;
    pxObject *pObject;

    pAlloc = pxAllocExtentCreate(pAllocD, 1024, NULL);
    pMinder = pxMinderCreate(pAlloc, (pxInterface *)pAlloc);
    pObject = PXINTERFACE_getInterface(pAlloc, pxObject);

    // test interface retrievals
    pxMinder *pM2 = PXINTERFACE_getInterface(pAlloc, pxMinder);
    if (pM2 != pMinder)
        fprintf(stderr, "failed to find aggregated interface\n");

    pxAlloc *pA2 = PXINTERFACE_getInterface(pMinder, pxAlloc);
    if (pA2 != pAlloc)
        fprintf(stderr, "failed to find owner interface\n");

    pxObject *pO3 = PXINTERFACE_getInterface(pMinder, pxObject);
    if (pO3 != pObject)
        fprintf(stderr, "failed to find owner object interface\n");

    PXOBJECT_destroy(pObject);
    if (!PXALLOCDEBUG_isEmpty(pAllocDebug))
        fprintf(stderr, "aggregate destruction failed (%d)\n", __LINE__);

    // test destruction of minded object residing in the arena
    pAlloc = pxAllocExtentCreate(pAllocD, 1024, NULL);
    pMinder = pxMinderCreate(pAlloc, (pxInterface *)pAlloc);
    pObject = PXINTERFACE_getInterface(pAlloc, pxObject);

    bool b1 = false;
    MyDestroyableCreate(pAlloc, &b1);

    PXOBJECT_destroy(pObject);
    if (!b1)
        fprintf(stderr, "destruction didn't destroy minded object\n");
    if (!PXALLOCDEBUG_isEmpty(pAllocDebug))
        fprintf(stderr, "aggregate destruction failed (%d)\n", __LINE__);
}

int main(void)
{
    pxAllocSystemInit();
    testpxMinder_basic();
    testpxMinder_arena();
    return 0;
}
