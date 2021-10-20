#include <stdio.h>

int nibai(int x) {
    return 2 * x;
}

int sanbai(int x) {
    return 3 * x;
}

int main(void) {
    int flag = 0;
    int (*a)(int);
    
    if (flag == 0) {
        a = nibai;
    } else {
        a = sanbai;
    }
    // a = sanbai;
    printf("%d\n", a(5));
    for (int j = 0; j < 100; j++) {
        // if (flag == 0) {
        //     nibai(5);
        // } else {
        //     sanbai(5);
        // }
        a(5);
    }
    return 0;
}