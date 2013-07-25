
#ifndef PXINTERFACE_H
#define PXINTERFACE_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif


struct pxInterface;
typedef struct pxInterfaceVt
{
    int pxObjectOffset; // offset of pxObject interface from this interface
    struct pxInterface *(*getInterface)(
        struct pxInterface *pThis, const char *const pName);
} pxInterfaceVt;

typedef struct pxInterface
{
    const pxInterfaceVt *const pVt;
} pxInterface;

#define PXINTERFACE_getInterface(pI, iName) \
    ((iName *)((*(pI)->pVt->interfaceVt.getInterface)((pxInterface *const)(pI), iName ## Name)))


#define PXINTERFACE_STRUCT(pI, sname, vtname) \
    ((sname *)(((char *)(pI)) - offsetof(sname, vtname)))

#endif // PXINTERFACE_H
