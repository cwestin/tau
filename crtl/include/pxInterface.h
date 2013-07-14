
#ifndef PXINTERFACE_H
#define PXINTERFACE_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif

typedef struct
{
    struct pxInterface (*getInterface)(const struct pxInterface *const pThis,
                                       const char *const pName);
} pxInterfaceVt;

typedef struct pxInterface
{
    const pxInterfaceVt *pVt;
} pxInterface;

#define pxInterfaceObject(pI, sname, vtname) \
    ((sname *)(((char *)pI) - offsetof(sname, vtname)))

#endif // PXINTERFACE_H
