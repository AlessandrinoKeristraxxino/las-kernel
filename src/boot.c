// crate/src/boot.c

#include <stdint.h>

extern void kernel_main(void);

void kernel_entry(void) {

    extern char _bss_start, _bss_end;  // ← cambia da uint8_t a char
    for (char *p = &_bss_start; p < &_bss_end; p++) {
        *p = 0;
    }
    
    kernel_main(); //rust
    
    while (1) {
        asm("hlt"); //non dovrebbe mai andare
    }
}