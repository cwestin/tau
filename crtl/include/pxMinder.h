
#ifndef PXMINDER_H
#define PXMINDER_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


struct pxMinder;

typedef struct
{
    PXINTERFACE_GET(pxMinder);

    struct pxObject *(*register)(struct pxMinder *pMinder, struct pxObject *pManaged);
} pxMinderVt;

typedef struct pxMinder
{
    const pxMinderVt *pVt;
} pxMinder;

extern const char pxMinderName[];

pxMinder *pxMinderCreate();

#endif // PXMINDER_H
