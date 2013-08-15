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

#ifndef PXOBJECT_H
#define PXOBJECT_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


/**
   @class pxObject
   All objects implement this interface, which is a derivative of pxInterface.

   At this time, pxObject is limited to object lifecycle management methods
   that are only to be used by the tau runtime system, and this interface
   should not be exposed (directly) in tau space, but is used by language
   features. pxObject also exposes metadata through virtual data, but this is
   used exclusively by canned pxObject methods.
*/

/*
  pxObjectInterface is used to create lookup tables for interfaces implemented
  by an object. The concrete implementation defines an array of
  pxObjectInterface structures populated with the name and offset of each
  interface the object has an implementation for. pxObject_getInterface
  provides a canned implementation of pxInterface::getInterface that will
  operate on this data. The lookup table is pointed to by the pxObject vtable.
 */
typedef struct
{
    const char *const pName; // name of the interface: standard symbol address
    ptrdiff_t interfaceOffset; // offset from pxObject interface to this interface
} pxObjectInterface;

/*
  pxObjectMember is used to create lookup tables for members of an object.

  These carry metadata used in object lifecycle management. The current
  application is a data-driven implementation of pxObject_clone(). An object
  implementation must properly describe pointers to other objects with an
  entry in its member table in order for clone to work.
 */
typedef struct
{
    const char *const pName; // pointer to the official interface name symbol
    ptrdiff_t memberOffset; // offset from pxObject interface to this member
} pxObjectMember;

struct pxAlloc;
struct pxHmMap;

struct pxObjectCloner;

struct pxObject;
typedef struct pxObjectVt
{
    pxInterfaceVt interfaceVt;

    /**
       @fn destroy @memberof pxObject
       Destroy this object.

       Note that destruction must only occur once the object can no longer
       be referenced, and hence will be tied to the object's scope. This
       is called by the run-time system at points the compiler determines are
       safe. Destruction is decoupled from memory management, and memory may
       or may not be freed, depending on the object's implementation, and its
       relationship with its containing arena.

       @param pObject pointer to the pxObject interface for this object
    */
    void (*destroy)(struct pxObject *pObject);

#define PXOBJECT_destroy(pI) \
    ((*(pI)->pVt->destroy)((pI)))

    /**
       @fn clone @memberof pxObject
       Clone this object into a new arena.

       Note that if this object has already been cloned, it will be known
       inside pMap, and then all that happens is the requested interface is
       returned. This is used to avoid creating multiple copies of the same
       object. The hash map provides the ability to clone a graph of objects,
       starting from any one of them.

       @param pI pointer to the object interface
       @param pIName the interface to return; unlike PXINTERFACE_getInterface(),
         this macro does *not* create the name symbol through preprocessor
         manipulation; the expectation is that this will use internal variables,
         so that would not be useful here
       @param pCloner an initialized pxObjectCloner
       @returns a pointer to the requested interface on a clone of the object
    */
    pxInterface *(*clone)(
        struct pxObject *pI, const char *const pIName,
        struct pxObjectCloner *pCloner);

#define PXOBJECT_clone(pI, pAlloc, pIName, pMap) \
    ((*(pI)->pVt->clone)(pI, pAlloc, pIName, pMap))

    // used by pxObject_getInterface()
    size_t nInterface; // size of the interface table as a number of entries
    const pxObjectInterface *pInterface; // the interface table

    // used by pxObject_clone()
    size_t size; // size of this object
    ptrdiff_t objectOffset; // offset to the pxObjectVt
    size_t nMember; // size of the member table as a number of entries
    const pxObjectMember *pMember;

} pxObjectVt;

typedef struct pxObject
{
    const pxObjectVt *const pVt;
} pxObject;

extern const char pxObjectName[];


/**
  Canned implementation of pxInterface::getInterface() driven by
  pxObjectInterface data.

  Use a reference to this function pointer in the interfaceVt initializer for
  your object's definition of every interface it implements.

  @param pI pointer to any interface (which must be derived from pxInterface)
  @param pName the name of the interface; at present, this only does pointer
    comparisions, so you should use the interface's public Name symbol
  @returns a pointer to the requested interface, if it is available, NULL
    otherwise
 */
pxInterface *pxObject_getInterface(pxInterface *pI, const char *const pName);

