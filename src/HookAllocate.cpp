#include "HookAllocate.hpp"
#include "AddrConfilm.hpp"
#include "RAIILockClass.hpp"

#include <thread>
/*
# build cmd for Ubuntu
g++ -fPIC -shared ../src/HookAllocate.cpp -o hook.so -rdynamic -ldl -std=c++2a
g++ ../src/test.cpp -rdynamic -ldl -lpthread -std=c++2a main.out
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

void *operator new(size_t size)
{
    void *retAddr = __builtin_return_address(0);
    void *p = malloc(size);

    printf("[new   (size:%4lu)][called: %s]:[%p]\n",
           size,
           ConvRetAddrToDmglFuncName(retAddr),
           p);
    MemManage::_pManager.addMap(p, retAddr);
    return p;
}
void *operator new[](size_t size)
{
    void *retAddr = __builtin_return_address(0);
    void *p = malloc(size);
    printf("[new[] (size:%4lu)][called: %s]:[%p]\n",
           size,
           ConvRetAddrToDmglFuncName(retAddr),
           p);
    MemManage::_pManager.addMap(p, retAddr);
    return p;
}
void operator delete(void *p)
{
    void *retAddr = __builtin_return_address(0);
    char *allocedFunc = MemManage::_pManager.removeMap(p);
    printf("[delete  ][called: %s]: [%p](allocated: %s)\n",
           ConvRetAddrToDmglFuncName(retAddr),
           p,
           allocedFunc);
    free(p);
}
void operator delete[](void *p)
{
    void *retAddr = __builtin_return_address(0);
    char *allocedFunc = MemManage::_pManager.removeMap(p);
    printf("[delete[]][called: %s]: [%p](allocated: %s)\n",
           ConvRetAddrToDmglFuncName(retAddr),
           p,
           allocedFunc);
    free(p);
}

void MemManage::pManager::addMap(void *p, void *retAddr)
{
    static_mutex::scoped_lock lk(m);

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

char *MemManage::pManager::removeMap(void *p)
{
    static_mutex::scoped_lock lk(m);

    mapedUnit *endPtr = ptr + (counter - 1);
    char *str = (char *)"this pointor not found";
    for (int i = counter - 1; 0 <= i; i--)
    {
        mapedUnit *arr = ptr + i;

        //　受け取ったポインタが格納したポインタと同じだった場合
        if (arr->allocedPtr == p)
        {
            str = (char *)ConvRetAddrToDmglFuncName(arr->returnAddress);
            // 末尾のアドレスと同じじゃない場合、末尾の中身で書き換える
            if (endPtr != arr)
            {
                memmove(arr, arr + 1, sizeof(mapedUnit) * (counter - 1 - i));
            }
            // 末尾の構造体を空にする
            memset(endPtr, 0x00, sizeof(mapedUnit));
            counter--;
            break;
        }
    }
    return str;
};

MemManage::pManager::pManager()
{
    counter = 0;
    ptr = (mapedUnit *)malloc(mapSize);
    memset(ptr, 0x00, mapSize);
    printf("[start]Memory management------------\n");
}

MemManage::pManager::~pManager()
{

    printf("[end]Memory management------------\n"
           "🌟these pointor is leaked [thread] %d\n",
           std::thread::hardware_concurrency());

    /* std::thread::hardware_concurrency()  */
    displayAllcMap(true);

    printf("-----------------------------------\n");
    free(ptr);
};

void MemManage::pManager::displayAllcMap(bool isExecFlee)
{
    static_mutex::scoped_lock lk(m);

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

        printf("[index: %4d][allocated:%s (%p)](%p)\n",
               arr->index,
               ConvRetAddrToDmglFuncName(arr->returnAddress),
               arr->returnAddress,
               arr->allocedPtr);

        // 一応、freeしておく
        if (isExecFlee)
        {
            free(arr->allocedPtr);
        }
    }
}

const char *ConvRetAddrToDmglFuncName(void *returnAddress)
{
    Dl_info info;
    dladdr(returnAddress, &info);
    int status;
    char *demangled = abi::__cxa_demangle(info.dli_sname, 0, 0, &status);

    return (demangled != nullptr) ? demangled : "main()";
}