// crate/src/boot.c

#include <stdint.h>
#include "limine.h"

extern void kernel_main(void);

// --- Limine protocol requests ---

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

// --- fine Limine requests ---

void kernel_entry(void) {
    extern char _bss_start, _bss_end;
    for (char *p = &_bss_start; p < &_bss_end; p++) {
        *p = 0;
    }
    
    kernel_main(); //rust
    
    while (1) {
        asm("hlt"); //non dovrebbe mai andare
    }
}