#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 指定したビットを取得
#define getBit(x, n) (((x) >> (n)) & 1)

// double型の乱数を生成 [0.0, 1.0]
#define randDouble() ((double)rand() / RAND_MAX)

// 32ビットのバイナリ表示
void printBin32(u_int x) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", getBit(x, i));
    }
    putchar(10);
}

// 各ビット毎のエラー率が一定の通信路
u_int channelNoise(u_int x, int nc, double e_prob) {
    u_int y = 0;
    for (int i = 0; i < nc; i++) {
        y |= (getBit(x, i) ^ (randDouble() <= e_prob)) << i;
    }
    return y;
}

// (3, 1)繰り返し符号の符号化関数
//  第2引数はメッセージのビット長を与える
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

// (3, 1)繰り返し符号の復号関数
// 第2引数はメッセージのビット長を与える
u_int decRepCode3(u_int rsv, int nm) {
    u_int msg = 0;
    int i, j, s;
    for (i = 0; i < nm; i++) {
        s = 0;
        for (j = 0; j < 3; j++) {
            s += getBit(rsv, i * 3 + j);
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
    u_int tm = rand() & 0b1111;
    u_int c, r, rm;
    printBin32(tm);
    c = encRepCode3(tm, 4);
    printBin32(c);
    r = channelNoise(c, 12, 0.1);
    printBin32(r);
    rm = decRepCode3(r, 4);
    printBin32(rm);
    return 0;
}