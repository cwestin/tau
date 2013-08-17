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

#ifndef PXLOOM_H
#include "pxLoom.h"
#endif

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

#ifndef PXEXIT_H
#include "pxExit.h"
#endif


const char pxLoomClosureName[] = "pxLoomClosure";



const char pxLoomName[] = "pxLoom";

typedef struct
{
    pxAlloc *pAlloc;

    const pxLoomVt *pLoomVt;
    pxObjectStruct objectStruct;
} pxLoom_s;

static const pxLoomVt pxLoomLoomVt =
{
    {
        offsetof(pxLoom_s, objectStruct.pObjectVt) - offsetof(pxLoom_s, pLoomVt),
        pxObject_getInterface,
    },
};

static const pxObjectInterface pxLoom_interfaces[] =
{
    {pxLoomName, offsetof(pxLoom_s, objectStruct.pObjectVt) - offsetof(pxLoom_s, pLoomVt)},
    {pxObjectName, 0},
};

static void pxLoom_destroy(pxObject *pI)
{
/*
    pxLoom_s *const pThis =
        PXINTERFACE_STRUCT(pI, pxLoom_s, objectStruct.pObjectVt);
*/

    pxExit("pxLoom_destroy: unimplemented\n");
    pxObject_destroy(pI);
}

static const pxObjectVt pxLoomObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxLoom_destroy,
    pxObject_cloneForbidden,
    sizeof(pxLoom_interfaces)/sizeof(pxLoom_interfaces[0]),
    pxLoom_interfaces,
    sizeof(pxLoom_s), 
    offsetof(pxLoom_s, objectStruct),
    0,
    NULL,
};

pxLoom *pxLoomCreate(pxAlloc *pAlloc)
{
    pxLoom_s *const pThis =
        PXALLOC_alloc(pAlloc, sizeof(pxLoom_s), PXALLOC_F_DIRTY);
    pThis->pAlloc = pAlloc;
    pThis->pLoomVt = &pxLoomLoomVt;
    pxObjectStructInit(&pThis->objectStruct, &pxLoomObjectVt, NULL);

    return (pxLoom *)&pThis->pLoomVt;
}
