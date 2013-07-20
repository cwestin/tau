
#ifndef PXDLL_H
#include "pxDll.h"
#endif

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
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
