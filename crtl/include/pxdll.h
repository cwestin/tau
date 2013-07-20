
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

typedef struct pxDllLink
{
    struct pxDllLink *pNext;
    struct pxDllLink *pPrevious;
} pxDllLink;

typedef pxDllLink pxDllHead;

static inline void pxDllInit(pxDllLink *const pThis)
{
    pThis->pNext = pThis;
    pThis->pPrevious = pThis;
}

static inline void pxDllRemoveUnsafe(pxDllLink *const pThis)
{
    // make the next and previous links point around this link
    pThis->pNext->pPrevious = pThis->pPrevious;
    pThis->pPrevious->pNext = pThis->pNext;
}

static inline void pxDllRemove(pxDllLink *const pThis)
{
    pxDllRemoveUnsafe(pThis);
    pxDllInit(pThis);
}

static inline void pxDllAddBefore(pxDllLink *const pThis, pxDllLink *pOther)
{
    pThis->pNext = pOther;
    pThis->pPrevious = pOther->pPrevious;

    pOther->pPrevious->pNext = pThis;
    pOther->pPrevious = pThis;
}

static inline void pxDllAddAfter(pxDllLink *const pThis, pxDllLink *pOther)
{
    pThis->pPrevious = pOther;
    pThis->pNext = pOther->pNext;

    pOther->pNext->pPrevious = pThis;
    pOther->pNext = pThis;
}

static inline bool pxDllIsEmpty(pxDllHead *const pThis)
{
    return pThis->pNext == pThis;
}

static inline void pxDllAddFirst(pxDllHead *const pThis, pxDllLink *pLink)
{
    pxDllAddAfter(pLink, pThis);
}

static inline void pxDllAddLast(pxDllHead *const pThis, pxDllLink *pLink)
{
    pxDllAddBefore(pLink, pThis);
}

static inline pxDllLink *pxDllGetNext(
    const pxDllHead *pThis, const pxDllLink *pLink)
{
    return pLink->pNext == pThis ? NULL : pLink->pNext;
}

static inline pxDllLink *pxDllGetFirst(const pxDllHead *pThis)
{
    return pxDllGetNext(pThis, pThis);
}

#define pxDllGetStruct(pThis, structName, linkName) \
    ((structName *)(((char *)pThis) - offsetof(structName, linkName)))


bool pxDllIsValid(const pxDllHead *pThis);

#endif // PXDLL_H
