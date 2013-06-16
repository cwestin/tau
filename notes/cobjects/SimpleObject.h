/*
Simple C object.

In some ways better than C++, since we have real encapsulation: no
compilation dependency on size, and true opacity: no visibility into the
implementation at all.
 */

typedef struct SimpleObject SimpleObject;

SimpleObject *SimpleObjectCreate(int a);

void SimpleObjectMethodA(SimpleObject *pThis, int x);

void SimpleObjectDestroy(SimpleObject *pThis);
