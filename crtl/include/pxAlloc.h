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
/** @file */

#ifndef PXALLOC_H
#define PXALLOC_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


/**
   @class pxAlloc
   Allocator interface.

   pxAlloc provides a means to allocate memory. This is not expected to be used
   directly in tau space, but rather provides the pure abstract class that
   language features related to allocation will use to obtain service.
*/

struct pxAlloc;
typedef struct pxAllocVt
{
    pxInterfaceVt interfaceVt;

    /**
       @fn alloc @memberof pxAlloc
       Allocate memory.

       Returned memory is aligned suitable to contain anything. The returned
       memory may or may not be managed by an arena that supports piecemeal
       freeing; see the details on the arena in use.

       @param pAlloc pointer to the interface
       @param size size of the memory
       @param flag options for allocation, specified by PXALLOC_F_* flags
     */
#define PXALLOC_F_DIRTY      0x0001 // don't clear memory before returning
#define PXALLOC_F_RETURN_OOM 0x0002 // return NULL on OOM
    void *(*alloc)(struct pxAlloc *pAlloc, size_t size, int flag);

#define PXALLOC_alloc(pI, size, flag) \
    ((*(pI)->pVt->alloc)((pI), (size), (flag)))

} pxAllocVt;

typedef struct pxAlloc
{
    const pxAllocVt *const pVt;
} pxAlloc;

extern const char pxAllocName[];

#endif // PXALLOC_H
