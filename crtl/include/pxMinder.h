
#ifndef PXMINDER_H
#define PXMINDER_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


typedef struct pxMinderVt
{
    pxInterfaceVt interfaceVt;

    struct pxObject *(*register)(struct pxMinderVt *const *const pMinder, struct pxObject *pManaged);
} pxMinderVt, *const pxMinder;

extern const char pxMinderName[];

pxMinder *pxMinderCreate();

#endif // PXMINDER_H
