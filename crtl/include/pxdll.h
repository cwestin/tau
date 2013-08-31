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
/** @file pxDll.h
Intrusive doubly linked list implementation.

A public membership structure, pxDllLink is provided, which can be embedded
into other structures that are meant to be put onto doubly linked lists.
Functions in this file manage the list strictly through the pointers in the
embedded membership. A macro with an offsetof() expression is then used to
recover the pointer to the original object from a pointer to the membership
structure.

For some examples of use, see the unit test in testsrc/testpxDll.c.
 */

#ifndef PXDLL_H
#define PXDLL_H

#ifndef PX_STDBOOL_H
#include <stdbool.h>
#define PX_STDBOOL_H
#endif

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif


/** @class pxDllLink
  Membership structure.

  Embed this within structures you want to put on a list. Initialize it with
  pxDllInit() before use. Once initialized, other functions in this module can
  be used to manipulate the link or the list it is a member of, if any.

  Be sure not to free the structure containing the link without first removing
  it from the list.
 */
typedef struct pxDllLink
{
// private:
    /*
      When initialized, these pointers point to the link itself. This makes
      operations like removal safe and idempotent.

      When used as the head of a list, the head serves as a sentinel for some
      operations, such as pxDllGetNext() and pxDllGetPrevious(), because the
      last and first links (respectively) point to the head.
     */
    struct pxDllLink *pNext;
    struct pxDllLink *pPrevious;
} pxDllLink;

/** @class pxDllHead
  Head of a doubly linked list.

  Must be initialized with pxDllInit() before use. Once initialized, the head
  represents an empty list. Items may be added to the head or end of the queue,
  and there are methods to assist in iterating through the queue.
 */
typedef pxDllLink pxDllHead;

/** @fn pxDllInit
  Initialize an intrusive link or head of a doubly-linked list.

  Use this once during the life of a link or list head before using it.

  @param pThis pointer to the link
 */
static inline void pxDllInit(pxDllLink *const pThis)
{
    pThis->pNext = pThis;
    pThis->pPrevious = pThis;
}

/*
private:
  Remove a link from a list without resetting it to represent an initialized
  link.

  Used internally as an optimization when the internal operation is going to
  follow this with another link setting operation.
 */
static inline void pxDllRemoveUnsafe(pxDllLink *const pThis)
{
    // make the next and previous links point around this link
    pThis->pNext->pPrevious = pThis->pPrevious;
    pThis->pPrevious->pNext = pThis->pNext;
}

/** @fn pxDllRemove
  Remove a link from a list.

  Calling this does not require having access to the list head.

  Note that it is an error to call this on a list head.

  @param pThis pointer to the link to remove
 */
static inline void pxDllRemove(pxDllLink *const pThis)
{
    pxDllRemoveUnsafe(pThis);
    pxDllInit(pThis);
}


/** @fn pxDllAddBefore
  Add one link before another "anchor" link in a list.

  @param pThis the "anchor" link, relative to which the operation occurs
  @param pOther the link that will be added before the "anchor" in the list
 */
static inline void pxDllAddBefore(pxDllLink *const pThis, pxDllLink *pOther)
{
    pThis->pNext = pOther;
    pThis->pPrevious = pOther->pPrevious;

    pOther->pPrevious->pNext = pThis;
    pOther->pPrevious = pThis;
}

/** @fn pxDllAddAfter
  Add one link after another "anchor" link in a list.

  @param pThis the "anchor" link, relative to which the operation occurs
  @param pOther the link that will be added after the "anchor" in the list
 */
static inline void pxDllAddAfter(pxDllLink *const pThis, pxDllLink *pOther)
{
    pThis->pPrevious = pOther;
    pThis->pNext = pOther->pNext;

    pOther->pNext->pPrevious = pThis;
    pOther->pNext = pThis;
}

/** @fn pxDllIsEmpty
  Report is a list is empty or not

  @param pThis the pointer to the head of the list
  @returns true if the list is empty, false otherwise
*/
static inline bool pxDllIsEmpty(pxDllHead *const pThis)
{
    return pThis->pNext == pThis;
}

/** @fn pxDllAddFirst
  Add a new link to the beginning of a list.

  @param pThis pointer to the head of the list
  @param pLink pointer to the new link
*/
static inline void pxDllAddFirst(pxDllHead *const pThis, pxDllLink *pLink)
{
    pxDllAddAfter(pLink, pThis);
}

/** @fn pxDllAddLast
  Add a new link to the end of a list.

  @param pThis pointer to the head of the list
  @param pLink pointer to the new link
*/
static inline void pxDllAddLast(pxDllHead *const pThis, pxDllLink *pLink)
{
    pxDllAddBefore(pLink, pThis);
}

/** @fn pxDllGetNext
  Get the next link on the list; this gets a link after a specified link
  for a specified list.

  @param pThis pointer to the head of the list
  @param pLink the current link
  @returns pointer to the link after pLink, or NULL if pLink is the last one
*/
static inline pxDllLink *pxDllGetNext(
    const pxDllHead *pThis, const pxDllLink *pLink)
{
    return pLink->pNext == pThis ? NULL : pLink->pNext;
}

/** @fn pxDllGetPrevious
  Get the previous link on the list; this gets a link before a specified link
  for a specified list.

  @param pThis pointer to the head of the list
  @param pLink pointer to the current link
  @returns pointer to the link before pLink, or NULL if pLink is the last one
*/
static inline pxDllLink *pxDllGetPrevious(
    const pxDllHead *pThis, const pxDllLink *pLink)
{
    return pLink->pPrevious == pThis ? NULL : pLink->pPrevious;
}

/** @fn pxDllGetFirst
  Get the first link on a list.

  @param pThis pointer to the head of the list
  @returns a pointer to the first link on the list, or NULL if the list is
    empty
*/
static inline pxDllLink *pxDllGetFirst(const pxDllHead *pThis)
{
    return pxDllGetNext(pThis, pThis);
}

/** @fn pxDllGetLast
  Get the last link on a list.

  @param pThis pointer to the head of the list
  @returns a pointer to the last link on the list, or NULL if the list is
    empty
*/
static inline pxDllLink *pxDllGetLast(const pxDllHead *pThis)
{
    return pxDllGetPrevious(pThis, pThis);
}

/**
   Recover the pointer to a list member from the membership.

   @param pThis pointer to the membership (the pxDllLink element)
   @param structName the C structure name for the containing structure
   @param linkName the name of the link element in structName
 */
#define PXDLL_STRUCT(pThis, structName, linkName) \
    ((structName *)(((char *)pThis) - offsetof(structName, linkName)))


/** @fn pxDllIsValid
  Check a list for valid structure.

  This checks to see if the list is circular, as well as making sure that the
  forwards and backwards pointers match up.

  @param pThis pointer to the head of the list
  @returns true if the list is valid, false otherwise
*/
bool pxDllIsValid(const pxDllHead *pThis);

/** @fn pxDllCount
  Count the number of elements on the list.

  @param pThis pointer to the head of the list
  @returns the count of elements on the list
*/
unsigned pxDllCount(const pxDllHead *pThis);

#endif // PXDLL_H
