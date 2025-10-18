#include <stdio.h>
#include <stdlib.h>

void func1(int size) {
    char *buf = malloc(0x20);
    fgets(buf, size, stdin);
}

void func2(int size) {
    char *buf = malloc(0x30);
    fgets(buf, size, stdin);
}

void func3(int size1, int size2) {
    char *buf1 = malloc(0x30);
    char *buf2 = malloc(0x30);

    fgets(buf1, size1, stdin);
    fgets(buf2, size2, stdin);
}

int main() {
    char *buf = malloc(0x10);
    fgets(buf, 0x20, stdin);

    func1(0x30);
    func2(0x30);

    func3(0x10, 0x50);
}
