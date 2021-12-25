#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fstream>

/* テスト用クラス */
class test
{
public:
    int *t;
    test()
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

int main()
{
    printf("start main function---------------\n");
    int *i = new int[6]{1, 2, 3, 4, 5, 6};
    char *str = new char[12];
    int *j = new int(0);
    test *tmp = new test();

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

    delete tmp;

    printf("finish main function---------------\n");
    return 0;
}
