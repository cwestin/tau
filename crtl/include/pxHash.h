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

/** @file pxHash.h
   Provides a definition for a hash value, as well as several utility functions
   for hashing various native types.

   The signature of the utility functions conforms to that required for
   pxHmMap's use.
 */

#ifndef PXHASH_H
#define PXHASH_H

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif

/**
   Used to accumulate and hold a hash value
 */
typedef int pxHashValue;

/**
   Initialize a hash value

   @param pHash pointer to an uninitialized hash value
 */
static inline void pxHashInit(pxHashValue *pHash)
{
    *pHash = 0xca5015ac;
}

/**
   Hash a sequence of bytes.

   @param pHash pointer to the hash value so far
   @param p pointer to the bytes to hash
   @param length number of bytes to hash
 */
void pxHashVoid(pxHashValue *pHash, const void *p, size_t length);


/**
   Hash an int.

   @param pHash pointer to the hash value so far
   @param p pointer to the pointer to the int
 */
void pxHashInt(pxHashValue *pHash, const void *p);

/**
   Hash a string.

   @param pHash pointer to the hash value so far
   @param p pointer to the pointer to the string (pointer to the (char *))
 */
void pxHashString(pxHashValue *pHash, const void *p);

/**
   Hash a structure pointer.

   @param pHash pointer to the hash value so far
   @param p pointer to the pointer to the struct (pointer to the (struct x *))
*/
void pxHashStructStar(pxHashValue *pHash, const void *p);

#endif // PXHASH_H
