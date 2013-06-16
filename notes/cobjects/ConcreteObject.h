/*
Declaration of an object implementing AbstractInterface
*/

#include "AbstractInterface.h"

AbstractInterface *ConcreteObjectCreate();

void ConcreteObjectStaticMethod(AbstractInterface *pThis);

void ConcreteObjectDestroy(AbstractInterface *pThis);
