
#ifndef PXALLOCDEBUG_H
#define PXALLOCDEBUG_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

#ifndef PX_STDBOOL_H
#include <stdbool.h>
#define PX_STDBOOL_H
#endif


struct pxAllocDebug;
typedef struct pxAllocDebugVt
{
    pxInterfaceVt interfaceVt;

    bool (*isEmpty)(struct pxAllocDebug *pAllocDebug);
    unsigned (*countPieces)(struct pxAllocDebug *pAllocDebug);
} pxAllocDebugVt;

typedef struct pxAllocDebug
{
    const pxAllocDebugVt *const pVt;
} pxAllocDebug;

extern const char pxAllocDebugName[];

#define PXALLOCDEBUG_isEmpty(pI) \
    ((*(pI)->pVt->isEmpty)(pI))
#define PXALLOCDEBUG_countPieces(pI) \
    ((*(pI)->pVt->countPieces)(pI))

// largely for testing and debugging
struct pxAlloc *pxAllocDebugCreate(
    struct pxAlloc *pAlloc, struct pxInterface *pOwner);

#endif // PXALLOCDEBUG_H
