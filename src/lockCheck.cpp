#include <thread>
#include <mutex>
#include <iostream>

#include "RAIILockClass.hpp"

int g_i = 0;

void safe_increment()
{
    static_mutex::scoped_lock lk(::m);
    /**/
    ++g_i;

    std::cout << "g_i: " << g_i << "; in thread #"
              << std::this_thread::get_id();

    printf("\n");
    // printf("g_i: %d; in thread #\n", g_i);
    //  g_i_mutex is automatically released when lock
    //  goes out of scope
}

int main()
{
    std::cout << "g_i: " << g_i << "; in main()\n";

    std::thread t1(safe_increment);
    std::thread t2(safe_increment);
    std::thread t3(safe_increment);
    std::thread t4(safe_increment);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::cout << "g_i: " << g_i << "; in main()\n";
    assert(g_i == 4);
}