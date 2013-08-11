/*
  tau - http://github.com/cwestin/tau
  Copyright 2013 Chris Westin

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

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

        IntStruct *pI = PXDLL_STRUCT(pLink, IntStruct, link);
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
