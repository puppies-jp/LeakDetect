#include "HookAllocate.hpp"
#include "AddrConfilm.hpp"

/*
g++ -fPIC -shared ManageClass.cpp  -std=c++20 -o libManage.o
g++ -fPIC -shared HookAllocate.cpp  libManage.o  -o hook.so -rdynamic -ldl -std=c++20
*/

FILE *fopen(const char *str, const char *mode)
{
    printf("[hooked fopen(%p)]%s %s\n", ORG_OPEN, str, mode);
    return ORG_OPEN(str, mode);
}

int fclose(FILE *fp)
{
    printf("[hooked] fclose (%p)\n", fp);
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
    void *retAddr = __builtin_return_address(0);
    void *p = malloc(n);

    printf("[%s]hook new operator alloc size :%lu[%p]\n",
           ConvRetAddrToDmglFuncName(retAddr),
           n, p);
    MemManage::_pManager.addMap(p, retAddr);
    return p;
}
void *operator new[](size_t n)
{
    void *retAddr = __builtin_return_address(0);
    void *p = malloc(n);
    printf("[%s]hook new operator[] alloc size :%lu[%p]\n",
           ConvRetAddrToDmglFuncName(retAddr),
           n, p);
    MemManage::_pManager.addMap(p, retAddr);
    return p;
}
void operator delete(void *p)
{
    void *retAddr = __builtin_return_address(0);
    printf("[%s]hook delete operator : [%p](%s)\n",
           ConvRetAddrToDmglFuncName(retAddr),
           p,
           ValidateAddr(p) ? "validate" : "Invalid");
    MemManage::_pManager.removeMap(p);
    free(p);
}
void operator delete[](void *p)
{
    void *retAddr = __builtin_return_address(0);
    printf("[%s]hook delete[] operator : [%p]\n",
           ConvRetAddrToDmglFuncName(retAddr),
           p);
    MemManage::_pManager.removeMap(p);
    free(p);
}

void MemManage::pManager::addMap(void *p, void *retAddr)
{
    // void *(*NewFunc)(size_t) = operator new;

    mapedUnit *arr = ptr + counter;
    mapedUnit tmp = {index, p, retAddr};
    memcpy(arr, &tmp, sizeof(mapedUnit));
    counter++;
    index++;
    if (counter > mapSize)
    {
        mapSize += mapSize;
        mapedUnit *pTmp = (mapedUnit *)realloc(ptr, mapSize);

        if (pTmp == nullptr)
        {
            exit(1);
        }
        else if (ptr != pTmp)
        {
            free(ptr);
            ptr = pTmp;
        }
    }
};

void MemManage::pManager::removeMap(void *p)
{
    mapedUnit *endPtr = ptr + counter;
    for (int i = counter; i > 0; i--)
    {
        mapedUnit *arr = ptr + i;

        //　受け取ったポインタが格納したポインタと同じだった場合
        if (arr->allocedPtr == p)
        {
            // 末尾のアドレスと同じじゃない場合、末尾の中身で書き換える
            if (endPtr != arr)
            {
                memcpy(arr, endPtr, sizeof(mapedUnit));
            }
            // 末尾の構造体を空にする
            memset(endPtr, 0x00, sizeof(mapedUnit));
            counter--;
        }
    }
};

MemManage::pManager::pManager()
{
    counter = 0;
    ptr = (mapedUnit *)malloc(mapSize);
    memset(ptr, 0x00, mapSize);
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

        // 空だった場合、continueする
        if (!memcmp(arr, &tmp, sizeof(mapedUnit)))
        {
            continue;
        }

        printf("[index: %4d][%s (%p)](%p)\n",
               arr->index,
               ConvRetAddrToDmglFuncName(arr->returnAddress),
               arr->returnAddress,
               arr->allocedPtr);
        free(arr->allocedPtr);
    }
    free(ptr);
};

const char *ConvRetAddrToDmglFuncName(void *returnAddress)
{
    Dl_info info;
    dladdr(returnAddress, &info);
    int status;
    char *demangled = abi::__cxa_demangle(info.dli_sname, 0, 0, &status);

    return (demangled == nullptr) ? "main()" : demangled;
}