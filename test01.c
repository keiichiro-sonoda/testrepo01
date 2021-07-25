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
u_int encRepCode3(u_int msg, int nm) {
    u_int code = 0;
    int i, j, b;
    for (i = 0; i < nm; i++) {
        b = getBit(msg, i);
        for (j = 0; j < 3; j++) {
            code |= b << (i * 3 + j);
        }
    }
    return code;
}

u_int decRepCode3(u_int code, int nm) {
    u_int msg = 0;
    int i, j, s;
    for (i = 0; i < nm; i++) {
        s = 0;
        for (j = 0; j < 3; j++) {
            s += getBit(code, i * 3 + j);
        }
        if (s >= 2) {
            msg |= 1 << i;
        }
    }
    return msg;
}

int main(void) {
    srand((unsigned)time(NULL));
    // 4ビットに制限
    u_int m = rand() & 0b1111;
    u_int c;
    printBin32(m);
    c = encRepCode3(m, 4);
    printBin32(c);
    printBin32(decRepCode3(c, 4));
    return 0;
}