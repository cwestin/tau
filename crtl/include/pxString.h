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
/** @file pxString.h */

#ifndef PXSTRING_H
#define PXSTRING_H

#ifndef PX_STDBOOL_H
#include <stdbool.h>
#define PX_STDBOOL_H
#endif

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


/**
   @class pxString
   Immutable String.

   Implements pxHashable, pxComparable.
*/

struct pxString;
typedef struct pxStringVt
{
    pxInterfaceVt interfaceVt;

    /**
       @fn isEmpty @memberof pxString
       Determine if a string is empty.

       @returns true if the string is empty, false otherwise
     */
    bool (*isEmpty)(struct pxString *pString);

#define PXSTRING_isEmpty(pI) \
    ((*(pI)->pVt->isEmpty)(pI))

} pxStringVt;

typedef struct pxString
{
    const pxStringVt *const pVt;
} pxString;

extern const char pxStringName[];


/**
   Create a pxString from a C string.

   @param pAlloc the allocator to use
   @param the C string
   @returns the created string
 */
struct pxAlloc;
pxString *pxStringCreate(struct pxAlloc *pAlloc, const char *pS);

#endif // PXSTRING_H
