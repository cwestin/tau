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
/** @file pxLoom.h */

#ifndef PXLOOM_H
#define PXLOOM_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif

#ifndef PXEXIT_H
#include "pxExit.h"
#endif

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


struct pxAlloc;

/**
   @class pxLoom
   Ultimately, tau should use green threads to build cells or Erlang-style
   processes. However, at this stage, we're just experimenting with syntax
   and features. In order to avoid having to deal with writing a byte-code
   interpreter with green threads, we use the pxLoom class to simulate this
   using C features.

   Ideally, we can test out multi-threaded use by creating a real OS thread
   for every core, and running a loom on each thread. For this reason,
   the interfaces to related objects, such as pxLoomSemaphore, are abstract,
   under the expectation that there will be different implementations depending
   on whether the target is local or remote.
*/

typedef int pxLoomState;
#define PXLOOMSTATE_RETURN 1 // function returned normally

struct pxLoom;

struct pxLoomContinuation;
typedef struct pxLoomContinuationVt
{
    pxInterfaceVt interfaceVt;

    pxLoomState (*resume)(struct pxLoomContinuation *pLC, struct pxLoom *pL);
#define PXLOOMCONTINUATION_resume(pI, pL)               \
    ((*(pI)->pVt->resume)(pI, pL))
} pxLoomContinuationVt;

typedef struct pxLoomContinuation
{
    const pxLoomContinuationVt *const pVt;
} pxLoomContinuation;

extern const char pxLoomContinuationName[];


/*
  pxLoomFrame represents a stack frame for a function call.

  In order to treat functions as first class objects, and to support closures,
  we declare that stack frames are objects which serve to hold the closure
  content. This allows us to use the existing pxObject mechanics for cloning
  stack frames if the green thread (or a function closure) needs to be garbage
  collected.
 */
typedef struct pxLoomFrame
{
// private:
    unsigned lineNumber;
    struct pxAlloc *pLocalAlloc;
    struct pxLoomFrame *pPreviousFrame;

    const pxLoomContinuationVt *pLoomContinuationVt;
    pxObjectStruct objectStruct;
} pxLoomFrame;


pxLoomContinuation *pxLoomFrameInit(
    pxLoomFrame *pLoomFrame, struct pxAlloc *pAlloc,
    const pxLoomContinuationVt *pLoomContinuationVt,
    const pxObjectVt *pLoomObjectVt);

/**
   Canned implementation of pxObject_destroy for loom frames
 */
void pxLoomFrame_destroy(struct pxObject *pI);

#define PXLOOMFRAME_BEGIN(pLoomFrame) \
    switch((pLoomFrame)->lineNumber) { case 0: {

#define PXLOOMFRAME_END(pLoomFrame) \
    break; } \
    default: pxExit("invalid line number (%d) at %s:%d\n", \
                    (pLoomFrame)->lineNumber, __FILE__, __LINE__); \
    break; } return PXLOOMSTATE_RETURN;

struct pxLoomSemaphore;
typedef struct pxLoomSemaphoreVt
{
    pxInterfaceVt interfaceVt;

    void (*put)(struct pxLoomSemaphore *pLS, unsigned n);
    void (*get)(struct pxLoomSemaphore *pLS, unsigned n);
} pxLoomSemaphoreVt;

typedef struct pxLoomSemaphore
{
    const pxLoomSemaphoreVt *const pVt;
} pxLoomSemaphore;

extern const char pxLoomSemaphoreName[];


struct pxLoom;
typedef struct pxLoomVt
{
    pxInterfaceVt interfaceVt;

    void (*createCell)(struct pxLoom *pI, pxLoomFrame *pFrame);
#define PXLOOM_createCell(pI, pFrame) \
    ((*(pI)->pVt->createCell)(pI, pFrame))

    void (*run)(struct pxLoom *pI);
#define PXLOOM_run(pI) \
    ((*(pI)->pVt->run)(pI))
} pxLoomVt;

typedef struct pxLoom
{
    const pxLoomVt *const pVt;
} pxLoom;

extern const char pxLoomName[];


pxLoom *pxLoomCreate(struct pxAlloc *pAlloc);

#endif // PXLOOM_H
