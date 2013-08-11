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

#ifndef PXINTERFACE_H
#define PXINTERFACE_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif


/**
   @class pxInterface
   All interfaces derive from pxInterface. pxInterface provides the means to
   get access to all the other interfaces implemented by an object.
 */
struct pxInterface;
typedef struct pxInterfaceVt
{
    ptrdiff_t pxObjectOffset; // offset of pxObject interface from this interface

    /**
       @fn getInterface @memberof pxInterface
       Get another interface pointer from this object.

       @param pThis pointer to an interface
       @param pName the name of the desired interface. Note that the current
         implementation assumes the standard name symbol is used, and only
         compares the name symbol addresses when names are referred to locally
         (in-process). Note that the invocation macro constructs a standard
         symbol name using the preprocessor's token concatenation operator
         (##), so the argument there should only be the interface's name
         as a symbol.
       @returns pointer to the requested interface, if the object implements it,
         otherwise NULL
    */
    struct pxInterface *(*getInterface)(
        struct pxInterface *pThis, const char *const pName);

#define PXINTERFACE_getInterface(pI, iName) \
    ((iName *)((*(pI)->pVt->interfaceVt.getInterface)((pxInterface *)(pI), iName ## Name)))

} pxInterfaceVt;

/*
  In order to be "derived" from pxInterface, all other interfaces include the
  pxInterfaceVt structure as their first element. In order to normalize the
  implementation of the PXINTERFACE_getInterface() macro so that it will work
  on any interface, including pxInterface itself, we create a faux interface
  that we can point to through the pxInterface structure so that the vtable's
  access is the same for pxInterface as it will be for all other interfaces.
 */
typedef struct
{
    pxInterfaceVt interfaceVt;
} pxInterfaceDerivativeVt;

typedef struct pxInterface
{
    const pxInterfaceDerivativeVt *const pVt;
} pxInterface;


/**
   Recover the pointer to a concrete implementation structure from the pointer
   to an interface.

   @param pI pointer to an interface
   @param sname the name of the concrete implementation's structure
   @param vtname the name of the structure member that points to the
     interface's vtable
   @returns a (typed) pointer to the concrete implementation's structure
 */
#define PXINTERFACE_STRUCT(pI, sname, vtname) \
    ((sname *)(((char *)(pI)) - offsetof(sname, vtname)))

#endif // PXINTERFACE_H
