/*
Implementation of an object implementing AbstractInterface
*/

#include <stddef.h>
#include <stdlib.h>
#include "ConcreteObject.h"

typedef struct
{
    const AbstractInterface_vt *pvt;
    int fieldX;
} ConcreteObject;

#define getObject(pt) ((ConcreteObject *)(((char *)(pt)) - offsetof(ConcreteObject, pvt)))

static void methodA(AbstractInterface *pt, int arg)
{
    ConcreteObject *pThis = getObject(pt);
    ++pThis->fieldX;
}

static const AbstractInterface_vt abstractInterface_vt =
{
    methodA
};

AbstractInterface *ConcreteObjectCreate()
{
    ConcreteObject *pThis = (ConcreteObject *)malloc(sizeof(ConcreteObject));
    pThis->pvt = &abstractInterface_vt;
    pThis->fieldX = 0;
    return &pThis->pvt;
}

void ConcreteObjectStaticMethod(AbstractInterface *pt)
{
    ConcreteObject *pThis = getObject(pt);
    ++pThis->fieldX;
}

void ConcreteObjectDestroy(AbstractInterface *pt)
{
    ConcreteObject *pThis = getObject(pt);
    free(pThis);
}
