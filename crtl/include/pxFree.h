
#ifndef PXFREE_H
#define PXFREE_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif


struct pxFree;
typedef struct pxFreeVt
{
    pxInterfaceVt interfaceVt;

    void (*free)(struct pxFree *pFree, void *p);
} pxFreeVt;

typedef struct pxFree
{
    const pxFreeVt *const pVt;
} pxFree;

extern const char pxFreeName[];

#define PXFREE_free(pI, p) \
    ((*(pI)->pVt->free)((pI), (p)))

#endif // PXFREE_H
