
#ifndef PXHASHMAP_H
#define PXHASHMAP_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


struct pxHashable;

struct pxHashMap;
typedef struct
{
    pxInterfaceVt interfaceVt;

    // NOTE: the key must also support pxComparable
    pxInterface *(*get)(struct pxHashMap *pI, struct pxHashable *pKey);

    // NOTE: the key must also support pxComparable
    pxInterface *(*put)(
        struct pxHashMap *pI, struct pxHashable *pKey, pxInterface *pO);
} pxHashMapVt;

typedef struct pxHashMap
{
    const pxHashMapVt *pxHashMapVt;
} pxHashMap;

extern const char pxHashMapName[];

#define PXHASHMAP_get(pI, pKey) \
    ((*(pI)->pVt->get)(pI, pKey))

#define PXHASHMAP_put(pI, pKey, pO) \
    ((*(pI)->pVt->get)(pI, pKey, pO)

#endif // PXHASHMAP_H
