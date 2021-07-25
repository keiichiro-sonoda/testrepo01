#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    // puts("こんにちは");
    srand((unsigned)time(NULL));
    printf("%d\n", rand());
    return 0;
}