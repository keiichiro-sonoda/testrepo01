#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void printBin32(u_int n) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (n >> i) & 1);
    }
    putchar(10);
}

int main(void) {
    // puts("こんにちは");
    srand((unsigned)time(NULL));
    int r = rand();
    printf("%d\n", r);
    printBin32(r);
    // printBin32(8);
    return 0;
}