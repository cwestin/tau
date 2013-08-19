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


void pxLoomFrame_destroy(pxObject *const pI)
{
    pxLoomFrame *const pThis =
        PXINTERFACE_STRUCT(pI, pxLoomFrame, objectStruct.pObjectVt);

    pxFree *const pFree = PXINTERFACE_getInterface(pThis->pLocalAlloc, pxFree);

    void *const p = ((char *)pThis) - pI->pVt->objectOffset +
        offsetof(pxLoomFrame, objectStruct);

    pxObject_destroy(pI);
    PXFREE_free(pFree, p);
}

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


typedef struct pxLoom_s
{
    struct pxLoom_Cell *pCurrentCell; // only valid during calls to _resume

    pxDllHead readyCellList;
    pxDllHead waitingCellList;

    pxAlloc *pAlloc;

    const pxLoomVt *pLoomVt;
    pxObjectStruct objectStruct;
} pxLoom_s;


const char pxLoomSemaphoreName[] = "pxLoomSemaphore";

struct pxLoom_s;
typedef struct
{
    unsigned n;
    struct pxLoom_s *pLoom;

    pxDllLink waitingCellList;

    const pxLoomSemaphoreVt *pLoomSemaphoreVt;
    pxObjectStruct objectStruct;
} pxLoomSemaphore_Local;

static void pxLoomSemaphore_Local_put(pxLoomSemaphore *pI, unsigned n)
{
    pxLoomSemaphore_Local *const pThis =
        PXINTERFACE_STRUCT(pI, pxLoomSemaphore_Local, pLoomSemaphoreVt);

    // increment the number of available counts
    pThis->n += n;

    // move waiters from the waiting list to the ready list
    // TODO
}

static bool pxLoomSemaphore_Local_get(
    pxLoomSemaphore *const pI, pxLoom *const pLoom,
    pxLoomSemaphore_Waiter *const pWaiter, const unsigned n)
{
    pxLoomSemaphore_Local *const pThis =
        PXINTERFACE_STRUCT(pI, pxLoomSemaphore_Local, pLoomSemaphoreVt);

    // if we can satisfy the request, do so immediately
    if (pThis->n >= n)
    {
        pThis->n -= n;
        return true;
    }

    pxLoom_s *const pLoom_s = PXINTERFACE_STRUCT(pLoom, pxLoom_s, pLoomVt);

    pWaiter->n = n;
    pWaiter->pCell = pLoom_s->pCurrentCell;
    pxDllInit(&pWaiter->link);
    pxDllAddLast(&pThis->waitingCellList, &pWaiter->link);

    return false;
}

static const pxLoomSemaphoreVt pxLoomSemaphore_LocalLoomSemaphoreVt =
{
    {
        offsetof(pxLoomSemaphore_Local, objectStruct.pObjectVt) - offsetof(pxLoomSemaphore_Local, pLoomSemaphoreVt),
        pxObject_getInterface,
    },
    pxLoomSemaphore_Local_put,
    pxLoomSemaphore_Local_get,
};

static const pxObjectInterface pxLoomSemaphore_Local_interfaces[] =
{
    {pxLoomSemaphoreName, offsetof(pxLoomSemaphore_Local, objectStruct.pObjectVt) - offsetof(pxLoomSemaphore_Local, pLoomSemaphoreVt)},
    {pxObjectName, 0},
};

static void pxLoomSemaphore_Local_destroy(pxObject *pI)
{
/*
    pxLoomSempahore_local *const pThis =
        PXINTERFACE_STRUCT(pI, pxLoomSemaphore_Local, objectStruct.pObjectVt);
*/

    pxExit("pxLoomSemaphore_Local_destroy: unimplemented\n");
    pxObject_destroy(pI);
}

static const pxObjectVt pxLoomSemaphore_LocalObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxLoomSemaphore_Local_destroy,
    pxObject_cloneForbidden,
    sizeof(pxLoomSemaphore_Local_interfaces)/sizeof(pxLoomSemaphore_Local_interfaces[0]),
    pxLoomSemaphore_Local_interfaces,
    sizeof(pxLoomSemaphore_Local), 
    offsetof(pxLoomSemaphore_Local, objectStruct),
    0,
    NULL,
};

