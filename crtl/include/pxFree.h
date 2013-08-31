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

#ifndef PXFREE_H
#define PXFREE_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif


/** @class pxFree
  Abstract interface for freeing memory.

  Some allocators implement this, some do not. PXINTERFACE_getInterface() must
  be used to query for it, and it may only be used conditionally if it is
  present.

  This is not expected to be exported into tau space, but is only to be used
  by the run-time to manage memory on behalf of the user.
 */
struct pxFree;
typedef struct pxFreeVt
{
    pxInterfaceVt interfaceVt;

    /** @fn free
        @param pFree the allocator/this
        @param p pointer to the piece of memory to be freed
     */
    void (*free)(struct pxFree *pFree, void *p);
#define PXFREE_free(pI, p) \
    ((*(pI)->pVt->free)((pI), (p)))
} pxFreeVt;

typedef struct pxFree
{
    const pxFreeVt *const pVt;
} pxFree;

extern const char pxFreeName[];

#endif // PXFREE_H
