#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

extern int counter;

int main(void)
{
    int fd = open("tmp.txt", O_WRONLY | O_APPEND);

    printf("tmp.txt fd = %d\n", fd);
    dup2(fd, 4);
    dprintf(4, "[socket:4]This string will be printed in tmp.txt file\n");
    dprintf(fd, "[socket:%d(fd=%1$d)]This string will be printed in tmp.txt file\n", fd);

    exit(EXIT_SUCCESS);
}
