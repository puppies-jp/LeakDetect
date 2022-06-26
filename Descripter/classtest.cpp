#include <stdio.h>
#include <iostream>

class test
{
public:
    int x = 0;
};

void func_norm(test tmp)
{
    printf("%-10s: %p : %d\n", __func__, &tmp, tmp.x);
}

void func_ref(test &tmp)
{
    printf("%-10s: %p : %d\n", __func__, &tmp, tmp.x);
}

void func_ptr(test *tmp)
{
    printf("%-10s: %p : %d\n", __func__, tmp, tmp->x);
}

int main()
{
    test tmp;
    tmp.x = 3;
    printf("%-10s: %p : %d\n", __func__, &tmp, tmp.x);
    func_norm(tmp);
    func_ref(tmp);
    func_ptr(&tmp);
    return 0;
}