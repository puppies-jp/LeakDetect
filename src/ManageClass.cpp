#include "ManageClass.hpp"
#include "AddrConfilm.hpp"
#include <cxxabi.h>

/*
g++ AddrConfilm.cpp -shared -o libAddr.so
g++ -fPIC -shared  ManageClass.cpp AddrConfilm.cpp  -std=c++20 -o libManage.o
g++ -fPIC -shared HookAllocate.cpp  libManage.o  -o hook.so -rdynamic -ldl -std=c++20

DYLD_INSERT_LIBRARIES=hook.so  DYLD_FORCE_FLAT_NAMESPACE=YES ./a.out
*/

namespace MemManage
{
    void pManager::addMap(void *p, const char *str)
    {
        // void *(*NewFunc)(size_t) = operator new;

        dictionary.insert(std::make_pair(
            p,
            str));
    };

    void pManager::removeMap(void *p)
    {
        dictionary.erase(p);
    };
    pManager::~pManager()
    {

        for (const auto &[key, value] : dictionary)
        {

            printf("Don`t released [%p]%s (%s)\n", key, value, ValidateAddr(key) ? "Validate" : "Invalid");
        }
    };

};