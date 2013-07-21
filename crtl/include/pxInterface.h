
#ifndef PXINTERFACE_H
#define PXINTERFACE_H

#ifndef PX_STDDEF_H
#include <stddef.h>
#define PX_STDDEF_H
#endif


typedef struct pxInterfaceVt
{
    int pxObjectOffset; // offset of pxObject interface from this interface
    struct pxInterfaceVt *const *const (*getInterface)(
        struct pxInterfaceVt *const *const pThis,
        const char *const pName);
} pxInterfaceVt, *const pxInterface;


#define PXINTERFACE_getInterface(pI, iName) \
    ((iName *)((*(*(pI))->interfaceVt.getInterface)((pxInterface *const)(pI), iName ## Name)))


#define PXINTERFACE_STRUCT(pI, sname, vtname) \
    ((sname *)(((char *)(pI)) - offsetof(sname, vtname)))

#endif // PXINTERFACE_H
