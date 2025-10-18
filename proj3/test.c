#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>

void main(void) {
    char buffer[0x50] = {};

    int fd = open("flag", O_RDONLY, 0);
    printf("The file descriptor for the file is %d\n", fd);

    read(fd, buffer, 50);
    write(0, buffer, 50);
}
