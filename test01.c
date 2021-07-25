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
    int i, j, b;
    for (i = 0; i < n; i++) {
        b = getBit(m, i);
        for (j = 0; j < 3; j++) {
            code |= b << (i * 3 + j);
        }
    }
    printBin32(code);
    return code;
}

int main(void) {
    srand((unsigned)time(NULL));
    // 4ビットに制限
    u_int m = rand() & 0b1111;
    u_int c;
    printBin32(m);
    c = repCode3(m, 4);
    printBin32(c);
    return 0;
}