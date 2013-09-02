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

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXALLOCDEBUG_H
#include "pxAllocDebug.h"
#endif

#ifndef PXALLOCSYSTEM_H
#include "pxAllocSystem.h"
#endif

#ifndef PXLOOM_H
#include "pxLoom.h"
#endif

#ifndef PXLOOMONESHOT_H
#include "pxLoomOneShot.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif

#ifndef PXPOKE_H
#include "pxPoke.h"
#endif


/*
  Stack frame elements for the Sender and Receiver. This is meant to model
  the currently envisioned innards of a channel.
 */

typedef struct
{
    pxPoke *pPoke;
} Receiver_args;

typedef struct
{
    // locals
    Receiver_args args;

    pxLoomFrame loomFrame;
} Receiver_frame;

static pxLoomState Receiver_resume(
    pxLoomContinuation *const pLC, pxLoom *const pLoom)
{
    Receiver_frame *const pFrame =
        PXINTERFACE_STRUCT(pLC, Receiver_frame, loomFrame.pLoomContinuationVt);

    PXLOOMFRAME_BEGIN(&pFrame->loomFrame)
    {
        // poke the Sender so that it will resume
        PXPOKE_poke(pFrame->args.pPoke);
    }
    PXLOOMFRAME_END(&pFrame->loomFrame)
}

static const pxLoomContinuationVt Receiver_frameLoomContinuationVt =
{
    {
        offsetof(Receiver_frame, loomFrame.objectStruct.pObjectVt) -
            offsetof(Receiver_frame, loomFrame.pLoomContinuationVt),
        pxObject_getInterface,
    },
    Receiver_resume,
};


static const pxObjectInterface Receiver_frame_interfaces[] =
{
    {pxLoomContinuationName,
     offsetof(Receiver_frame, loomFrame.objectStruct.pObjectVt) -
         offsetof(Receiver_frame, loomFrame.pLoomContinuationVt)},
    {pxObjectName, 0},
};

static const pxObjectVt Receiver_frameObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxLoomFrame_destroy,
    pxObject_cloneForbidden, // TODO
    pxObjectStruct_dupThis,
    sizeof(Receiver_frame_interfaces)/sizeof(Receiver_frame_interfaces[0]),
    Receiver_frame_interfaces,
    sizeof(Receiver_frame),
    offsetof(Receiver_frame, loomFrame.objectStruct),
    0,
    NULL,
};

static Receiver_frame *ReceiverCreate(
    pxAlloc *const pAlloc, pxPoke *const pPoke)
{
    Receiver_frame *const pFrame =
        PXALLOC_alloc(pAlloc, sizeof(Receiver_frame), 0);

    pFrame->args.pPoke = pPoke;
    pxLoomFrameInit(&pFrame->loomFrame, pAlloc,
                    &Receiver_frameLoomContinuationVt,
                    &Receiver_frameObjectVt);

    return pFrame;
}


typedef struct
{
    pxLoomFrame loomFrame;
} Sender_frame;

static pxLoomState Sender_resume(
    pxLoomContinuation *const pLC, pxLoom *const pLoom)
{
    Sender_frame *const pFrame =
        PXINTERFACE_STRUCT(pLC, Sender_frame, loomFrame.pLoomContinuationVt);

    PXLOOMFRAME_BEGIN(&pFrame->loomFrame)
    {
        // create the Receiver, feeding it the OneShot
        {
            pxPoke *const pPoke =
                pxLoomOneShotCreate(pFrame->loomFrame.pLocalAlloc);
            Receiver_frame *const pReceiver =
                ReceiverCreate(pFrame->loomFrame.pLocalAlloc, pPoke);

            // start up the Receiver
            PXLOOM_createCell(pLoom, &pReceiver->loomFrame);

            // wait for the Receiver
            PXLOOMONESHOT_WAIT(&pFrame->loomFrame, pLoom, pPoke);
        }
    }
    PXLOOMFRAME_END(&pFrame->loomFrame)
}

static const pxLoomContinuationVt Sender_frameLoomContinuationVt =
{
    {
        offsetof(Sender_frame, loomFrame.objectStruct.pObjectVt) -
            offsetof(Sender_frame, loomFrame.pLoomContinuationVt),
        pxObject_getInterface,
    },
    Sender_resume,
};


static const pxObjectInterface Sender_frame_interfaces[] =
{
    {pxLoomContinuationName,
     offsetof(Sender_frame, loomFrame.objectStruct.pObjectVt) -
         offsetof(Sender_frame, loomFrame.pLoomContinuationVt)},
    {pxObjectName, 0},
};

static const pxObjectVt Sender_frameObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxLoomFrame_destroy,
    pxObject_cloneForbidden, // TODO
    pxObjectStruct_dupThis,
    sizeof(Sender_frame_interfaces)/sizeof(Sender_frame_interfaces[0]),
    Sender_frame_interfaces,
    sizeof(Sender_frame),
    offsetof(Sender_frame, loomFrame.objectStruct),
    0,
    NULL,
};

static Sender_frame *SenderCreate(pxAlloc *const pAlloc)
{
    Sender_frame *const pFrame =
        PXALLOC_alloc(pAlloc, sizeof(Sender_frame), 0);

    pxLoomFrameInit(&pFrame->loomFrame, pAlloc,
                    &Sender_frameLoomContinuationVt,
                    &Sender_frameObjectVt);

    return pFrame;
}


static void testpxLoomOneShot()
{
    struct pxAlloc *const pAllocS = pxAllocSystemGet();
    struct pxAlloc *const pAllocD = pxAllocDebugCreate(pAllocS, NULL);
    pxLoom *const pLoom = pxLoomCreate(pAllocD);

    Sender_frame *const pSender = SenderCreate(pAllocD);

    PXLOOM_createCell(pLoom, &pSender->loomFrame);
    const unsigned remainingCells = PXLOOM_run(pLoom);

    if (remainingCells)
        fprintf(stderr, "testpxLoomOneShot: there are unfinished cells\n");

    // free the loom and check the debug allocator
    pxObject *const pObject = PXINTERFACE_getInterface(pLoom, pxObject);
    PXOBJECT_destroy(pObject);
    pxAllocDebug *const pAllocDebug =
        PXINTERFACE_getInterface(pAllocD, pxAllocDebug);
    const unsigned remainingPieces = PXALLOCDEBUG_countPieces(pAllocDebug);
    if (remainingPieces)
        fprintf(stderr, "there is still memory in use (%u pieces)\n",
                remainingPieces);
}

int main(void)
{
    pxAllocSystemInit();
    testpxLoomOneShot();
    return 0;
}
