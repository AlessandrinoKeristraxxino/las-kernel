#include <stdio.h>
#include <stdint.h>

int main() {

    uint8_t u = 0b01110101;
    if (u == 'u') printf("eh si %x\n", u & 'u');
    printf("%x\n", 'u');


    switch ('u' & 1) { // i char 'u' e 'd' sono uno "pari" e l'altro "dispari" grazie a questo funziona 
    case 1:
        // codice per su
        break;
    
    case 0:
        // codice per giù
        break;

    default:
        break;
    }

    return 0;
}