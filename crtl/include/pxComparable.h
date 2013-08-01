
#ifndef PXCOMPARABLE_H
#define PXCOMPARABLE_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


struct pxComparable;
typedef struct pxComparableVt
{
    pxInterfaceVt intefaceVt;

    int (*compare)(struct pxComparable *pI, struct pxInterface *pOther);
} pxComparableVt;

typedef struct pxComparable
{
    const pxComparableVt *pVt;
} pxComparable;

extern const char pxComparableName[];

#define PXCOMPARABLE_compare(pI, pOther) \
    ((*(pI)->pVt->compare)(pI, pOther))

#endif // PXCOMPARABLE_H
