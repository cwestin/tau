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

#ifndef PXLOOMONESHOT_H
#include "pxLoomOneShot.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXLOOM_H
#include "pxLoom.h"
#endif

#ifndef PXPOKE_H
#include "pxPoke.h"
#endif


typedef struct
{
    struct pxLoom_Cell *pThisCell;

    const pxPokeVt *pPokeVt;
    pxLoomFrame loomFrame;
} pxLoomOneShot_frame;

static void pxLoomOneShot_poke(pxPoke *const pPoke)
{
    pxLoomOneShot_frame *const pFrame =
        PXINTERFACE_STRUCT(pPoke, pxLoomOneShot_frame, pPokeVt);

    // schedule ourselves
    PXLOOMCELL_schedule(pFrame->pThisCell);
}

static const pxPokeVt pxLoomOneShot_framePokeVt =
{
    {
        offsetof(pxLoomOneShot_frame, loomFrame.objectStruct.pObjectVt) -
            offsetof(pxLoomOneShot_frame, pPokeVt),
        pxObject_getInterface,
    },
    pxLoomOneShot_poke,
};

static pxLoomState pxLoomOneShot_resume(
    pxLoomContinuation *const pLC, pxLoom *const pLoom)
{
    pxLoomOneShot_frame *const pFrame =
        PXINTERFACE_STRUCT(pLC, pxLoomOneShot_frame,
                           loomFrame.pLoomContinuationVt);

    PXLOOMFRAME_BEGIN(&pFrame->loomFrame)
    {
        // suspend ourselves
        pFrame->pThisCell = PXLOOM_getCell(pLoom);
        PXLOOMCELL_suspend(pFrame->pThisCell);// TODO needs to be a loom op
        // TODO create a PXLOOMFRAME_YIELD for this purpose
        // TODO need a case label
    }
    PXLOOMFRAME_END(&pFrame->loomFrame);
}

static const pxLoomContinuationVt pxLoomOneShot_frameLoomContinuationVt =
{
    {
        offsetof(pxLoomOneShot_frame, loomFrame.objectStruct.pObjectVt) -
            offsetof(pxLoomOneShot_frame, loomFrame.pLoomContinuationVt),
        pxObject_getInterface,
    },
    pxLoomOneShot_resume,
};

static const pxObjectInterface pxLoomOneShot_frame_interfaces[] =
{
    {pxLoomContinuationName,
     offsetof(pxLoomOneShot_frame, loomFrame.objectStruct.pObjectVt) -
         offsetof(pxLoomOneShot_frame, loomFrame.pLoomContinuationVt)},
    {pxPokeName,
     offsetof(pxLoomOneShot_frame, loomFrame.objectStruct.pObjectVt) -
         offsetof(pxLoomOneShot_frame, pPokeVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxLoomOneShot_frameObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxLoomFrame_destroy,
    pxObject_cloneForbidden, // TODO
    sizeof(pxLoomOneShot_frame_interfaces)/sizeof(pxLoomOneShot_frame_interfaces[0]),
    pxLoomOneShot_frame_interfaces,
    sizeof(pxLoomOneShot_frame),
    offsetof(pxLoomOneShot_frame, loomFrame.objectStruct),
    0,
    NULL,
};

pxPoke *pxLoomOneShotCreate(pxAlloc *const pAlloc)
{
    pxLoomOneShot_frame *const pFrame =
        PXALLOC_alloc(pAlloc, sizeof(pxLoomOneShot_frame), PXALLOC_F_DIRTY);

    pFrame->pPokeVt = &pxLoomOneShot_framePokeVt;
    pxLoomFrameInit(&pFrame->loomFrame, pAlloc,
                    &pxLoomOneShot_frameLoomContinuationVt,
                    &pxLoomOneShot_frameObjectVt);
    
    return (pxPoke *)&pFrame->pPokeVt;
}
