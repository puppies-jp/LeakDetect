#pragma once

#ifndef MMANAGER
#define MMANAGER

#include <iostream>
#include <map>

namespace MemManage
{

    class pManager
    {
    public:
        void addMap(void *p, const char *str);
        void removeMap(void *p);
        ~pManager();

    private:
        std::map<void *, const char *> dictionary = {};
    };
    pManager _pManager;
}

#endif