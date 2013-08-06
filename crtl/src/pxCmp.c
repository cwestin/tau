
#ifndef PXCMP_H
#include "pxCmp.h"
#endif


int pxCmpInt(const void *pL, const void *pR)
{
    if (*(const int *)pL < *(const int *)pR)
        return -1;
    if (*(const int *)pL > *(const int *)pR)
        return 1;

    return 0;
}
