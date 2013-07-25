
#ifndef PXMINDER_H
#define PXMINDER_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

struct pxObject;
struct pxMemory;


struct pxMinder;
typedef struct pxMinderVt
{
    pxInterfaceVt interfaceVt;

    struct pxObject *(*_register)(
        struct pxMinder *pMinder, struct pxObject *pManaged);
} pxMinderVt;

typedef struct pxMinder
{
    const pxMinderVt *const pVt;
} pxMinder;

extern const char pxMinderName[];

#define PXMINDER_register(pI, pManaged) \
    ((*(*(pI))->_register)(pI, pManaged))

pxMinder *pxMinderCreate(struct pxMemory *pArena);

#endif // PXMINDER_H
