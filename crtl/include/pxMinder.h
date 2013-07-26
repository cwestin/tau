
#ifndef PXMINDER_H
#define PXMINDER_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif

struct pxObject;
struct pxAlloc;


struct pxMinder;
typedef struct pxMinderVt
{
    pxInterfaceVt interfaceVt;

    // the underscore gets around this being a reserved word
    struct pxObject *(*_register)(
        struct pxMinder *pMinder, struct pxObject *pManaged);
} pxMinderVt;

typedef struct pxMinder
{
    const pxMinderVt *const pVt;
} pxMinder;

extern const char pxMinderName[];

#define PXMINDER_register(pI, pManaged) \
    ((*(pI)->pVt->_register)((pI), (pManaged)))

pxMinder *pxMinderCreate(struct pxAlloc *pAlloc);

#endif // PXMINDER_H