/**
  Canned implementation of pxObject::destroy() driven by pxObjectStruct data.

  Use a reference to this function pointer in the pxObjectVt initializer for
  your object's definition of its implementation of pxObject.

  @param pI pointer to the pxObject interface for this object
 */
void pxObject_destroy(pxObject *pI);


struct pxObjectStruct;
struct pxObjectCloner_item;
typedef struct pxObjectCloner
{
// private:
    struct pxObjectStruct *(*clone)(
        struct pxObjectCloner *pCloner, struct pxObjectStruct *pO);
    struct pxObjectCloner_Item *pItemList; // list of items to process members

    struct pxHmMap *pMap; // the map of cloned objects, if we have it
    struct pxAlloc *pAlloc; // the allocator to use for cloning
} pxObjectCloner;

/**
   Initialize an object cloner in order to clone one or more objects.

   This is typically used when preparing to clone objects in one arena into
   another

   @param pCloner an uninitialized pxObjectCloner instance
   @param pAlloc the arena the objects will be cloned into
 */
void pxObjectClonerInitGraph(
    pxObjectCloner *pCloner, struct pxAlloc *const pAlloc);

/**
   Initialize an object cloner in order to clone one object.

   This is typically used when preparing to clone an object in order to
   send it through a channel or to clone it for local user use, such as
   cloning a key for a hash map.

   @param pCloner an uninitialized pxObjectCloner instance
   @param pAlloc the arena the objects will be cloned into
 */
void pxObjectClonerInitSingle(
    pxObjectCloner *pCloner, struct pxAlloc *const pAlloc);

/**
   Cleanup an object cloner that you are finished using

   @param pCloner the object cloner
 */
void pxObjectClonerCleanup(pxObjectCloner *pCloner);

/**
   Canned implementation of pxObject::clone() driven by pxObjectMember data.

   Use a reference to this function pointer in the pxObjectVt initializer for
   your object's definition of its implementation of pxObject.

   This implementation assumes the pxObject interface is implemented through
   a pxObjectStruct, and will use that to find the object's run-time metadata.

   @param pI pointer to the object interface
   @param pIName the interface to return
   @param pCloner an initialized pxObjectCloner
   @returns a pointer to the requested interface on the newly cloned object
*/
pxInterface *pxObject_clone(
    pxObject *pI, const char *const pIName, struct pxObjectCloner *pCloner);

/**
   Canned implementation for pxObject::clone() for objects which cannot be
   cloned.

   Some objects on the borders of tau space do not support cloning; this can be
   used for their implementation. Currently, it exits with an error.

   @param pI pointer to the object interface
   @param pIName the interface to return
   @param pCloner an initialized pxObjectCloner
   @returns a pointer to the requested interface on the newly cloned object
*/
pxInterface *pxObject_cloneForbidden(
    pxObject *pI, const char *const pIName, struct pxObjectCloner *pCloner);

/*
  pxObjectStruct: embed this in your object to represent your object's
  pxObject implementation.

  For most interfaces, an object's concrete implementation will simply contain
  members that are pointers to those interfaces' vtables. The canned pxObject
  implementation requires some additional mutable per-object state. This state
  is captured in pxObjectStruct.

  Putting this here makes it convenient to change this (if necessary) without
  have to change the declaration of every concrete implementation struct.

  Note that when initializing your object, you must initialize the nested
  pxObjectStruct by calling pxObjectStructInit(), below.
 */
typedef struct pxObjectStruct
{
// private:
    struct pxObjectStruct *pNextMixin; // next mixin on owners mixin list
    struct pxObjectStruct *pMixinList; // mixin list for this, if any
    struct pxObjectStruct *pOwner; // owner of this mixin, if any

    const pxObjectVt *pObjectVt;
} pxObjectStruct;

/**
  Initialize a pxObjectStruct instance.

  @param pObjectStruct the embedded pxObjectStruct in your implementation struct
  @param pVt your pxObjectVt
  @param pOwner the owning object if this object is a mixin that is being mixed
    in to another object (the owner)
*/
void pxObjectStructInit(
    pxObjectStruct *pObjectStruct, const pxObjectVt *pVt,
    pxInterface *pOwner);

#endif // PXOBJECT_H