typedef struct pxLoom_Cell
{
    pxLoomFrame *pTopFrame;

    pxDllLink link;
} pxLoom_Cell;

pxLoomSemaphore *pxLoomSemaphoreCreate(pxLoom *pI, unsigned n)
{
    pxLoom_s *const pLoom = PXINTERFACE_STRUCT(pI, pxLoom_s, pLoomVt);

    pxLoomSemaphore_Local *const pSem =
        (pxLoomSemaphore_Local *)PXALLOC_alloc(
            pLoom->pAlloc, sizeof(pxLoomSemaphore_Local), PXALLOC_F_DIRTY);
    pSem->pLoomSemaphoreVt = &pxLoomSemaphore_LocalLoomSemaphoreVt;
    pxObjectStructInit(
        &pSem->objectStruct, &pxLoomSemaphore_LocalObjectVt, NULL);

    pSem->n = n;
    pSem->pLoom = pLoom;
    pxDllInit(&pSem->waitingCellList);

    return (pxLoomSemaphore *)&pSem->pLoomSemaphoreVt;
}

static void pxLoom_createCell(pxLoom *const pI, pxLoomFrame *const pFrame)
{
    pxLoom_s *const pThis = PXINTERFACE_STRUCT(pI, pxLoom_s, pLoomVt);

    pxLoom_Cell *pCell = PXALLOC_alloc(
        pThis->pAlloc, sizeof(pxLoom_Cell), PXALLOC_F_DIRTY);
    pCell->pTopFrame = pFrame;
    pxDllInit(&pCell->link);

    // add this to the list of things that can run
    pxDllAddLast(&pThis->readyCellList, &pCell->link);
}

static void pxLoom_run(pxLoom *const pI)
{
    pxLoom_s *const pThis = PXINTERFACE_STRUCT(pI, pxLoom_s, pLoomVt);

    pxDllLink *pLink;
    while((pLink = pxDllGetFirst(&pThis->readyCellList)))
    {
        pxLoom_Cell *const pCell = PXDLL_STRUCT(pLink, pxLoom_Cell, link);

        // execute until the cell yields
        pxLoomFrame *pFrame = pCell->pTopFrame;
        pThis->pCurrentCell = pCell;
        pxLoomState state =
            PXLOOMCONTINUATION_resume(
                (pxLoomContinuation *)&pFrame->pLoomContinuationVt, pI);
        switch(state)
        {
        case PXLOOMSTATE_RETURN:
        {
            // pop this frame off the cells's stack
            pCell->pTopFrame = pFrame->pPreviousFrame;

            // deallocate the popped frame
            PXOBJECT_destroy((pxObject *)&pFrame->objectStruct.pObjectVt);

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

        case PXLOOMSTATE_CALL:
            // nothing to do, we just need to run the new top frame
            // which will happen on the next pass of the loop
            break;

        case PXLOOMSTATE_WAIT:
            pxDllRemove(pLink);
            pxDllAddLast(&pThis->waitingCellList, pLink);
            break;

        default:
            pxExit("pxLoom_run: unknown loom state %d\n", state);
        }

        // TODO round-robin scheduling
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
    pxDllInit(&pThis->readyCellList);
    pxDllInit(&pThis->waitingCellList);
    pThis->pAlloc = pAlloc;
    pThis->pLoomVt = &pxLoomLoomVt;
    pxObjectStructInit(&pThis->objectStruct, &pxLoomObjectVt, NULL);

    return (pxLoom *)&pThis->pLoomVt;
}

void pxLoomCall(pxLoom *pI, pxLoomFrame *pFrame)
{
    pxLoom_s *const pThis = PXINTERFACE_STRUCT(pI, pxLoom_s, pLoomVt);

    pFrame->pPreviousFrame = pThis->pCurrentCell->pTopFrame;
    pThis->pCurrentCell->pTopFrame = pFrame;
}
