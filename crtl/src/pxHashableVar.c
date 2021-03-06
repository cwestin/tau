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

#ifndef PXHASHABLEVAR_H
#include "pxHashableVar.h"
#endif

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

#ifndef PX_STRING_H
#include <string.h>
#define PX_STRING_H
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXCOMPARABLE_H
#include "pxComparable.h"
#endif

#ifndef PXEXIT_H
#include "pxExit.h"
#endif

#ifndef PXHASHABLE_H
#include "pxHashable.h"
#endif

#ifndef PXHASHER_H
#include "pxHasher.h"
#endif


static void pxHashableVar_hash(pxHashable *pI, pxHasher *pHasher)
{
    const pxHashableVar_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHashableVar_s, pHashableVt);

    PXHASHER_hashBytes(pHasher, pThis->p, pThis->size);
}

static const pxHashableVt pxHashableVarHashableVt =
{
    {offsetof(pxHashableVar_s, objectStruct.pObjectVt) -
     offsetof(pxHashableVar_s, pHashableVt),
     pxObject_getInterface,
    },
    pxHashableVar_hash,
};

static int pxHashableVar_compare(pxComparable *pI, pxInterface *pOther)
{
    const pxHashableVar_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHashableVar_s, pComparableVt);

    // the two objects have to be of the same class
    pxObject *const pObject =
        PXINTERFACE_getInterface(pOther, pxObject);
    if (pObject->pVt != pThis->objectStruct.pObjectVt)
        pxExit("pxHashableVar_compare: objects of different class\n");

    // same class: they must have the same length or be variable
    const pxHashableVar_s *const pOtherThis =
        PXINTERFACE_STRUCT(pObject, pxHashableVar_s, objectStruct.pObjectVt);
    // TODO allow variable size
    if ((pThis->size != pOtherThis->size))
        pxExit("pxHashableVar_compare: objects of different class\n");
    
    size_t size;
    int sizecmp;
    if (pThis->size < pOtherThis->size)
    {
        size = pThis->size;
        sizecmp = -1;
    }
    else
    {
        size = pOtherThis->size;
        sizecmp = (pThis->size == pOtherThis->size) ? 0 : 1;
    }

    const int cmp = memcmp(pThis->p, pOtherThis->p, size);
    if (cmp == 0)
        return sizecmp;
    return cmp;
}

static const pxComparableVt pxHashableVarComparableVt =
{
    {offsetof(pxHashableVar_s, objectStruct.pObjectVt) -
     offsetof(pxHashableVar_s, pComparableVt),
     pxObject_getInterface,
    },
    pxHashableVar_compare,
};


static const pxObjectInterface pxHashableVar_interfaces[] =
{
    {pxComparableName,
     offsetof(pxHashableVar_s, objectStruct.pObjectVt) -
     offsetof(pxHashableVar_s, pComparableVt)},
    {pxHashableName,
     offsetof(pxHashableVar_s, objectStruct.pObjectVt) -
     offsetof(pxHashableVar_s, pHashableVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxHashableVarObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxObject_destroy,
    pxObject_cloneForbidden,
    pxObjectStruct_dupThis,

    sizeof(pxHashableVar_interfaces)/sizeof(pxHashableVar_interfaces[0]),
    pxHashableVar_interfaces,

    sizeof(pxHashableVar_s),
    offsetof(pxHashableVar_s, objectStruct),
    0,
    NULL,
};

pxHashable *pxHashableVarInit(
    pxHashableVar_s *const pThis, const void *const p, const size_t size,
    pxInterface *pOwner)
{
    pThis->pComparableVt = &pxHashableVarComparableVt;
    pThis->pHashableVt = &pxHashableVarHashableVt;
    pxObjectStructInit(&pThis->objectStruct, &pxHashableVarObjectVt, pOwner);

    pThis->p = p;
    pThis->size = size;

    return (pxHashable *)&pThis->pHashableVt;
}

pxHashable *pxHashableVarCreate(
    pxAlloc *pAlloc, const void *p, const size_t size, pxInterface *pOwner)
{
    pxHashableVar_s *const pThis =
        PXALLOC_alloc(pAlloc, sizeof(pxHashableVar_s), PXALLOC_F_DIRTY);
    return pxHashableVarInit(pThis, p, size, pOwner);
}
