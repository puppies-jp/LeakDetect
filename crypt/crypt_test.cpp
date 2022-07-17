#include <unistd.h>
#include <stdio.h>

/* 🌟コンパイルオプションにcryptをリンクすること
 * g++ crypt_test.cpp -o crypt.out -lcrypt
 */

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("usage %s <password> <salt>\n", argv[0]);
        return 1;
    }
    printf("password = '%s' , salt = '%s'\n", argv[1], argv[2]);
    printf("hash => '%s'\n", crypt(argv[1], argv[2]));
}
