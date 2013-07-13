
#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif

#ifndef PXDLL_H
#include "pxdll.h"
#endif

static void validate(const pxdllHead *pHead)
{
    if (!pxdllIsValid(pHead))
        fprintf(stderr, "testpxdll: invalid list\n");
}

typedef struct
{
    int x;
    pxdllLink link;
} IntStruct;

static void testpxdll()
{
    pxdllHead head;
    pxdllInit(&head);
    validate(&head);

    const int nInts = 10;
    for(int i = 0; i < nInts; ++i)
    {
        IntStruct *pI = (IntStruct *)malloc(sizeof(IntStruct));
        pxdllInit(&pI->link);
        pI->x = i;
        pxdllAddLast(&head, &pI->link);
        validate(&head);
    }

    int i = 0;
    for(; !pxdllIsEmpty(&head); ++i)
    {
        pxdllLink *pLink = pxdllGetFirst(&head);
        if (pLink == NULL)
            break;

        IntStruct *pI = pxdllGetStruct(pLink, IntStruct, link);
        if (pI->x != i)
            fprintf(stderr, "testpxdll: invalid value in list\n");

        pxdllRemove(pLink);
        free(pI);

        validate(&head);
    }

    if (i != nInts)
        fprintf(stderr,
                "testpxdll: didn't see all ints in list (i = %d, nInts = %d)\n",
                i, nInts);
}

int main(void)
{
    testpxdll();
    return 0;
}
