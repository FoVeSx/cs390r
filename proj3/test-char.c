#include <stdio.h>

void main() {
	char a[0x10] = "Hello";
	printf("%p\n", &a);
}
