
#ifndef PXCOMPARATOR_H
#define PXCOMPARATOR_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


struct pxComparator;
typedef struct
{
    pxInterfaceVt intefaceVt;

    int (*compare)(struct pxComparator *pI,
                   struct pxInterface *pL, struct pxInterface *pR);
} pxComparatorVt;

typedef struct pxComparator
{
    const pxComparatorVt *pComparatorVt;
} pxComparator;

extern const char pxComparatorName[];

#endif // PXCOMPARATOR_H
