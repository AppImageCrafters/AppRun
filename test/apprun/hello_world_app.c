#include <stdio.h>
#include <gnu/libc-version.h>

int main() {
    printf("Hello World\n");
    printf("GLIBC Version: %s\n", gnu_get_libc_version());
    printf("GLIBC Version: %d.%d\n", __GLIBC__, __GLIBC_MINOR__);
    return 0;
}