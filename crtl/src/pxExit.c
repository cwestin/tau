
#ifndef PXEXIT_H
#include "pxExit.h"
#endif

#ifndef PX_STDARG_H
#include <stdarg.h>
#define PX_STDARG_H
#endif

#ifndef PX_STDIO_H
#include <stdio.h>
#define PX_STDIO_H
#endif


void pxExit(const char *ps, ...)
{
    va_list argp;

    va_start(argp, ps);
    vfprintf(stderr, ps, argp);
    va_end(argp);

    exit(1);
}
