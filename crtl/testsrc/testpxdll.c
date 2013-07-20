
#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif

#ifndef PXDLL_H
#include "pxDll.h"
#endif

static void validate(const pxDllHead *pHead)
{
    if (!pxDllIsValid(pHead))
        fprintf(stderr, "testpxDll: invalid list\n");
}

typedef struct
{
    int x;
    pxDllLink link;
} IntStruct;

static void testpxDll()
{
    pxDllHead head;
    pxDllInit(&head);
    validate(&head);

    const int nInts = 10;
    for(int i = 0; i < nInts; ++i)
    {
        IntStruct *pI = (IntStruct *)malloc(sizeof(IntStruct));
        pxDllInit(&pI->link);
        pI->x = i;
        pxDllAddLast(&head, &pI->link);
        validate(&head);
    }

    int i = 0;
    for(; !pxDllIsEmpty(&head); ++i)
    {
        pxDllLink *pLink = pxDllGetFirst(&head);
        if (pLink == NULL)
            break;

        IntStruct *pI = pxDllGetStruct(pLink, IntStruct, link);
        if (pI->x != i)
            fprintf(stderr, "testpxdll: invalid value in list\n");

        pxDllRemove(pLink);
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
    testpxDll();
    return 0;
}
