
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

typedef struct pxdllLink
{
    struct pxdllLink *pNext;
    struct pxdllLink *pPrevious;
} pxdllLink;

typedef pxdllLink pxdllHead;

static inline void pxdllInit(pxdllLink *const pThis)
{
    pThis->pNext = pThis;
    pThis->pPrevious = pThis;
}

static inline void pxdllRemoveUnsafe(pxdllLink *const pThis)
{
    // make the next and previous links point around this link
    pThis->pNext->pPrevious = pThis->pPrevious;
    pThis->pPrevious->pNext = pThis->pNext;
}

static inline void pxdllRemove(pxdllLink *const pThis)
{
    pxdllRemoveUnsafe(pThis);
    pxdllInit(pThis);
}

static inline void pxdllAddBefore(pxdllLink *const pThis, pxdllLink *pOther)
{
    pThis->pNext = pOther;
    pThis->pPrevious = pOther->pPrevious;

    pOther->pPrevious->pNext = pThis;
    pOther->pPrevious = pThis;
}

static inline void pxdllAddAfter(pxdllLink *const pThis, pxdllLink *pOther)
{
    pThis->pPrevious = pOther;
    pThis->pNext = pOther->pNext;

    pOther->pNext->pPrevious = pThis;
    pOther->pNext = pThis;
}

static inline bool pxdllIsEmpty(pxdllHead *const pThis)
{
    return pThis->pNext != pThis;
}

static inline void pxdllAddFirst(pxdllHead *const pThis, pxdllLink *pLink)
{
    pxdllAddAfter(pLink, pThis);
}

static inline void pxdllAddLast(pxdllHead *const pThis, pxdllLink *pLink)
{
    pxdllAddBefore(pLink, pThis);
}

static inline pxdllLink *pxdllGetNext(
    const pxdllHead *pThis, const pxdllLink *pLink)
{
    return pLink->pNext == pThis ? NULL : pLink->pNext;
}

#define pxdllGetStruct(pThis, structName, linkName) \
    ((structName *)(((char *)pThis) - offsetof(structName, linkName)))


bool pxdllIsValid(const pxdllHead *pThis);

#endif // PXDLL_H
