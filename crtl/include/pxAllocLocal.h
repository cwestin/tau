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
/** @class pxAllocLocal
  Implementation of an allocator that gets all of its space from a local
  variable (an array that provides space).

  To use, a local variable is declared and initialized. The declaration
  is done with a macro that takes an argument specifying the amount of space
  to make available. Once the available space is used up, subsequent allocation
  requests fail.

  The structure is declared publicly, but its contents should be considered
  private.

  This won't be exported into tau space.
 */

#ifndef PXALLOCLOCAL_H
#define PXALLOCLOCAL_H

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif

#ifndef PXALIGN_H
#include "pxAlign.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


/*
  Used by the PXALLOCLOCAL_SPACE macro; do not use directly.
 */
typedef struct
{
// private:
    char *p; // pointer to the next available space
    size_t avail; // the amount of space left

    const struct pxAllocVt *pAllocVt;
    pxObjectStruct objectStruct;

    pxAlignAll space[1]; // available space trails off the end
} pxAllocLocal_s;

/**
  Reserve space in a local stack frame for an allocator.

  Use this to declare your variable as

    PXALLOCLOCAL_SPACE(42) someSpace;

  Space must be initialized with PXALLOCLOCAL_INIT() before use.

  When the function returns, the space will no longer exist, so don't retain
  any pointers. This is best used to inject memory management for bounded needs.

  @param size the amount of space to reserve.
 */
#define PXALLOCLOCAL_SPACE(size) \
    union \
    { \
        pxAllocLocal_s allocLocal; \
        pxAlignAll space[(sizeof(pxAllocLocal_s) + (size) - 1) / sizeof(pxAlignAll)]; \
    }

/**
  Initialize locally reserved space for use as a local allocator.

  Use this to initialize your variable declared with PXALLOCLOCAL_SPACE:

    PXALLOCLOCAL_INIT(&someSpace);

  @param ps pointer to your space-reserving variable
*/
#define PXALLOCLOCAL_INIT(ps) \
    pxAllocLocalInit(&(ps)->allocLocal, sizeof(*ps))

/*
  Used by the PXALLOCLOCAL_INIT() macro to initialize your space.
 */
struct pxAlloc *pxAllocLocalInit(pxAllocLocal_s *pLocal, size_t size);

#endif // PXALLOCLOCAL_H
