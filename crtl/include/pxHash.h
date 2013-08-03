
#ifndef PXHASH_H
#define PXHASH_H

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif


typedef int pxHashValue;

static inline void pxHashInit(pxHashValue *pHash)
{
    *pHash = 0xca5015ac;
}

void pxHashVoid(pxHashValue *pHash, const void *p, size_t length);

void pxHashString(pxHashValue *pHash, const char *pc);

static inline void pxHashInt(pxHashValue *pHash, int v)
{
    pxHashVoid(pHash, &v, sizeof(v));
}

#endif // PXHASH_H
