
#ifndef PXCOMPARABLE_H
#define PXCOMPARABLE_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


struct pxComparable;
typedef struct
{
    pxInterfaceVt intefaceVt;

    int (*compare)(struct pxComparable *pI, struct pxInterface *pO);
} pxComparableVt;

typedef struct pxComparable
{
    const pxComparableVt *pComparableVt;
} pxComparable;

extern const char pxComparableName[];

#endif // PXCOMPARABLE_H
