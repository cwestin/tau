
#ifndef PXALIGN_H
#define PXALIGN_H

#ifndef PX_STDLIB_H
#include <stdlib.h>
#define PX_STDLIB_H
#endif


struct pxAlignUnknown;
typedef union
{
    char c;
    short s;
    long l;
    int i;
    struct pxAlignUnknown *pU;
    void *p;
} pxAlignAll;

typedef struct
{
    char c;
    pxAlignAll alignAll;
} pxAligner;


// this assumes that worst alignment must be a power of 2
// which seems reasonable, given hardware bus layout requirements
#define PXALIGN_WORST offsetof(pxAligner, alignAll)
#define PXALIGN_WORST_MASK (PXALIGN_WORST - 1)

#define PXALIGN_SIZE(p) \
    (((p) + PXALIGN_WORST_MASK) & (~PXALIGN_WORST_MASK))

#endif // PXALIGN_H
