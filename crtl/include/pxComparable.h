
#ifndef PXCOMPARABLE_H
#define PXCOMPARABLE_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


struct pxComparable;
typedef struct
{
    pxInterfaceVt intefaceVt;

    void (*compare)(struct pxComparable *pI,
                 struct pxInterface *pL, struct pxInterface *pR);
} pxComparableVt;

typedef struct pxComparable
{
    const pxComparableVt *pComparableVt;
} pxComparable;

extern const char pxComparableName[];

#endif // PXCOMPARABLE_H
