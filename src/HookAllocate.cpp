#include <dlfcn.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

//#include "ManageClass.hpp"
#include <iostream>

#include "AddrConfilm.hpp"

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
/*
g++ -fPIC -shared ManageClass.cpp  -std=c++20 -o libManage.o
g++ -fPIC -shared HookAllocate.cpp  libManage.o  -o hook.so -rdynamic -ldl -std=c++20
*/

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

typedef struct mapedUnit
{
    void *allocedPtr;
    void *returnAddress;
} mapedUnit;

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

FILE *fopen(const char *str, const char *mode)
{
    printf("[hooked fopen(%p)]%s %s\n", ORG_OPEN, str, mode);
    return ORG_OPEN(str, mode);
}

int fclose(FILE *fp)
{
    printf("[hooked] fclose (%d)\n", fp);
    return ORG_CLOSE(fp);
}

int printf(const char *fmt, ...)
{
    // パラメータパックを ... で展開して、
    // 関数g()の引数として渡す
    ORG_PRINTF("[HookLibs]:");

    va_list ap;
    // 可変長引数を１個の変数にまとめる
    va_start(ap, fmt);
    // まとめられた変数で処理する
    int ret = vprintf(fmt, ap);
    va_end(ap);
    return ret;
};

/*extern "C"
{
    void *malloc(size_t __size) noexcept;
    void free(void *p) noexcept;
}
*/

/*
void *malloc(size_t _size) noexcept
{
    void *res = ORG_MALLOC(_size);
    printf("alloc this size\t:[%p] %5lu byte\n", res, _size);
    return res;
}
*/
/*
void free(void *p) noexcept
{
    printf("free memory\t:[%p]\n", p);
    return ORG_FREE(p);
}
*/

void *
operator new(size_t n)
{
    Dl_info info;
    void *retAddr = __builtin_return_address(0);
    dladdr(retAddr, &info);
    void *p = malloc(n);

    printf("[%s]hook new operator alloc size :%lu[%p]\n", info.dli_sname, n, p);
    MemManage::_pManager.addMap(p, retAddr);
    return p;
}
void *operator new[](size_t n)
{
    void *retAddr = __builtin_return_address(0);
    Dl_info info;
    dladdr(retAddr, &info);
    void *p = malloc(n);
    printf("[%s]hook new operator[] alloc size :%lu[%p]\n", info.dli_sname, n, p);
    MemManage::_pManager.addMap(p, retAddr);
    return p;
}
void operator delete(void *p)
{
    Dl_info info;
    dladdr(__builtin_return_address(0), &info);
    printf("[%s]hook delete operator : [%p](%s)\n", info.dli_sname, p, ValidateAddr(p) ? "validate" : "Invalid");
    MemManage::_pManager.removeMap(p);
    free(p);
}
void operator delete[](void *p)
{
    Dl_info info;
    dladdr(__builtin_return_address(0), &info);
    printf("[%s]hook delete[] operator : [%p]\n", info.dli_sname, p);
    MemManage::_pManager.removeMap(p);
    free(p);
}

void MemManage::pManager::addMap(void *p, void *retAddr)
{
    // void *(*NewFunc)(size_t) = operator new;

    mapedUnit *arr = ptr + counter;
    mapedUnit tmp = {p, retAddr};
    memcpy(arr, &tmp, sizeof(mapedUnit));
    counter++;
};

void MemManage::pManager::removeMap(void *p)
{
    for (int i = counter; i > 0; i--)
    {
        mapedUnit *arr = ptr + i;
        if (arr->allocedPtr == p)
        {
            printf("🌟detect alloced pointor\n");
            memset(arr, 0x00, sizeof(mapedUnit));
        }
        else
        {
            // nop
        }
    }
};

MemManage::pManager::pManager()
{
    counter = 0;
    ptr = (mapedUnit *)malloc(sizeof(mapedUnit) * 255);
    memset(ptr, 0x00, sizeof(sizeof(mapedUnit) * 255));
    puts("[start]Memory management------------");
}

MemManage::pManager::~pManager()
{

    puts("[end]Memory management------------\nthese pointor is leaked");

    mapedUnit tmp;
    memset(&tmp, 0x00, sizeof(mapedUnit));

    for (int i = 0; i < counter; i++)
    {
        mapedUnit *arr = ptr + i;
        if (!memcmp(arr, &tmp, sizeof(mapedUnit)))
        {
            continue;
        }
        Dl_info info;
        dladdr(arr->returnAddress, &info);
        printf("[%p](%s) %p\n", arr->allocedPtr, info.dli_sname, arr->returnAddress);
        free(arr->allocedPtr);
    }
    free(ptr);
};
