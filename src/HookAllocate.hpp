#include <dlfcn.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <cxxabi.h>
#include <cstring>
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

/* allocateしたアドレスを保存する構造体 */
typedef struct mapedUnit
{
    unsigned int index;  /* 採番 */
    void *allocedPtr;    /* allocated pointor */
    void *returnAddress; /* return Addr */
} mapedUnit;

const char *ConvRetAddrToDmglFuncName(void *returnAddress);

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
        char *removeMap(void *p);
        void displayAllcMap(bool isExecFlee = false);
        pManager();
        ~pManager();

    private:
        mapedUnit *ptr;
        int counter = 0;
        unsigned int index = 0;
        int mapSize = sizeof(mapedUnit) * 255;
    };
    pManager _pManager;
}
