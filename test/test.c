#include <stdio.h>
#include <stdint.h>

int main() {

    uint8_t a = 0, b = 255;

    printf("%d, %d\n", a, b);

    b++;

    printf("%d, %d\n", a, b);
}