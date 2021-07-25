#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 指定したビットを取得
#define getBit(x, n) (((x) >> (n)) & 1)

// 32ビットのバイナリ表示
void printBin32(u_int x) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", getBit(x, i));
    }
    putchar(10);
}

// 繰り返し符号を生成して返す
u_int repCode3(u_int m, int n) {
    u_int code = 0;
    // int i, j;
    // for (i = 0; i < n; i++) {
    //     for (j = 0; j < 3; j++) {

    //     }
    // }
    return code;
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