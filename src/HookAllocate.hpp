#include <dlfcn.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

//#include "ManageClass.hpp"
#include <iostream>

typedef void *(*TMALLOC)(size_t _size);
typedef void (*TFREE)(void *p);
typedef int (*TPRINTF)(const char *, ...);
typedef FILE *(*TOPEN)(const char *, const char *);
typedef int (*TFCLOSE)(FILE *);

const static TMALLOC ORG_MALLOC = (TMALLOC)dlsym(RTLD_NEXT, "malloc");
const static TFREE ORG_FREE = (TFREE)dlsym(RTLD_NEXT, "free");
const static TPRINTF ORG_PRINTF = (TPRINTF)dlsym(RTLD_NEXT, "printf");
const static TOPEN ORG_OPEN = (TOPEN)dlsym(RTLD_NEXT, "fopen");
const static TFCLOSE ORG_CLOSE = (TFCLOSE)dlsym(RTLD_NEXT, "fclose");

typedef struct mapedUnit
{
    void *allocedPtr;
    void *returnAddress;
} mapedUnit;

namespace HOOK
{
    struct _hook
    {
        _hook()
        {
            printf("Start Hook Lib--------------\n");
        }
        ~_hook()
        {
            printf("Finish Hook Lib-------------\n");
        }
    };
    _hook tmp;

};

namespace MemManage
{

    class pManager
    {
    public:
        void addMap(void *p, void *retAddr);
        void removeMap(void *p);
        pManager();
        ~pManager();
        mapedUnit *ptr;
        int counter = 0;
    };
    pManager _pManager;
}
