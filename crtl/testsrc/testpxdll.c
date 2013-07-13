
#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif

#ifndef PXDLL_H
#include "pxdll.h"
#endif

static void testpxdll()
{
    pxdllHead head;
    pxdllInit(&head);

    if (!pxdllIsValid(&head))
        fprintf(stderr, "testpxdll: invalid list after initialization\n");
}

int main(void)
{
    testpxdll();
    return 0;
}
