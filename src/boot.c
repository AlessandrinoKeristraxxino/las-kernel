#include <stdint.h>

extern void kernel_main(void);

void kernel_entry(void) {

    extern uint8_t _bss_start, _bss_end;
    for (uint8_t *p = &_bss_start; p < &_bss_end; p++) {
        *p = 0;
    }
    
    kernel_main(); //rust
    
    while (1) {
        asm("hlt"); //non dovrebbe mai andare
    }
}