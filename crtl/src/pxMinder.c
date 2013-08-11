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

#ifndef PXMINDER_H
#include "pxMinder.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXDLL_H
#include "pxDll.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


const char pxMinderName[] = "pxMinder";

typedef struct
{
    const pxMinderVt *pMinderVt;
    pxObjectStruct objectStruct;

    pxAlloc *pAlloc; // the arena to use to allocate registered items
    pxDllHead list; // the list of items to mind
} pxMinder_s;


typedef struct
{
    pxObjectStruct objectStruct;
    pxObject *pManaged;
    pxDllLink link;
} pxMinder_item;

static void pxMinder_item_destroy(pxObject *pI)
{
    // destroying a minder object is the way to deregister a managed object
    pxMinder_item *const pMO =
        PXINTERFACE_STRUCT(pI, pxMinder_item, objectStruct.pObjectVt);
    pxDllRemove(&pMO->link);
    pMO->pManaged = NULL;

    // forward to the base class
    pxObject_destroy(pI);
}

static const pxObjectInterface pxMinder_item_interfaces[] =
{
    {pxObjectName, 0},
};

static const pxObjectVt pxMinder_itemObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxMinder_item_destroy,
    pxObject_cloneForbidden, // TODO
    sizeof(pxMinder_item_interfaces)/sizeof(pxMinder_item_interfaces[0]),
    pxMinder_item_interfaces,
    0, NULL,
};

static pxObject *pxMinder_register(pxMinder *pI, pxObject *pManaged)
{
    pxMinder_s *const pThis = PXINTERFACE_STRUCT(pI, pxMinder_s, pMinderVt);

    // allocate a place to hang on to this
    pxMinder_item *const pMO = (pxMinder_item *)
        PXALLOC_alloc(pThis->pAlloc, sizeof(*pMO), PXALLOC_F_DIRTY);

    // initialize
    pxObjectStructInit(&pMO->objectStruct, &pxMinder_itemObjectVt, NULL);
    pMO->pManaged = pManaged;
    pxDllAddAfter(&pMO->link, &pThis->list);

    return (pxObject *)&pMO->objectStruct.pObjectVt;
}

static void pxMinder_destroy(pxObject *pI)
{
    pxMinder_s *const pThis = PXINTERFACE_STRUCT(pI, pxMinder_s, objectStruct.pObjectVt);

    // run through the list from back to front
    pxDllLink *pLink;
    while((pLink = pxDllGetLast(&pThis->list)))
    {
        pxMinder_item *const pMO = PXDLL_STRUCT(pLink, pxMinder_item, link);

        // remove it from the list
        pxDllRemove(&pMO->link);

        // destroy the managed object
        PXOBJECT_destroy(pMO->pManaged);
        pMO->pManaged = NULL;
    }

    // forward call to base class
    pxObject_destroy(pI);
}

static const pxMinderVt pxMinder_MinderVt =
{
    {
        offsetof(pxMinder_s, objectStruct.pObjectVt) - offsetof(pxMinder_s, pMinderVt),
        pxObject_getInterface,
    },
    pxMinder_register,
};

static const pxObjectInterface pxMinder_interfaces[] =
{
    {pxMinderName, offsetof(pxMinder_s, objectStruct.pObjectVt) - offsetof(pxMinder_s, pMinderVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxMinder_ObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxMinder_destroy,
    pxObject_cloneForbidden, // TODO
    sizeof(pxMinder_interfaces)/sizeof(pxMinder_interfaces[0]),
    pxMinder_interfaces,
    0, NULL
};


pxMinder *pxMinderCreate(pxAlloc *pAlloc, pxInterface *pOwner)
{
    pxMinder_s *const pMinder =
        PXALLOC_alloc(pAlloc, sizeof(*pMinder), PXALLOC_F_DIRTY);

    pMinder->pMinderVt = &pxMinder_MinderVt;
    pxObjectStructInit(&pMinder->objectStruct, &pxMinder_ObjectVt, pOwner);

    pMinder->pAlloc = pAlloc;
    pxDllInit(&pMinder->list);

    return (pxMinder *)&pMinder->pMinderVt;
}
