#include <stdio.h>
#include <iostream>
#include <typeinfo>
#include <tuple>
#include <map>

#define STRINGIFY(n) #n
#define TOSTRING(n) STRINGIFY(n)
#define PREFIX __FILE__ ":" TOSTRING(__LINE__) ": "

class test
{
public:
    int *t;
    test() { t = new int(1); }
};
int main()
{
    printf("start main function---------------\n");
    int *xi = new int[6]{1, 2, 3, 4, 5, 6};
    char *str = new char[12];
    int *j = new int{0};
    test *tmp = new test();
    delete tmp;
    printf("finish main function---------------\n");
    return 0;
}
