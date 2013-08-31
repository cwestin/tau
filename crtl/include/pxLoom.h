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
/** @file pxLoom.h
  The loom provides a means to fake thread-like co-routines in C. This includes
  synchronization primitives such as counting semaphores.

  The loom fakes co-routines using some preprocessor macro voodoo. However, the
  principle behind how this works is pretty simple.

  In a multi-processing operating system, context switching basically works
  by having a way to save away the current processes' state when the process is
  to be "unscheduled." The next process to run has its previous state restored
  (from the same saving process applied to it at some pointer earlier in time).

  Within a C switch statement, C doesn't care about the relative scope of case
  labels. These are treated like labels, and can appear anywhere within the
  switch. In particular, I can write something like this:

  switch(some_condition)
  {
  case A:
    for(i = 0; i < 10; ++i)
    {
      foo(...);
  case B:
      bar(...);
  case C:
      baz(...);
    }
  }

  Note how the case labels for B and C appear inside the for() loop's
  statement-block.

  In general, this looks like a crazy thing to do that will result in a
  non-sensical program, but it can be used to great effect. I can get a little
  help from the preprocessor and do things like this:

  void yielding_function(State *pState)
  {
    switch(pState->lineNumber)
    {
    case 0: // never executed before
      for(pState->i = 0; pState->i < 10; ++pState->i)
      {
          foo(...);
          if (some_condition) { pState->lineNumber = __LINE__; return; } case __LINE__:
          bar(...);
          if (some_condition) { pState->lineNumber = __LINE__; return; } case __LINE__:
          baz(...);
      }
    }
  }

  I've emulated the context-switching process: when I want to yield, I can save
  away the line number, and when I re-enter this function (with the preserved
  state), I will return to where execution left off.

  Note that for this to work, I cannot put any state in local variables --
  these will come and go across invocations of the C function. I must keep
  the higher-level stack and its associated locals in the State.

  In order to call other functions that can yield in this way, I build up a
  stack of States, each of which maintains the common stack frame state
  (analogous to the PC, or program counter at the assembly language level),
  as well as any function-specific state (such as "local variables" in the
  higher-level tau space we're representing here).

  Care must be taken not to use a nested switch, as that will assume ownership
  of any case labels that appear within it.

  The macros and functions in this file provide a realization of the above
  principle, using terminology from the multi-threaded programming world. It
  is not expected that users will write code this way, but that the initial
  language experiments will generate C code using this in order to emulate
  tau's green threads and behavior.
 */

#ifndef PXLOOM_H
#define PXLOOM_H

#ifndef PX_LIMITS_H
#include <limits.h>
#define PX_LIMITS_H
#endif

#ifndef PX_STDBOOL_H
#include <stdbool.h>
#define PX_STDBOOL_H
#endif

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif

#ifndef PXDLL_H
#include "pxDll.h"
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

/*
  Return values for PXLOOMFRAME_END used by the loom to co-ordinate execution
  across green threads
 */
typedef int pxLoomState;
#define PXLOOMSTATE_RETURN 1 // function returned normally (pop)
#define PXLOOMSTATE_CALL 2 // call a new function (push)
#define PXLOOMSTATE_WAIT 3 // unschedule and wait for an event

struct pxLoom;

