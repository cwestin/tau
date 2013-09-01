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
/** @file pxPoke.h */

#ifndef PXPOKE_H
#define PXPOKE_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


/**
   @class pxPoke
   Poke interface.

   pxPoke provides a means to poke an object.
*/

struct pxPoke;
typedef struct pxPokeVt
{
    pxInterfaceVt interfaceVt;

    /**
       @fn poke @memberof pxPoke
       Poke a target.
     */
    void (*poke)(struct pxPoke *pPoke);

#define PXPOKE_poke(pI) \
    ((*(pI)->pVt->poke)(pI))

} pxPokeVt;

typedef struct pxPoke
{
    const pxPokeVt *const pVt;
} pxPoke;

extern const char pxPokeName[];

#endif // PXPOKE_H
