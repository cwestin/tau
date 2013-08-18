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

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


typedef struct
{
    // locals
    int i;

    pxLoomFrame loomFrame;
} Producer_frame;

static pxLoomState Producer_resume(pxLoomContinuation *pLC)
{
    Producer_frame *const pFrame =
        PXINTERFACE_STRUCT(pLC, Producer_frame, loomFrame.pLoomContinuationVt);

    PXLOOMFRAME_BEGIN(&pFrame->loomFrame)
    {
        ++pFrame->i;
    }
    PXLOOMFRAME_END(&pFrame->loomFrame)
}

static const pxLoomContinuationVt Producer_frameLoomContinuationVt =
{
    {
        offsetof(Producer_frame, loomFrame.objectStruct.pObjectVt) -
            offsetof(Producer_frame, loomFrame.pLoomContinuationVt),
        pxObject_getInterface,
    },
    Producer_resume,
};


static const pxObjectInterface Producer_frame_interfaces[] =
{
    {pxLoomContinuationName,
     offsetof(Producer_frame, loomFrame.objectStruct.pObjectVt) -
         offsetof(Producer_frame, loomFrame.pLoomContinuationVt)},
    {pxObjectName, 0},
};

static const pxObjectVt Producer_frameObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxObject_destroy,
    pxObject_cloneForbidden, // TODO
    sizeof(Producer_frame_interfaces)/sizeof(Producer_frame_interfaces[0]),
    Producer_frame_interfaces,
    sizeof(Producer_frame),
    offsetof(Producer_frame, loomFrame.objectStruct),
    0,
    NULL,
};

static pxLoomContinuation *ProducerCreate(pxAlloc *const pAlloc)
{
    Producer_frame *const pFrame =
        PXALLOC_alloc(pAlloc, sizeof(Producer_frame), 0);
    pFrame->i = 0;
    return pxLoomFrameInit(&pFrame->loomFrame, pAlloc,
                           &Producer_frameLoomContinuationVt,
                           &Producer_frameObjectVt);
}

typedef struct
{
    pxLoomFrame loomFrame;
} Consumer_frame;

static void testpxLoom()
{
    struct pxAlloc *const pAllocS = pxAllocSystemGet();
    struct pxAlloc *const pAllocD = pxAllocDebugCreate(pAllocS, NULL);
    pxLoom *const pLoom = pxLoomCreate(pAllocD);

    pxLoomContinuation *const pProducer = ProducerCreate(pAllocD);
    PXLOOM_createCell(pLoom, pProducer);
    PXLOOM_run(pLoom);

    Producer_frame *const pProducer_frame =
        PXINTERFACE_STRUCT(pProducer, Producer_frame,
                           loomFrame.pLoomContinuationVt);
    if (pProducer_frame->i != 1)
        fprintf(stderr, "producer did not run\n");
}

int main(void)
{
    pxAllocSystemInit();
    testpxLoom();
    return 0;
}
