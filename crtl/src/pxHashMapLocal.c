
#ifndef PXHASHMAPLOCAL_H
#include "pxHashMapLocal.h"
#endif

#ifndef PXALLOC_H
#include "pxAlloc.h"
#endif

#ifndef PXHASHMAP_H
#include "pxHashMap.h"
#endif

#ifndef PXOBJECT_H
#include "pxObject.h"
#endif


typedef struct
{
    const pxHashMapVt *pHashMapVt;
    pxObjectStruct objectStruct;
} pxHashMapLocal_s;


static const pxHashMapVt pxHashMapLocalHashMapVt =
{
    {
        offsetof(pxHashMapLocal_s, objectStruct.pObjectVt) -
        offsetof(pxHashMapLocal_s, pHashMapVt),
        pxObject_getInterface,
    },
};

static const pxObjectLookup pxHashMapLocal_lookup[] =
{
    {pxHashMapName,
     offsetof(pxHashMapLocal_s, objectStruct.pObjectVt) -
     offsetof(pxHashMapLocal_s, pHashMapVt)},
    {pxObjectName, 0},
};

static const pxObjectVt pxHashMapLocalObjectVt =
{
    {
        0,
        pxObject_getInterface,
    },
    sizeof(pxHashMapLocal_lookup)/sizeof(pxHashMapLocal_lookup[0]),
    pxHashMapLocal_lookup,
    pxObject_destroy,
};

pxHashMap *pxHashMapLocalCreate(pxAlloc *pAlloc, pxInterface *pOwner)
{
    pxHashMapLocal_s *const pO =
        PXALLOC_alloc(pAlloc, sizeof(pxHashMapLocal_s), PXALLOC_F_DIRTY);
    pO->pHashMapVt = &pxHashMapLocalHashMapVt;
    pxObjectStructInit(&pO->objectStruct, &pxHashMapLocalObjectVt, pOwner);

    return (pxHashMap *)&pO->pHashMapVt;
}
