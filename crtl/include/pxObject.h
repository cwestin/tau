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
   that are only to be used by the tau runtime, and this interface should not
   be exposed in tau space. pxObject also exposes metadata through virtual
   data, but this is used exclusively by canned pxObject methods.
*/

/*
  pxObjectLookup is used to create lookup tables for interfaces implemented by
  an object. The concrete implementation defines an array of pxObjectLookup
  structures populated with the name and offset of each interface the object
  has an implementation for. pxObject_getInterface provides a canned
  implementation of pxInterface::getInterface that will operate on this data.
  The lookup table is pointed to by the pxObject vtable.
 */
typedef struct
{
    const char *const pName; // name of the interface: standard symbol address
    ptrdiff_t interfaceOffset; // offset from pxObject interface to this interface
} pxObjectLookup;

struct pxObject;
typedef struct pxObjectVt
{
    pxInterfaceVt interfaceVt;

    size_t nLookup; // size of the lookup table as number of entries
    const pxObjectLookup *pLookup; // the lookup table

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

} pxObjectVt;

typedef struct pxObject
{
    const pxObjectVt *const pVt;
} pxObject;

extern const char pxObjectName[];


/**
  Canned implementation of pxInterface::getInterface() driven by pxObjectLookup
  data.

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
  Canned implementation of pxObject::destroy() driven by pxObjectMeta data.

  Use a reference to this function pointer in the pxObjectVt initializer for
  your object's definition of its implementation of pxObject.

  @param pI pointer to the pxObject interface for this object
 */
void pxObject_destroy(pxObject *pI);


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
    const pxObjectVt *pObjectVt;
    struct pxObjectStruct *pNextMixin;
    struct pxObjectStruct *pOwner;
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
