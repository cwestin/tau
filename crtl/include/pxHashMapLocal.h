
#ifndef PXHASHMAPLOCAL_H
#define PXHASHMAPLOCAL_H


struct pxAlloc;
struct pxInterface;
struct pxHashMap *pxHashMapLocalCreate(
    struct pxAlloc *pAlloc, struct pxInterface *pOwner);

#endif // PXHASHMAPLOCAL_H
