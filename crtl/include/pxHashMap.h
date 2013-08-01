
#ifndef PXHASHMAP_H
#define PXHASHMAP_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

#ifndef PX_STDBOOL_H
#include <stdbool.h>
#define PX_STDBOOL_H
#endif


struct pxHashable;

struct pxHashMap;
typedef struct
{
    pxInterfaceVt interfaceVt;

    bool (*isEmpty)(struct pxHashMap *pI);

    // NOTE: the key must also support pxComparable
    pxInterface *(*get)(struct pxHashMap *pI, struct pxHashable *pKey);

    // NOTE: the key must also support pxComparable
    pxInterface *(*put)(
        struct pxHashMap *pI, struct pxHashable *pKey, pxInterface *pO);
} pxHashMapVt;

typedef struct pxHashMap
{
    const pxHashMapVt *pVt;
} pxHashMap;

extern const char pxHashMapName[];

#define PXHASHMAP_isEmpty(pI) \
    ((*(pI)->pVt->isEmpty)(pI))

#define PXHASHMAP_get(pI, pKey) \
    ((*(pI)->pVt->get)(pI, pKey))

#define PXHASHMAP_put(pI, pKey, pO) \
    ((*(pI)->pVt->put)(pI, pKey, pO))

#endif // PXHASHMAP_H