/*
  Each loom-executable function's stack frame is modeled as an implementation
  of the continuation interface. The loom-executable function is implicitly
  provided to the loom through the implementation of this interface. In other
  words, the loom-executable function is the _resume method on this interface,
  and must be provided.
 */
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
  pxLoomFrame represents a stack frame for a loom-executable function call.

  In order to treat functions as first class objects, and to support closures,
  we declare that stack frames are objects which serve to hold the closure
  content. This allows us to use the existing pxObject mechanics for cloning
  stack frames if the green thread (or a function closure) needs to be garbage
  collected.

  Each loom-executable function must have an associated stack frame structure
  that contains its state (see the example at the top of this file in order
  to understand why this is required in order to preserve that state across
  the function call's save-and-restore). Embed this structure inside your
  stack frame structure, and initialize it by calling pxLoomFrameInit() (below).
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

/** @fn pxLoomFrameInit
  Initialize the common part of a loom-executable function's stack frame.

  This should be called on the stack frame after it has been allocated, but
  before the first call the the loom-executable function.

  @param pLoomFrame pointer to the common stack frame to initialize
  @param pAlloc the allocator that was used to allocate the stack frame
  @param pLoomContinuation pointer to the continuation interface implementation
  @param pLoomObjectVt pointer to the continutation object's Object implementation
 */
pxLoomContinuation *pxLoomFrameInit(
    pxLoomFrame *pLoomFrame, struct pxAlloc *pAlloc,
    const pxLoomContinuationVt *pLoomContinuationVt,
    const pxObjectVt *pLoomObjectVt);

/*
   Canned implementation of pxObject_destroy for loom frames.

   Uses the local allocator to deallocate the frame. Use this implementation of
   Object.destroy in your continuation object's Object interface implementation.
 */
void pxLoomFrame_destroy(struct pxObject *pI);


/*
  The following are macros that provide the line number saving and use
  mechanics demonstrated in the example at the top of this file. Use these
  in the implementation of loom-executable functions so that their state can
  be saved and restored. For examples of usage, see testsrc/testpxLoom.c.
 */

/**
  Introduces a loom-executable function's body.
 */
#define PXLOOMFRAME_BEGIN(pLoomFrame) \
    switch((pLoomFrame)->lineNumber) { case 0:

/**
  Concludes a loom-executable function's body.
 */
#define PXLOOMFRAME_END(pLoomFrame) \
    break; \
    default: pxExit("invalid line number (%d) at %s:%d\n", \
                    (pLoomFrame)->lineNumber, __FILE__, __LINE__); \
    break; } return PXLOOMSTATE_RETURN;

/**
  Return from a loom-executable function in the same way the "return" statement
  works in C.

  State for the function is marked as "completed," and the function may not be
  re-entered.
 */
#define PXLOOMFRAME_RETURN(pLoomFrame) \
    (pLoomFrame)->lineNumber = INT_MAX; return PXLOOMSTATE_RETURN;

/**
  Invoke a loom-executable function from within another loom-executable
  function.

  Prior to this, the stack frame for the target function must have been
  created and initialized.
 */
#define PXLOOMFRAME_CALL(pLoomFrame, pLoom, pCallFrame) \
    pxLoomCall(pLoom, pLoomFrame, __LINE__, pCallFrame); \
    return PXLOOMSTATE_CALL; case __LINE__:


/*
  Below are definitions for Dijkstra-style counting semaphores that operate in
  the context of a loom.

  The acquisition macro yields in ways similar to the _CALL and _RETURN macros
  above, but sometimes has to retry acquisitions because the availability of
  the semaphore may have changed since the time the loom thread was scheduled
  to run (under the belief that the required counts would be available). This
  is why the case label appears first, rather than last.

  Note that releasing semaphore counts is *not* a yielding operation, and
  does not have to operate this way. The put operation is on the loom semaphore
  interface defined below.
 */

/**
  Acquire a loom semaphore.

  Prior to use, the target semaphore must have been created. The function will
  not advance past this point until it can obtain the specified counts from the
  semaphore.

  @params n the number of semaphore counts to obtain
 */
#define PXLOOMFRAME_SEMAPHOREGET(pLoomFrame, pSem, n)    \
    case __LINE__: { \
        if (!((*(pSem)->pVt->get)(pSem, n))) { \
            (pLoomFrame)->lineNumber = __LINE__; return PXLOOMSTATE_WAIT; }}


/** @class pxLoomSemaphore
  The loom semaphore interface.

  The implementation of this interface is provided by pxLoom; create semaphores
  using pxLoomSemaphoreCreate().

  While this is not going to be exported into tau space, it is exposed as
  an abstract interface because I expect there will be two implementations of
  it: one local, for a semaphore that is *local to this loom,*, and one for
  semaphores that are owned by another loom (that is running in another real
  OS thread). Such communication will require more than just yielding locally,
  since real mutual exclusion will have to be used to pass data between
  OS-level threads. The design of that is still TBD, but I imagine it will
  involve some form of messaging that will still yield locally so that other
  local loom threads can continue to run. External I/O will do something
  similar, leading to what looks like blocking I/O from a loom function's
  perspective actually being Node.js-style asynchronous I/O under the covers.
 */
struct pxLoomSemaphore;
typedef struct pxLoomSemaphoreVt
{
    pxInterfaceVt interfaceVt;

    /** @fn put
      Add counts to the semaphore.

      @param pLS pointer to the loom semaphore
      @param n the number of counts to add
     */
    void (*put)(struct pxLoomSemaphore *pLS, unsigned n);
#define PXLOOMSEMAPHORE_put(pI, n) \
    ((*(pI)->pVt->put)(pI, n))

    bool (*get)(struct pxLoomSemaphore *pLS, unsigned n);
// use PXLOOMFRAME_SEMAPHOREGET
} pxLoomSemaphoreVt;

typedef struct pxLoomSemaphore
{
    const pxLoomSemaphoreVt *const pVt;
} pxLoomSemaphore;

extern const char pxLoomSemaphoreName[];

/**
   Create a loom semaphore

   @param pLoom the loom
   @param n the number of counts it should start with
   @returns pointer to the semaphore's interface
 */
pxLoomSemaphore *pxLoomSemaphoreCreate(struct pxLoom *pLoom, unsigned n);


/** @class pxLoom
  The Loom interface.

  This provides access to all of the Loom's functionality.
 */
struct pxLoom;
typedef struct pxLoomVt
{
    pxInterfaceVt interfaceVt;

    /** @fn createCell
      Create a new cell (green-thread) which will execute the given
      frame/continuation.

      @param pI the loom/this
      @param pFrame pointer to the initialized stack frame/continuation
     */
    void (*createCell)(struct pxLoom *pI, pxLoomFrame *pFrame);
#define PXLOOM_createCell(pI, pFrame) \
    ((*(pI)->pVt->createCell)(pI, pFrame))

    /** @fn run
      Execute the loom.

      This runs all executable loom functions until there are none left.

      @param pI the loom/this
      @returns the number of cells that are in some wait state and could not
        be run
     */
    unsigned (*run)(struct pxLoom *pI);
#define PXLOOM_run(pI) \
    ((*(pI)->pVt->run)(pI))
} pxLoomVt;

typedef struct pxLoom
{
    const pxLoomVt *const pVt;
} pxLoom;

extern const char pxLoomName[];


/**
  Create a loom.

  @param pAlloc the allocator to create the loom and its data structures in
 */
pxLoom *pxLoomCreate(struct pxAlloc *pAlloc);

/*
private:
  For internal use by the PXLOOMFRAME_CALL macro.

  @param pLoom the loom
  @param pFrame pointer to the current stack frame
  @param line the current line number (i.e., __LINE__) in the current execution
  @param pCallFrame pointer to the continuation stack frame to be called
 */
void pxLoomCall(pxLoom *pLoom,
                pxLoomFrame *pFrame, unsigned line,
                pxLoomFrame *pCallFrame);

#endif // PXLOOM_H
