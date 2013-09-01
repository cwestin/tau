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
/** @file pxHasher.h */

#ifndef PXHASHER_H
#define PXHASHER_H

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


/** @class pxHasher
  Cumulative hasher.

  After creating, repeatedly call hash methods on this to add more values to
  the hash. Once done, use getHash() to get the final value.
 */
struct pxHasher;
typedef struct
{
    pxInterfaceVt interfaceVt;

    // methods on here are not virtual (i.e., they are final)
} pxHasherVt;

typedef struct pxHasher
{
    const pxHasherVt *pVt;
} pxHasher;

extern const char pxHasherName[];

/** @fn getHash @memberof pxHasher
  Get the current hash value.

  @returns the current hash value
*/
int PXHASHER_getHash(pxHasher *pI);

/** @fn hashBytes @memberof pxHasher
  Hash the given bytes into the cumulative hash value.

  @param pI the hasher/this
  @param p pointer to the bytes to hash
  @param length number of bytes to hash
 */
void PXHASHER_hashBytes(pxHasher *pI, const void *p, size_t length);


/** @fn hashBytes @memberof pxHasher
  Hash a NULL-terminated C string into the cumulative hash value.

  @param pI the hasher/this
  @param p pointer to the string
 */
void PXHASHER_hashString(pxHasher *pI, const char *pS);

/**
   Create a hasher.

   @param pAlloc the allocator to use
   @param pOwner the owner to mix this into, if any; may be NULL
   @returns the newly created hasher
*/
struct pxAlloc;
pxHasher *pxHasherCreate(struct pxAlloc *pAlloc, pxInterface *pOwner);

#endif // PXHASHER_H
