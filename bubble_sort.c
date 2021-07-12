#include <stdio.h>
#include <stdlib.h>

#define swapArray(type, A, i, j) do {\
    type _tmp;\
    _tmp = A[i];\
    A[i] = A[j];\
    A[j] = _tmp;\
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
    int i, j;
    for (i = 0; i < n - 2; i++) {
        for (j = i; j < n - 2; j++) {
            if (A[j] > A[j + 1]) {
                swapArray(int, A, j, j + 1);
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