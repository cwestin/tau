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

#ifndef PXALLOCSYSTEM_H
#include "pxAllocSystem.h"
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

#ifndef PXFREE_H
#include "pxFree.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


typedef struct
{
    const pxAllocVt *pAllocVt;
    const pxFreeVt *pFreeVt;
    pxObjectStruct objectStruct;
} pxAllocSystem_s;

static void *pxAllocSystem_alloc(pxAlloc *pAlloc, size_t size, int flag)
{
    void *p = malloc(size);
    if (p == NULL)
    {
        if (flag & PXALLOC_F_RETURN_OOM)
            return NULL;

        pxExit("pxAllocSystem_alloc: OOM\n");
    }

    if (!(flag & PXALLOC_F_DIRTY))
        bzero(p, size);

    return p;
}

static void pxAllocSystem_free(pxFree *pFree, void *p)
{
    if (p == NULL)
    {
        fprintf(stderr, "pxAllocSystem_free: Attempt to free NULL\n");
        exit(-1);
    }

    free(p);
}

static const pxAllocVt pxAllocSystemAllocVt =
{
    {
        offsetof(pxAllocSystem_s, objectStruct.pObjectVt) - offsetof(pxAllocSystem_s, pAllocVt),
        pxObject_getInterface,
    },
    pxAllocSystem_alloc,
};

static const pxFreeVt pxAllocSystemFreeVt =
{
    {
        offsetof(pxAllocSystem_s, objectStruct.pObjectVt) - offsetof(pxAllocSystem_s, pFreeVt),
        pxObject_getInterface,
    },
    pxAllocSystem_free,
};

static const pxObjectInterface pxAllocSystem_interfaces[] =
{
    {pxAllocName, offsetof(pxAllocSystem_s, objectStruct.pObjectVt) - offsetof(pxAllocSystem_s, pAllocVt)},
    {pxFreeName, offsetof(pxAllocSystem_s, objectStruct.pObjectVt) - offsetof(pxAllocSystem_s, pFreeVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxAllocSystemObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxObject_destroy,
    pxObject_cloneForbidden,
    pxObjectStruct_dupThis,

    sizeof(pxAllocSystem_interfaces)/sizeof(pxAllocSystem_interfaces[0]),
    pxAllocSystem_interfaces,

    sizeof(pxAllocSystem_s),
    offsetof(pxAllocSystem_s, objectStruct),
    0,
    NULL,
};

static pxAllocSystem_s *pxAllocSystemCreate()
{
    pxAllocSystem_s *const ppxAllocSystem_i =
        (pxAllocSystem_s *)malloc(sizeof(pxAllocSystem_s));
    ppxAllocSystem_i->pAllocVt = &pxAllocSystemAllocVt;
    ppxAllocSystem_i->pFreeVt = &pxAllocSystemFreeVt;
    pxObjectStructInit(
        &ppxAllocSystem_i->objectStruct, &pxAllocSystemObjectVt, NULL);
    return ppxAllocSystem_i;
}

static pxAllocSystem_s *pAlloc;

void pxAllocSystemInit()
{
    pAlloc = pxAllocSystemCreate();
}

pxAlloc *pxAllocSystemGet()
{
    return (pxAlloc *)&pAlloc->pAllocVt;
}
