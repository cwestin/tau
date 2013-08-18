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
    pxLoomSemaphore *pProducerSem;
    pxLoomSemaphore *pConsumerSem;
    int *pi;

    pxLoomFrame loomFrame;
} Producer_frame;

static pxLoomState Producer_resume(
    pxLoomContinuation *const pLC, pxLoom *const pLoom)
{
    Producer_frame *const pFrame =
        PXINTERFACE_STRUCT(pLC, Producer_frame, loomFrame.pLoomContinuationVt);

    PXLOOMFRAME_BEGIN(&pFrame->loomFrame)
    {
        ++*pFrame->pi;
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
    pxLoomFrame_destroy,
    pxObject_cloneForbidden, // TODO
    sizeof(Producer_frame_interfaces)/sizeof(Producer_frame_interfaces[0]),
    Producer_frame_interfaces,
    sizeof(Producer_frame),
    offsetof(Producer_frame, loomFrame.objectStruct),
    0,
    NULL,
};

static Producer_frame *ProducerCreate(
    pxAlloc *const pAlloc, int *pi,
    pxLoomSemaphore *pProducerSem, pxLoomSemaphore *pConsumerSem)
{
    Producer_frame *const pFrame =
        PXALLOC_alloc(pAlloc, sizeof(Producer_frame), 0);

    pxLoomFrameInit(&pFrame->loomFrame, pAlloc,
                    &Producer_frameLoomContinuationVt,
                    &Producer_frameObjectVt);

    pFrame->pProducerSem = pProducerSem;
    pFrame->pConsumerSem = pConsumerSem;
    pFrame->pi = pi;

    return pFrame;
}


typedef struct
{
    // locals
    pxLoomSemaphore *pProducerSem;
    pxLoomSemaphore *pConsumerSem;
    int *pi;

    pxLoomFrame loomFrame;
} Consumer_frame;

static pxLoomState Consumer_resume(
    pxLoomContinuation *const pLC, pxLoom *const pLoom)
{
    Consumer_frame *const pFrame =
        PXINTERFACE_STRUCT(pLC, Consumer_frame, loomFrame.pLoomContinuationVt);

    PXLOOMFRAME_BEGIN(&pFrame->loomFrame)
    {
        // create two semaphores
        pFrame->pProducerSem = pxLoomSemaphoreCreate(pLoom);
        pFrame->pConsumerSem = pxLoomSemaphoreCreate(pLoom);

        // create the producer, feeding it the semaphores and the shared int
        Producer_frame *const pProducer =
            ProducerCreate(pFrame->loomFrame.pLocalAlloc, pFrame->pi,
                           pFrame->pProducerSem, pFrame->pConsumerSem);

        // start up the producer
        PXLOOM_createCell(pLoom, &pProducer->loomFrame);

        // consume the producer's ints until we hit the last
        // TODO
    }
    PXLOOMFRAME_END(&pFrame->loomFrame)
}

static const pxLoomContinuationVt Consumer_frameLoomContinuationVt =
{
    {
        offsetof(Consumer_frame, loomFrame.objectStruct.pObjectVt) -
            offsetof(Consumer_frame, loomFrame.pLoomContinuationVt),
        pxObject_getInterface,
    },
    Consumer_resume,
};


static const pxObjectInterface Consumer_frame_interfaces[] =
{
    {pxLoomContinuationName,
     offsetof(Consumer_frame, loomFrame.objectStruct.pObjectVt) -
         offsetof(Consumer_frame, loomFrame.pLoomContinuationVt)},
    {pxObjectName, 0},
};

static const pxObjectVt Consumer_frameObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxLoomFrame_destroy,
    pxObject_cloneForbidden, // TODO
    sizeof(Consumer_frame_interfaces)/sizeof(Consumer_frame_interfaces[0]),
    Consumer_frame_interfaces,
    sizeof(Consumer_frame),
    offsetof(Consumer_frame, loomFrame.objectStruct),
    0,
    NULL,
};

static Consumer_frame *ConsumerCreate(pxAlloc *const pAlloc, int *pi)
{
    Consumer_frame *const pFrame =
        PXALLOC_alloc(pAlloc, sizeof(Consumer_frame), 0);

    pFrame->pi = pi;
    pxLoomFrameInit(&pFrame->loomFrame, pAlloc,
                    &Consumer_frameLoomContinuationVt,
                    &Consumer_frameObjectVt);

    return pFrame;
}


static void testpxLoom()
{
    struct pxAlloc *const pAllocS = pxAllocSystemGet();
    struct pxAlloc *const pAllocD = pxAllocDebugCreate(pAllocS, NULL);
    pxLoom *const pLoom = pxLoomCreate(pAllocD);

    int i = 0;
    Consumer_frame *const pConsumer = ConsumerCreate(pAllocD, &i);

    PXLOOM_createCell(pLoom, &pConsumer->loomFrame);
    PXLOOM_run(pLoom);

    if (i != 1)
        fprintf(stderr, "producer did not run\n");
}

int main(void)
{
    pxAllocSystemInit();
    testpxLoom();
    return 0;
}
