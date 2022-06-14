#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <typeinfo>
#include <tuple>
#include <map>
#include <thread>

#define STRINGIFY(n) #n
#define TOSTRING(n) STRINGIFY(n)
#define PREFIX __FILE__ ":" TOSTRING(__LINE__) ": "

/*
#include <dlfcn.h>
typedef int (*TPRINTF)(const char *, ...);
const static TPRINTF ORG_PRINTF = (TPRINTF)dlsym(RTLD_NEXT, "printf");

int printf(const char *fmt, ...)
{
    ORG_PRINTF(PREFIX);

    va_list ap;
    // 可変長引数を１個の変数にまとめる
    va_start(ap, fmt);
    // まとめられた変数で処理する
    int ret = vprintf(fmt, ap);
    va_end(ap);
    return ret;
};
*/

/* テスト用クラス */
class test
{
public:
    int *t;
    test(int x)
    {
        printf("test class constructor\n");
        t = new int[2]{1, 2};
    }
    ~test()
    {
        printf("test class destructor\n");
        delete[] t;
    }
};

void safe_increment()
{
    test *tmp = new test(2);
}

void safe_increment2()
{
    test *tmp = new test(4);
    delete tmp;
}

int main()
{
    printf("start main function---------------\n");
    int *i = new int[6]{1, 2, 3, 4, 5, 6};
    char *str = new char[12];
    int *j = new int(0);
    test *tmp = new test(1);

    /* file open hook  */
    FILE *fp;
    fp = fopen("text.txt", "w");
    fclose(fp);

    /* C++ iohook r */
    std::ifstream ifs("test2_r.txt");
    ifs.close();

    /* C++ ofstream r */
    std::ofstream ofs("test2_w.txt");
    ofs << ">seq1" << std::endl;
    ofs.close();

    // delete str;

    std::thread t1(safe_increment);
    std::thread t2(safe_increment2);
    std::thread t3(safe_increment);
    std::thread t4(safe_increment);
    printf("finish main function---------------\n");
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    return 0;
}
