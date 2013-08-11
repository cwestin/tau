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

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXEXIT_H
#include "pxExit.h"
#endif

#ifndef PXHM_H
#include "pxHm.h"
#endif


const char pxObjectName[] = "pxObject";

static pxInterface *pxObjectStruct_getInterface(
    const pxObjectStruct *const pObjectStruct, const char *const pName)
{
    // check through interfaces this object implements directly
    const pxObjectInterface *pInterface = pObjectStruct->pObjectVt->pInterface;
    for(int i = pObjectStruct->pObjectVt->nInterface; i; ++pInterface, --i)
    {
        if (pInterface->pName == pName)
        {
            return (pxInterface *)
                (((char *)&pObjectStruct->pObjectVt) -
                 pInterface->interfaceOffset);
        }
    }

    // check on our mixins
    for(const pxObjectStruct *pMixin = pObjectStruct->pNextMixin; pMixin;
        pMixin = pMixin->pNextMixin)
    {
        // For this lookup, we use the direct implementation, otherwise
        // the mixin will use the owner pointer, causing infinite recursion
        pxInterface *const pI = pxObjectStruct_getInterface(pMixin, pName);
        if (pI)
            return pI;
    }

    // if we got here, we didn't find it
    return NULL;
}

pxInterface *pxObject_getInterface(pxInterface *pI, const char *const pName)
{
    pxObject *const pObject =
        (pxObject *)(((char *)pI) + pI->pVt->interfaceVt.pxObjectOffset);
    pxObjectStruct *const pObjectStruct =
        PXINTERFACE_STRUCT(pObject, pxObjectStruct, pObjectVt);
    
    // if we have an owner, i.e., are part of an aggregate, delegate to owner
    if (pObjectStruct->pOwner)
    {
        // we can't use the macro here because it relies on symbol manipulation
        return pObjectStruct->pOwner->pObjectVt->interfaceVt.getInterface(
            (pxInterface*)&pObjectStruct->pOwner->pObjectVt, pName);
    }

    // check this object and its mixins
    return pxObjectStruct_getInterface(pObjectStruct, pName);
}

void pxObject_destroy(pxObject *pI)
{
    pxObjectStruct *const pThis =
        PXINTERFACE_STRUCT(pI, pxObjectStruct, pObjectVt);

    // mixins have to be destroyed in the reverse order, and need to be kept
    // around until they are destroyed, in case they depend on each other
    if (pThis->pNextMixin)
    {
        PXOBJECT_destroy((pxObject *)&pThis->pNextMixin->pObjectVt);

        // remove the destroyed one from the list
        pThis->pNextMixin = NULL;
    }
}

pxInterface *pxObject_clone(pxObject *pI, pxAlloc *pAlloc,
                            const char *const pIName, pxHmMap *pMap)
{
    pxExit("pxObject_clone: unimplemented\n");

    return NULL;
}

pxInterface *pxObject_cloneForbidden(pxObject *pI, pxAlloc *pAlloc,
                                     const char *const pIName, pxHmMap *pMap)
{
    pxExit("pxObject_clone: attempt to clone uncloneable object\n");

    return NULL;
}

void pxObjectStructInit(
    pxObjectStruct *pObjectStruct, const pxObjectVt *pVt,
    pxInterface *pIOwner)
{
    pObjectStruct->pObjectVt = pVt;
    pObjectStruct->pNextMixin = NULL;
    pObjectStruct->pOwner = NULL;

    // if there's an owner add this to it
    if (pIOwner)
    {
        pxObject *const pOOwner = PXINTERFACE_getInterface(pIOwner, pxObject);
        pxObjectStruct *const pOwner =
            PXINTERFACE_STRUCT(pOOwner, pxObjectStruct, pObjectVt);

        pObjectStruct->pOwner = pOwner;

        // this new object has to go on the end of the owner's mixin list
        pxObjectStruct **ppMixin;
        for(ppMixin = &pOwner->pNextMixin; *ppMixin;
            ppMixin = &(*ppMixin)->pNextMixin)
            ;
        *ppMixin = pObjectStruct;
    }
}
