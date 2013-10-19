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

#ifndef PXALIGN_H
#define PXALIGN_H

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif

/*
  We use the local compiler in order to determine memory alignment requirements
  on the local host. We use several C standards and truisms to do this:
  (1) truism: A union must be as badly aligned as its worst aligned member
  (2) standard: The address of union members must all be the same as the address
    of the union itself
  (3) standard: The address of the first member of a structure must be the same
    as the address of the union itself

  In order to determine what alignment is required, we declare a union that
  contains one of every basic type; this union must then have worst-case
  alignment.

  We can add that union to a structure that begins with a byte. The compiler
  is then forced to align the union by adding pad bytes between the byte and
  the union, and we can count those bytes using offsetof().
 */
struct pxAlignUnknown;
typedef union
{
    char c;
    short s;
    long l;
    int i;
    float f;
    double d;
    struct pxAlignUnknown *pU;
    void *p;
} pxAlignAll;

typedef struct
{
    char c;
    pxAlignAll alignAll;
} pxAligner;


/*
 This assumes that worst alignment must be a power of 2, which seems
 reasonable, given hardware bus layout requirements.
*/
#define PXALIGN_WORST offsetof(pxAligner, alignAll)
#define PXALIGN_WORST_MASK (PXALIGN_WORST - 1)

/*
  Return the aligned size for a size. That is, add enough padding to the
  argument that it could be used to create an array of items of this size,
  with each one beginning on the appropriate boundary.
 */
#define PXALIGN_SIZE(p) \
    (((p) + PXALIGN_WORST_MASK) & (~PXALIGN_WORST_MASK))

#endif // PXALIGN_H
