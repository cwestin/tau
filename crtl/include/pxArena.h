
#ifndef PXARENA_H
#define PXARENA_H

#ifndef PXINTERFACE_H
#include "pxInterface.h"
#endif


struct pxArena;

typedef struct
{
    PXINTERFACE_GET(pxArena);
} pxArenaVt;

typedef struct pxArena
{
    const pxArenaVt *pVt;
} pxArena;

extern const char pxArenaName[];

#endif // PXARENA_H
