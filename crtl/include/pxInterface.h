
#ifndef PXINTERFACE_H
#define PXINTERFACE_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif

struct pxInterface;

#define PXINTERFACE_GET(iName) \
    struct pxInterface *(*getInterface)(const struct iName *const pThis, const char *const pName)

typedef struct
{
    PXINTERFACE_GET(pxInterface);
} pxInterfaceVt;

typedef struct pxInterface
{
    const pxInterfaceVt *pVt;
} pxInterface;

#define PXINTERFACE_OBJECT(pI, sname, vtname) \
    ((sname *)(((char *)(pI)) - offsetof(sname, vtname)))

#define PXINTERFACE_getInterface(pO, iName) \
    ((iName *)((*(pO)->pVt->getInterface)((pO), iName ## Name)))

#endif // PXINTERFACE_H
