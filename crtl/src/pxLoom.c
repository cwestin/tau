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

#ifndef PXDLL_H
#include "pxDll.h"
#endif

#ifndef PXEXIT_H
#include "pxExit.h"
#endif

#ifndef PXFREE_H
#include "pxFree.h"
#endif


const char pxLoomContinuationName[] = "pxLoomContinuation";


pxLoomContinuation *pxLoomFrameInit(
    pxLoomFrame *const pLoomFrame, pxAlloc *const pAlloc,
    const pxLoomContinuationVt *const pLoomContinuationVt,
    const pxObjectVt *const pLoomObjectVt)
{
    pLoomFrame->lineNumber = 0;
    pLoomFrame->pLocalAlloc = pAlloc;
    pLoomFrame->pPreviousFrame = NULL;

    pLoomFrame->pLoomContinuationVt = pLoomContinuationVt;
    pxObjectStructInit(&pLoomFrame->objectStruct, pLoomObjectVt, NULL);

    return (pxLoomContinuation *)&pLoomFrame->pLoomContinuationVt;
}

const char pxLoomSemaphoreName[] = "pxLoomSemaphore";

typedef struct
{
    unsigned n;
    pxLoom *pLoom;

    const pxLoomSemaphoreVt *pLoomSemaphoreVt;
    pxObjectStruct objectStruct;
} pxLoomSemaphore_local;

static void pxLoomSemaphore_local_put(pxLoomSemaphore *pI, unsigned n)
{
    pxLoomSemaphore_local *const pThis =
        PXINTERFACE_STRUCT(pI, pxLoomSemaphore_local, pLoomSemaphoreVt);

    pThis->n += n;
    // TODO
}

static void pxLoomSemaphore_local_get(pxLoomSemaphore *pI, unsigned n)
{
    pxLoomSemaphore_local *const pThis =
        PXINTERFACE_STRUCT(pI, pxLoomSemaphore_local, pLoomSemaphoreVt);

    // TODO
    pThis->n -= n;
}

static const pxLoomSemaphoreVt pxLoomSemaphore_localLoomSemaphoreVt =
{
    {
        offsetof(pxLoomSemaphore_local, objectStruct.pObjectVt) - offsetof(pxLoomSemaphore_local, pLoomSemaphoreVt),
        pxObject_getInterface,
    },
    pxLoomSemaphore_local_put,
    pxLoomSemaphore_local_get,
};

static const pxObjectInterface pxLoomSemaphore_local_interfaces[] =
{
    {pxLoomSemaphoreName, offsetof(pxLoomSemaphore_local, objectStruct.pObjectVt) - offsetof(pxLoomSemaphore_local, pLoomSemaphoreVt)},
    {pxObjectName, 0},
};

static void pxLoomSemaphore_local_destroy(pxObject *pI)
{
/*
    pxLoomSempahore_local *const pThis =
        PXINTERFACE_STRUCT(pI, pxLoomSemaphore_local, objectStruct.pObjectVt);
*/

    pxExit("pxLoomSemaphore_local_destroy: unimplemented\n");
    pxObject_destroy(pI);
}

static const pxObjectVt pxLoomSemaphore_localObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxLoomSemaphore_local_destroy,
    pxObject_cloneForbidden,
    sizeof(pxLoomSemaphore_local_interfaces)/sizeof(pxLoomSemaphore_local_interfaces[0]),
    pxLoomSemaphore_local_interfaces,
    sizeof(pxLoomSemaphore_local), 
    offsetof(pxLoomSemaphore_local, objectStruct),
    0,
    NULL,
};


typedef struct
{
    pxLoomFrame *pTopFrame;

    pxDllLink link;
} pxLoom_Cell;

typedef struct
{
    pxDllHead cellList;

    pxAlloc *pAlloc;

    const pxLoomVt *pLoomVt;
    pxObjectStruct objectStruct;
} pxLoom_s;

static void pxLoom_createCell(pxLoom *const pI, pxLoomFrame *const pFrame)
{
    pxLoom_s *const pThis = PXINTERFACE_STRUCT(pI, pxLoom_s, pLoomVt);

    pxLoom_Cell *pCell = PXALLOC_alloc(
        pThis->pAlloc, sizeof(pxLoom_Cell), PXALLOC_F_DIRTY);
    pCell->pTopFrame = pFrame;
    pxDllInit(&pCell->link);

    // add this to the list of things that can run
    pxDllAddLast(&pThis->cellList, &pCell->link);
}

static void pxLoom_run(pxLoom *const pI)
{
    pxLoom_s *const pThis = PXINTERFACE_STRUCT(pI, pxLoom_s, pLoomVt);

    pxDllLink *pLink;
    while((pLink = pxDllGetFirst(&pThis->cellList)))
    {
        pxLoom_Cell *const pCell = PXDLL_STRUCT(pLink, pxLoom_Cell, link);

        // execute until the cell yields
        pxLoomFrame *pFrame = pCell->pTopFrame;
        pxLoomState state =
            PXLOOMCONTINUATION_resume(
                (pxLoomContinuation *)&pFrame->pLoomContinuationVt);
        switch(state)
        {
        case PXLOOMSTATE_RETURN:
        {
            // pop this frame off the cells's stack
            pCell->pTopFrame = pFrame->pPreviousFrame;

            // deallocate the popped frame
/*
            pxFree *const pFreeFrame =
                PXINTERFACE_getInterface(pFrame->pLocalAlloc, pxFree);
            // TODO
*/

            // if there's nothing more to do, the cell dies
            if (!pCell->pTopFrame)
            {
                pxDllRemove(pLink);

                pxFree *const pFree =
                    PXINTERFACE_getInterface(pThis->pAlloc, pxFree);
                PXFREE_free(pFree, pCell);
            }

            break;
        }

        default:
            pxExit("pxLoom_run: unknown loom state %d\n", state);
        }
    }
}


const char pxLoomName[] = "pxLoom";

static const pxLoomVt pxLoomLoomVt =
{
    {
        offsetof(pxLoom_s, objectStruct.pObjectVt) - offsetof(pxLoom_s, pLoomVt),
        pxObject_getInterface,
    },
    pxLoom_createCell,
    pxLoom_run,
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
    pxDllInit(&pThis->cellList);
    pThis->pAlloc = pAlloc;
    pThis->pLoomVt = &pxLoomLoomVt;
    pxObjectStructInit(&pThis->objectStruct, &pxLoomObjectVt, NULL);

    return (pxLoom *)&pThis->pLoomVt;
}
