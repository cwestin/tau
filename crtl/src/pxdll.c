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

#ifndef PXDLL_H
#include "pxDll.h"
#endif


bool pxDllIsValid(const pxDllHead *const pThis)
{
    // check if the link is malformed
    if (pThis->pNext == NULL)
        return false;
    if (pThis->pPrevious == NULL)
        return false;

    // check for the empty list
    if (pThis->pNext == pThis)
    {
        if (pThis->pPrevious != pThis)
            return false;

        return true;
    }

    // go through the list, checking on the values in links, and for loops
    bool alternate = false;
    const pxDllLink *pOther = pThis;
    for(const pxDllLink *pLink = pThis->pNext; pLink != pThis;
        alternate = !alternate, pLink = pLink->pNext)
    {
        // check if the link is malformed
        if (pLink->pNext == NULL)
            return false;
        if (pLink->pPrevious == NULL)
            return false;

        if (pLink->pNext->pPrevious != pLink)
            return false;
        if (pLink->pPrevious->pNext != pLink)
            return false;

        // check to see if we've looped around
        if (pLink == pOther)
            return false;
        if (alternate)
            pOther = pxDllGetNext(pThis, pOther);
    }

    // if we got here, all is well
    return true;
}

unsigned pxDllCount(const pxDllHead *const pThis)
{
    unsigned count = 0;
    for(pxDllLink *pLink = pxDllGetFirst(pThis); pLink;
        pLink = pxDllGetNext(pThis, pLink))
    {
        ++count;
    }

    return count;
}
