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

#ifndef PXHASHER_H
#include "pxHasher.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXHASH_H
#include "pxHash.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


const char pxHasherName[] = "pxHasher";

typedef struct
{
    pxHashValue hashValue;

    pxObjectStruct objectStruct;
} pxHasher_s;


static const pxObjectInterface pxHasher_interfaces[] =
{
    {pxHasherName, 0},
    {pxObjectName, 0},
};

static const pxObjectVt pxHasherObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxObject_destroy,
    pxObject_clone,
    pxObjectStruct_dupThis,

    sizeof(pxHasher_interfaces)/sizeof(pxHasher_interfaces[0]),
    pxHasher_interfaces,

    sizeof(pxHasher_s),
    offsetof(pxHasher_s, objectStruct),
    0,
    NULL,
};

pxHasher *pxHasherCreate(pxAlloc *const pAlloc, pxInterface *const pOwner)
{
    pxHasher_s *pThis =
        PXALLOC_alloc(pAlloc, sizeof(pxHasher_s), PXALLOC_F_DIRTY);
    pxObjectStructInit(&pThis->objectStruct, &pxHasherObjectVt, pOwner);
    pxHashInit(&pThis->hashValue);

    return (pxHasher *)&pThis->objectStruct.pObjectVt;
}

int PXHASHER_getHash(pxHasher *const pI)
{
    pxHasher_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHasher_s, objectStruct.pObjectVt);

    return pThis->hashValue;
}

void PXHASHER_hashBytes(pxHasher *const pI,
                        const void *const p, const size_t length)
{
    pxHasher_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHasher_s, objectStruct.pObjectVt);

    pxHashVoid(&pThis->hashValue, p, length);
}

void PXHASHER_hashString(pxHasher *const pI, const char *const pS)
{
    pxHasher_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxHasher_s, objectStruct.pObjectVt);

    pxHashString(&pThis->hashValue, pS);
}
