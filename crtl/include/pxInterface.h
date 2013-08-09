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

#ifndef PXINTERFACE_H
#define PXINTERFACE_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif


struct pxInterface;
typedef struct pxInterfaceVt
{
    ptrdiff_t pxObjectOffset; // offset of pxObject interface from this interface
    struct pxInterface *(*getInterface)(
        struct pxInterface *pThis, const char *const pName);
} pxInterfaceVt;

typedef struct pxInterface
{
    const pxInterfaceVt *const pVt;
} pxInterface;

#define PXINTERFACE_getInterface(pI, iName) \
    ((iName *)((*(pI)->pVt->interfaceVt.getInterface)((pxInterface *)(pI), iName ## Name)))


#define PXINTERFACE_STRUCT(pI, sname, vtname) \
    ((sname *)(((char *)(pI)) - offsetof(sname, vtname)))

#endif // PXINTERFACE_H
