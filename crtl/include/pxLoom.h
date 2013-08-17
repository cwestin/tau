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

struct pxLoomClosure;
typedef struct pxLoomClosureVt
{
    pxInterfaceVt interfaceVt;

    void (*resume)(struct pxLoomClosure *pLC);
#define PXLOOMCLOSURE_resume(pI) \
    ((*(pI)->pVt->resume)(pI))
} pxLoomClosureVt;

typedef struct pxLoomClosure
{
    const pxLoomClosureVt *const pVt;
} pxLoomClosure;

extern const char pxLoomClosureName[];


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
    unsigned lineNumber;
    struct pxAlloc *pLocalAlloc;
    struct pxLoomFrame *pPreviousFrame;

    const pxLoomClosureVt *pLoomClosureVt;
    pxObjectStruct objectStruct;
} pxLoomFrame;



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
} pxLoomVt;

typedef struct pxLoom
{
    const pxLoomVt *const pVt;
} pxLoom;

extern const char pxLoomName[];


pxLoom *pxLoomCreate(struct pxAlloc *pAlloc);

#endif // PXLOOM_H
