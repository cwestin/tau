
#include <stdlib.h>
#include "SimpleObject.h"

struct SimpleObject
{
    int field1;
};

SimpleObject *SimpleObjectCreate(int a)
{
    SimpleObject *pThis = malloc(sizeof(SimpleObject));
    pThis->field1 = a;
    return pThis;
}

static void privateMethod(SimpleObject *pThis)
{
}

void SimpleObjectMethodA(SimpleObject *pThis, int x)
{
    privateMethod();
    pThis->field1 += x;
}

void SimpleObjectDestroy(SimpleObject *pThis)
{
    free(pThis);
}
