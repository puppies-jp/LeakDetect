#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILEPATH "msg_file.txt"

int main();
void vuln(const char *line);
void stack_dump(void *ptr, int counts);
void hello();

int main()
{
    char linebuf[1024];
    FILE *fp;
    long mark1 = 0x11111111;
    memset(linebuf, 0, sizeof(linebuf));

    fp = fopen(FILEPATH, "r");
    fgets(linebuf, sizeof(linebuf) - 1, fp);
    fclose(fp);

    vuln(linebuf);

    printf("------------- end of main() -------------\n");
}

void vuln(const char *line)
{
    char msg[20];
    long mark2 = 0x22222222;
    memset(msg, 0, sizeof(msg));

    strcpy(msg, line);

    stack_dump(&mark2, 16);

    printf("INPUT[%s]\n", msg);
}

void stack_dump(void *ptr, int counts)
{
    int i;
    unsigned long *ulong_ptr = (unsigned long *)ptr;
    unsigned char uchar_buf[4];

    unsigned long delta = (unsigned long)hello - 0x100003dd0;
    void *retAddr = __builtin_return_address(0);
    printf("Return Addr (%p) -> hello(%p),hello diff(%08lx),main(%08lx)\n",
           retAddr,
           hello,
           delta,
           (unsigned long)main - delta);
    // memcpy((uchar_buf - sizeof(void *)), (void *)hello, sizeof(void *));
    printf("-----------------------------------------\n");
    printf(" address | long var | +0 +1 +2 +3 | 0123\n");
    printf("-----------------------------------------\n");
    for (i = 0; i < counts; i++)
    {
        printf(" %08lx| %08lx", &ulong_ptr[i], ulong_ptr[i]);
        memcpy(uchar_buf, &ulong_ptr[i], sizeof(uchar_buf));
        printf(" | %02x %02x %02x %02x",
               uchar_buf[0], uchar_buf[1], uchar_buf[2], uchar_buf[3]);
        if (uchar_buf[0] < 32 || uchar_buf[0] > 126)
            uchar_buf[0] = '.';
        if (uchar_buf[1] < 32 || uchar_buf[1] > 126)
            uchar_buf[1] = '.';
        if (uchar_buf[2] < 32 || uchar_buf[2] > 126)
            uchar_buf[2] = '.';
        if (uchar_buf[3] < 32 || uchar_buf[3] > 126)
            uchar_buf[3] = '.';
        printf(" | %c%c%c%c\n",
               uchar_buf[0], uchar_buf[1], uchar_buf[2], uchar_buf[3]);
    }
    printf("-----------------------------------------\n");
}

void hello()
{
    printf("+----------+\n");
    printf("| HELLO! |\n");
    printf("+----------+\n");
    exit(0);
}