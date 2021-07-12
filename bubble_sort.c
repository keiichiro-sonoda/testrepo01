#include <stdio.h>
#include <stdlib.h>

// 配列の要素を入れ替えるマクロ
// 一時変数は呼び出し元で用意しておく
#define swapArray(A, i, j, tmp) do {\
    (tmp) = (A)[i];\
    (A)[i] = (A)[j];\
    (A)[j] = (tmp);\
} while (0)

// 配列表示関数
void showIntArray(const int *A, int n) {
    for (int i = 0; i < n; i++) {
        printf("%2d ", A[i]);
    }
    putchar(10);
}

// バブルソート関数
void bubbleSort(int *A, int n) {
    int i, j, tmp;
    for (i = 0; i < n - 2; i++) {
        for (j = i; j < n - 2; j++) {
            if (A[j] > A[j + 1]) {
                swapArray(A, j, j + 1, tmp);
            }
        }
    }
}

int main(void) {
    int n = 16;
    int A[n]; // テスト配列
    srand(123U);
    for (int i = 0; i < n; i++) {
        A[i] = rand() % 100;
    }
    showIntArray(A, n);
    bubbleSort(A, n);
    showIntArray(A, n);
    return 0;
}