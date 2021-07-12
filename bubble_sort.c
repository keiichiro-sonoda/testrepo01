#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SEED 189U

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
// おそらく課題11の通り
void bubbleSort(int *A, int n) {
    int i, j, tmp;
    for (i = 0; i < n - 1; i++) {
        for (j = i; j < n - 1; j++) {
            if (A[j] > A[j + 1]) {
                swapArray(A, j, j + 1, tmp);
            }
        }
    }
}

// バブルソート関数
// 昇順ソート
void bubbleSort2(int *A, int n) {
    int i, j, tmp;
    for (i = n - 1; i > 0; i--) {
        for (j = 0; j < i; j++) {
            if (A[j] > A[j + 1]) {
                swapArray(A, j, j + 1, tmp);
            }
        }
    }
}

int main(void) {
    int n = 16;
    int A[n]; // テスト配列
    // srand(SEED);
    srand((unsigned)time(NULL));
    for (int i = 0; i < n; i++) {
        A[i] = rand() % 100;
    }
    showIntArray(A, n);
    // bubbleSort(A, n);
    bubbleSort2(A, n);
    showIntArray(A, n);
    return 0;
}