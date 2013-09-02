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

#ifndef PXSTRING_H
#include "pxString.h"
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

#ifndef PXFREE_H
#include "pxFree.h"
#endif

#ifndef PXHASHABLE_H
#include "pxHashable.h"
#endif

#ifndef PXHASHER_H
#include "pxHasher.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


const char pxStringName[] = "pxString";


typedef struct
{
    pxAlloc *pAlloc; // the allocator used

    const pxStringVt *pStringVt;
    const pxComparableVt *pComparableVt;
    const pxHashableVt *pHashableVt;
    pxObjectStruct objectStruct;

    char string[1]; // the string value
} pxString_s;


static bool pxString_isEmpty(pxString *pString)
{
    pxString_s *const pThis =
        PXINTERFACE_STRUCT(pString, pxString_s, pStringVt);

    return pThis->string[0] == '\0';
}

static const pxStringVt pxStringStringVt =
{
    {
        offsetof(pxString_s, objectStruct.pObjectVt) - offsetof(pxString_s, pStringVt),
        pxObject_getInterface,
    },
    pxString_isEmpty,
};


static int pxString_compare(
    pxComparable *const pComparable, pxInterface *const pO)
{
    // this always beats an empty string
    if (!pO)
        return 1;

    pxString *const pOS = PXINTERFACE_getInterface(pO, pxString);
    if (!pOS)
        pxExit("pxString_compare: can't compare pxString with non-pxString\n");

    pxString_s *const pThis =
        PXINTERFACE_STRUCT(pComparable, pxString_s, pComparableVt);
    pxString_s *const pOther =
        PXINTERFACE_STRUCT(pOS, pxString_s, pStringVt);

    return strcmp(pThis->string, pOther->string);
}

static const pxComparableVt pxStringComparableVt =
{
    {
        offsetof(pxString_s, objectStruct.pObjectVt) - offsetof(pxString_s, pComparableVt),
        pxObject_getInterface,
    },
    pxString_compare,
};


static void pxString_hash(pxHashable *const pHashable, pxHasher *const pHasher)
{
    pxString_s *const pThis =
        PXINTERFACE_STRUCT(pHashable, pxString_s, pStringVt);

    PXHASHER_hashString(pHasher, pThis->string);
}

static const pxHashableVt pxStringHashableVt =
{
    {
        offsetof(pxString_s, objectStruct.pObjectVt) - offsetof(pxString_s, pHashableVt),
        pxObject_getInterface,
    },
    pxString_hash,
};


static void pxString_destroy(pxObject *pObject)
{
    pxString_s *const pThis =
        PXINTERFACE_STRUCT(pObject, pxString_s, objectStruct.pObjectVt);

    // if the underlying allocator supports freeing, use it
    pxFree *const pFree = PXINTERFACE_getInterface(pThis->pAlloc, pxFree);
    pxObject_destroy(pObject);
    if (pFree)
        PXFREE_free(pFree, pThis);
}

static pxObjectStruct *pxString_dupThis(
    pxObjectStruct *const pO, pxAlloc *const pA)
{
    pxString_s *const pThis = PXINTERFACE_STRUCT(pO, pxString_s, objectStruct);

    const size_t length = strlen(pThis->string);
    pxString_s *const pNew =
        PXALLOC_alloc(pA, sizeof(pxString_s) + length, PXALLOC_F_DIRTY);
    memcpy(pNew, pThis, sizeof(pxString_s) + length);

    // also capture the allocator we used for the new instance
    pNew->pAlloc = pA;
    
    return &pNew->objectStruct;
}

static const pxObjectInterface pxString_interfaces[] =
{
    {pxStringName, offsetof(pxString_s, objectStruct.pObjectVt) - offsetof(pxString_s, pStringVt)},
    {pxComparableName, offsetof(pxString_s, objectStruct.pObjectVt) - offsetof(pxString_s, pComparableVt)},
    {pxHashableName, offsetof(pxString_s, objectStruct.pObjectVt) - offsetof(pxString_s, pHashableVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxStringObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxString_destroy,
    pxObject_clone,
    pxString_dupThis,

    sizeof(pxString_interfaces)/sizeof(pxString_interfaces[0]),
    pxString_interfaces,

    sizeof(pxString_s),
    offsetof(pxString_s, objectStruct),
    0,
    NULL,
};

pxString *pxStringCreate(pxAlloc *pAlloc, const char *pS)
{
    if (!pS)
        return NULL;

    pxString_s *const pThis =
        PXALLOC_alloc(pAlloc, sizeof(pxString_s) + strlen(pS), PXALLOC_F_DIRTY);

    pThis->pStringVt = &pxStringStringVt;
    pThis->pHashableVt = &pxStringHashableVt;
    pThis->pComparableVt = &pxStringComparableVt;
    pxObjectStructInit(&pThis->objectStruct, &pxStringObjectVt, NULL);

    pThis->pAlloc = pAlloc;
    strcpy(pThis->string, pS);

    return (pxString *)&pThis->pStringVt;
}
