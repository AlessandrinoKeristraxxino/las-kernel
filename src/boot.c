// crate/src/boot.c

#include <stdint.h>

extern void kernel_main(void);

void kernel_entry(void) {

    extern uint8_t _bss_start, _bss_end;
    for (uintptr_t p = (uintptr_t)&_bss_start; p < (uintptr_t)&_bss_end; p++) {
        *(uint8_t *)p = 0;
    }
    
    kernel_main(); //rust
    
    while (1) {
        asm("hlt"); //non dovrebbe mai andare
    }
}