#include <dlfcn.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "ManageClass.hpp"

typedef void *(*TMALLOC)(size_t _size);
typedef void (*TFREE)(void *p);
typedef int (*TPRINTF)(const char *, ...);

const static TMALLOC ORG_MALLOC = (TMALLOC)dlsym(RTLD_NEXT, "malloc");
const static TFREE ORG_FREE = (TFREE)dlsym(RTLD_NEXT, "free");
const static TPRINTF ORG_PRINTF = (TPRINTF)dlsym(RTLD_NEXT, "printf");

/*
g++ -fPIC -shared ManageClass.cpp  -std=c++20 -o libManage.o
g++ -fPIC -shared HookAllocate.cpp  libManage.o  -o hook.so -rdynamic -ldl -std=c++20
*/

namespace Hook
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
    bool WatchStart = false;

};
void initWatch()
{
    Hook::WatchStart = true;
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

void *(*NewFunc)(size_t) = operator new;
void *(*NewArrFunc)(size_t) = operator new[];

void *operator new(size_t n)
{
    Dl_info info;
    dladdr(__builtin_return_address(0), &info);
    // dladdr((void *)NewFunc, &info);
    void *p = malloc(n);

    printf("[%s]hook new operator alloc size :%lu[%p]\n", info.dli_sname, n, p);
    // MemManage::_pManager.addMap(p, info.dli_sname);
    return p;
}
void *operator new[](size_t n)
{
    Dl_info info;
    Dl_info info2;
    dladdr(__builtin_return_address(0), &info);
    dladdr((void *)NewFunc, &info2);
    void *p = malloc(n);
    printf("[%s][%s]hook new operator[] alloc size :%lu[%p]\n", info.dli_sname, info2.dli_sname, n, p);
    MemManage::_pManager.addMap(p, info.dli_sname);
    return p;
}
void operator delete(void *p)
{
    Dl_info info;
    dladdr(__builtin_return_address(0), &info);
    printf("[%s]hook delete operator : [%p]\n", info.dli_sname, p);
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
