#include <stdio.h>
#include <stdint.h>

int main() {

    uint8_t a = 11, b = 2;
    uint8_t *p = &a;

    printf("%d, %d, %d\n", *p, *(p+1), *(&a+1));

    return 0;
}