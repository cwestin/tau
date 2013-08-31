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
/*
  Unit test for the loom.

  Tests the loom by creating a producer-consumer pair. The pair share a
  state variable that is used by the producer to send Fibonacci numbers to
  the consumer. The state variable is protected by a loom semaphore. The use
  of Fibonacci numbers provided a contrived way to test function calls
  for loom-executable functions (i.e., continuation stack frames).

  The argument passing here is purely experimental and does not reflect the
  style that may ultimately be used for tau-generated code. The key elements
  are loom functionality such as the cross-loom-thread synchronization.
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


/*
  Classic Fibonacci implementation used to test the output of the loom-enabled
  version.
 */
static unsigned fibo(unsigned n)
{
    if (n <= 1)
        return 1;

    return fibo(n - 1) + fibo(n - 2);
}


/*
  Stack frame elements for the loom-executable Fibonacci function, including
  separately defined argument and return value structures.

  The style here is experimental; some of these techniques may be used in
  generated code. The idea is for a structure full of arguments to be set
  by the caller, and for the caller to provide a place to put the results.
  If a loom function calls more than one other loom function, it includes a
  local union that can hold the results of all those functions; only one of
  those will be used at a time, in much the same way as disjoint C blocks
  use a union of space in the containing function's stack frame.

  For all the examples here, the stack frame structure has a name that is
  suffixed with _frame. A factory (<FunctionName>Create(...)) provides a means
  to create and initialize the stack frome before executing the loom-enabled
  call.
 */
typedef struct
{
    unsigned u;
} Fibo_args;

typedef struct
{
    unsigned *pu;
} Fibo_results;

typedef struct
{
    // args
    Fibo_args args;
    Fibo_results *pResults;

    // locals
    unsigned f1;
    unsigned f2;

    // called function results
    union
    {
        Fibo_results Fibo_results_r;
    } results;

    pxLoomFrame loomFrame;
} Fibo_frame;

static Fibo_frame *FiboCreate(pxAlloc *pAlloc, Fibo_results *pResults);

static pxLoomState Fibo_resume(
    pxLoomContinuation *const pLC, pxLoom *const pLoom)
{
    Fibo_frame *const pFrame =
        PXINTERFACE_STRUCT(pLC, Fibo_frame, loomFrame.pLoomContinuationVt);

    PXLOOMFRAME_BEGIN(&pFrame->loomFrame)
    {
        if (pFrame->args.u <= 1)
        {
            *pFrame->pResults->pu = 1;
            PXLOOMFRAME_RETURN(&pFrame->loomFrame);
        }

        pFrame->results.Fibo_results_r.pu = &pFrame->f1;
        {
            Fibo_frame *pFiboFrame =
                FiboCreate(pFrame->loomFrame.pLocalAlloc,
                           &pFrame->results.Fibo_results_r);
            pFiboFrame->args.u = pFrame->args.u - 1;
            PXLOOMFRAME_CALL(&pFrame->loomFrame, pLoom, &pFiboFrame->loomFrame);
        }

        pFrame->results.Fibo_results_r.pu = &pFrame->f2;
        {
            Fibo_frame *pFiboFrame =
                FiboCreate(pFrame->loomFrame.pLocalAlloc,
                           &pFrame->results.Fibo_results_r);
            pFiboFrame->args.u = pFrame->args.u - 2;
            PXLOOMFRAME_CALL(&pFrame->loomFrame, pLoom, &pFiboFrame->loomFrame);
        }

        *pFrame->pResults->pu = pFrame->f1 + pFrame->f2;
        PXLOOMFRAME_RETURN(&pFrame->loomFrame);
    }
    PXLOOMFRAME_END(&pFrame->loomFrame)
}

static const pxLoomContinuationVt Fibo_frameLoomContinuationVt =
{
    {
        offsetof(Fibo_frame, loomFrame.objectStruct.pObjectVt) -
            offsetof(Fibo_frame, loomFrame.pLoomContinuationVt),
        pxObject_getInterface,
    },
    Fibo_resume,
};


static const pxObjectInterface Fibo_frame_interfaces[] =
{
    {pxLoomContinuationName,
     offsetof(Fibo_frame, loomFrame.objectStruct.pObjectVt) -
         offsetof(Fibo_frame, loomFrame.pLoomContinuationVt)},
    {pxObjectName, 0},
};

static const pxObjectVt Fibo_frameObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    pxLoomFrame_destroy,
    pxObject_cloneForbidden, // TODO
    sizeof(Fibo_frame_interfaces)/sizeof(Fibo_frame_interfaces[0]),
    Fibo_frame_interfaces,
    sizeof(Fibo_frame),
    offsetof(Fibo_frame, loomFrame.objectStruct),
    0,
    NULL,
};

static Fibo_frame *FiboCreate(pxAlloc *const pAlloc, Fibo_results *pResults)
{
    Fibo_frame *const pFrame = PXALLOC_alloc(pAlloc, sizeof(Fibo_frame), 0);

    pxLoomFrameInit(&pFrame->loomFrame, pAlloc,
                    &Fibo_frameLoomContinuationVt,
                    &Fibo_frameObjectVt);

    pFrame->pResults = pResults;

    return pFrame;
}


