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

/**
   @class pxHm
   pxHm provides an intrusive hash map implementation for high-performance
   applications. This object exposes its implementation, and is not suitable
   for export into tau space.
*/

#ifndef PXHM_H
#define PXHM_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif

#ifndef PXHASH_H
#include "pxHash.h"
#endif


struct pxAlloc;

/**
   @class pxHmEntry @memberof @pxHm
   pxHmEntry is the intrusive element you embed into your own structure in order
   to be able to insert it into a pxHm hash map. Initialization takes place
   when the item is inserted, and the content is managed by pxHm.

   Your hashed structure must contain both the key and value for your item.
   The key will be accessed indirectly for lookups via some of the functions in
   the dope vector below.
 */
typedef struct pxHmEntry
{
// private:    
    struct pxHmEntry *pNext; // next entry in the same hash bucket
    pxHashValue rawHash; // the raw hash value for this item's key
} pxHmEntry;

/**
   Recover a pointer to your structure from its pxHmEntry.

   @param pEntry pointer to the hash map entry
   @param sname the name of your structure
   @param ename the name of the pxHmEntry member in your structure
   @returns the (typecast) pointer to your structure
 */
#define PXHM_STRUCT(pEntry, sname, ename) \
    ((sname *)(((char *)(pEntry)) - offsetof(sname, ename)))

/**
   The dope vector for a hash map. Contains read-only configuration for your
   hashmap.

   Create an initialize a local static instance of this structure for your
   hash map instances; this will be passed in to pxHmMapInit().
 */
typedef struct
{
    /**
       The desired average bucket size for your hashmap.

       The size of the hash map bucket array is always a power of two. When the
       average number of elements in buckets exceeds avgBucket, the bucket
       array will be resized by doubling its size.
     */
    size_t avgBucket;

    /**
       The offset to the item's key from the pxHmEntry. This is used to find
       the key when it needs to be compared to a search key.
     */
    ptrdiff_t keyOffset; // offset from this to the key

    /**
       The hash function for your key.

       The signature has been chosen to coincide with convenience functions
       in pxHash.h; if your key is a scalar, there are functions there that
       can be used directly. If your key is a composite, create a new function
       of your own by composing others from pxHash.h or from other objects.

       @param pHash the pxHashValue computed so far
       @param p a pointer to the key
     */
    void (*hash)(pxHashValue *pHash, const void *p); // hash function for keys

    /**
       The comparison function for your key.

       The signature has been chosen to coincide with convenience functions
       in pxCmp.h; if your key is a scalar, there are functions there that can
       be used directly. If your key is a composite, create a new function of
       your own.

       @param pL pointer to the left-hand key being compared
       @param pR pointer to the right-hand key being compared
       @returns a number less than zero when the left key is less than the
         right key, zero when the two keys are equal, and a number greater than
         zero when the left key is greater than the right key
    */
    int (*cmp)(const void *pL, const void *pR); // comparison function for keys
} pxHmDope;

/**
   The hash map's private state. Presented in this form so that you may
   embed it. Initialize this with pxHmMapInit() before use.
 */
typedef struct pxHmMap
{
// private:    
    struct pxHmBucket *pBucket; // the bucket array
    size_t nBuckets; // current number of buckets
    size_t nEntries; // current number of entries

    const pxHmDope *pDope; // the dope vector for this hash map

    struct pxAlloc *pAlloc; // the allocator used for this hashmap
} pxHmMap;

/**
   Initialize a hash map.

   @param pMap pointer to an uninitialized map instance
   @param pDope dope vector for your hash map
   @param pAlloc allocator you want to use for your hashmap's entries
   @param initCap a hint for the initial capacity of your map; this will be
     combined with pDope->avgBucket to come up with an initial bucket size
     (which will be rounded up to the nearest power of two).
 */
void pxHmMapInit(pxHmMap *pMap, const pxHmDope *pDope,
                 struct pxAlloc *pAlloc, size_t initCap);

/**
   Count the current number of entries.

   @param pMap pointer to an initialized hash map
 */
static inline unsigned pxHmMapCount(const pxHmMap *pMap)
{
    return pMap->nEntries;
}

/**
   Find an element in the hashmap.

   Finds an element in the hashmap, along with the option to create an element
   if one isn't found.

   If you provide a creation callback, it must use the supplied allocator to
   create the new entry, and it must return the address of the embedded
   membership element, which will then be initialized on your behalf.

   @param pMap pointer to an initialized hashmap
   @param pKey pointer to the search key
   @param create function to call to create a new entry if one is not found;
     may be NULL
   @param pCtx pointer to a context for the creation function; may be NULL
   @returns pointer to the entry found
 */
pxHmEntry *pxHmMapFind(pxHmMap *pMap, const void *pKey,
               pxHmEntry *(*create)(void *pCtx, struct pxAlloc *pAlloc),
               void *pCtx);

#endif // PXHM_H
