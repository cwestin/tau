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

#ifndef PXALLOCLOCAL_H
#include "pxAllocLocal.h"
#endif

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif

#ifndef PX_STRINGS_H
#include <strings.h>
#define PX_STRINGS_H
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXEXIT_H
#include "pxExit.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


typedef struct
{
    char *p; // pointer to the next available space
    size_t avail; // the amount of space left

    const pxAllocVt *pAllocVt;
    pxObjectStruct objectStruct;

    pxAlignAll space[1]; // available space trails off the end
} pxAllocLocal_s;

static void *pxAllocLocal_alloc(pxAlloc *pI, size_t size, int flag)
{
    if (size <= 0)
        pxExit("pxAllocLocal_alloc: requested size is <= 0\n");

    pxAllocLocal_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxAllocLocal_s, pAllocVt);
    if (pThis->avail < size)
    {
        if (flag & PXALLOC_F_RETURN_OOM)
            return NULL;

        pxExit("pxAllocLocal_alloc: insufficient space available; "
               "avail = %lu, size = %lu\n", pThis->avail, size);
    }

    const size_t alignedSize = PXALIGN_SIZE(size);
    void *p = pThis->p;
    pThis->p += alignedSize;
    if (alignedSize >= pThis->avail)
        pThis->avail = 0;
    else
        pThis->avail -= alignedSize;

    if (!(flag & PXALLOC_F_DIRTY))
        bzero(p, size);

    return p;
}

static const pxAllocVt pxAllocLocalAllocVt =
{
    {
        offsetof(pxAllocLocal_s, objectStruct.pObjectVt) -
        offsetof(pxAllocLocal_s, pAllocVt),
        pxObject_getInterface,
    },
    pxAllocLocal_alloc,
};

static const pxObjectLookup pxAllocLocal_lookup[] =
{
    {pxAllocName, offsetof(pxAllocLocal_s, objectStruct.pObjectVt) - offsetof(pxAllocLocal_s, pAllocVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxAllocLocalObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    sizeof(pxAllocLocal_lookup)/sizeof(pxAllocLocal_lookup[0]),
    pxAllocLocal_lookup,
    pxObject_destroy,
};

pxAlloc *pxAllocLocalInit(pxAlignAll *pSpace, size_t size)
{
    if (size < sizeof(pxAllocLocal_s))
        pxExit("can't initialize local allocator\n");

    pxAllocLocal_s *const pThis = (pxAllocLocal_s *)pSpace;
    pThis->pAllocVt = &pxAllocLocalAllocVt;
    pxObjectStructInit(&pThis->objectStruct, &pxAllocLocalObjectVt, NULL);
    pThis->p = (char *)pThis->space;
    pThis->avail = size - offsetof(pxAllocLocal_s, space);

    return (pxAlloc *)&pThis->pAllocVt;
}