typedef struct
{
    // locals
    pxLoomSemaphore *pProducerSem;
    pxLoomSemaphore *pConsumerSem;
    unsigned i;
#define PRODUCER_N 10
    unsigned *pu;
    bool *pIsDone;

    // called function results
    union
    {
        Fibo_results Fibo_results_r;
    } results;

    pxLoomFrame loomFrame;
} Producer_frame;

static pxLoomState Producer_resume(
    pxLoomContinuation *const pLC, pxLoom *const pLoom)
{
    Producer_frame *const pFrame =
        PXINTERFACE_STRUCT(pLC, Producer_frame, loomFrame.pLoomContinuationVt);

    PXLOOMFRAME_BEGIN(&pFrame->loomFrame)
    {
        for(pFrame->i = 0; pFrame->i < PRODUCER_N; ++pFrame->i)
        {
            PXLOOMFRAME_SEMAPHOREGET(
                &pFrame->loomFrame, pFrame->pConsumerSem, 1);

            pFrame->results.Fibo_results_r.pu = pFrame->pu;
            Fibo_frame *const pFiboFrame =
                FiboCreate(pFrame->loomFrame.pLocalAlloc,
                           &pFrame->results.Fibo_results_r);
            pFiboFrame->args.u = pFrame->i;
            PXLOOMFRAME_CALL(&pFrame->loomFrame, pLoom, &pFiboFrame->loomFrame);

            *pFrame->pIsDone = false;
            PXLOOMSEMAPHORE_put(pFrame->pProducerSem, 1);
        }

        *pFrame->pIsDone = true;
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
    pxAlloc *const pAlloc, unsigned *pu, bool *pIsDone,
    pxLoomSemaphore *pProducerSem, pxLoomSemaphore *pConsumerSem)
{
    Producer_frame *const pFrame =
        PXALLOC_alloc(pAlloc, sizeof(Producer_frame), 0);

    pxLoomFrameInit(&pFrame->loomFrame, pAlloc,
                    &Producer_frameLoomContinuationVt,
                    &Producer_frameObjectVt);

    pFrame->pProducerSem = pProducerSem;
    pFrame->pConsumerSem = pConsumerSem;
    pFrame->pu = pu;
    pFrame->pIsDone = pIsDone;

    return pFrame;
}


typedef struct
{
    // locals
    pxLoomSemaphore *pProducerSem;
    pxLoomSemaphore *pConsumerSem;
    unsigned *pu;
    bool isDone;
    int count;

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
        pFrame->pProducerSem = pxLoomSemaphoreCreate(pLoom, 0);
        pFrame->pConsumerSem = pxLoomSemaphoreCreate(pLoom, 1);

        // create the producer, feeding it the semaphores and the shared int
        {
            Producer_frame *const pProducer =
                ProducerCreate(pFrame->loomFrame.pLocalAlloc,
                               pFrame->pu, &pFrame->isDone,
                               pFrame->pProducerSem, pFrame->pConsumerSem);

            // start up the producer
            PXLOOM_createCell(pLoom, &pProducer->loomFrame);
        }

        // consume the producer's ints until we hit the last
        pFrame->count = 0;
        while(true)
        {
            PXLOOMFRAME_SEMAPHOREGET(
                &pFrame->loomFrame, pFrame->pProducerSem, 1);

            if (fibo(pFrame->count) != *pFrame->pu)
                fprintf(stderr, "fibo value incorrect (%d, %u)\n",
                        pFrame->count, *pFrame->pu);

            ++pFrame->count;
            if (pFrame->isDone)
            {
                if (pFrame->count != PRODUCER_N)
                    fprintf(stderr, "didn't generate enough numbers\n");

                // free the semaphores
                pxObject *pObject;
                pObject = PXINTERFACE_getInterface(
                    pFrame->pProducerSem, pxObject);
                PXOBJECT_destroy(pObject);
                pObject = PXINTERFACE_getInterface(
                    pFrame->pConsumerSem, pxObject);
                PXOBJECT_destroy(pObject);

                PXLOOMFRAME_RETURN(&pFrame->loomFrame);
            }

            PXLOOMSEMAPHORE_put(pFrame->pConsumerSem, 1);
        }

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

static Consumer_frame *ConsumerCreate(pxAlloc *const pAlloc, unsigned *pu)
{
    Consumer_frame *const pFrame =
        PXALLOC_alloc(pAlloc, sizeof(Consumer_frame), 0);

    pFrame->pu = pu;
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

    unsigned u = 0;
    Consumer_frame *const pConsumer = ConsumerCreate(pAllocD, &u);

    PXLOOM_createCell(pLoom, &pConsumer->loomFrame);
    PXLOOM_run(pLoom);

    if (u == 0)
        fprintf(stderr, "producer did not run\n");

    // free the loom and check the debug allocator
    // TODO free the loom and check the debug allocator
}

int main(void)
{
    pxAllocSystemInit();
    testpxLoom();
    return 0;
}
