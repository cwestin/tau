/*
C Abstract interface.

We're basically building our own C++-style vtable.
*/

typedef struct AbstractInterface_vt
{
    int (*methodA)(const struct AbstractInterface_vt **ppThis, int arg);
} AbstractInterface_vt;

typedef const AbstractInterface_vt *AbstractInterface;
