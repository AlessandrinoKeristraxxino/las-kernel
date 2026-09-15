// crate/src/boot.c

#include <stdint.h>
#include "limine.h"

extern void kernel_main(void);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_kernel_address_request kaddr_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0
};
__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;


uint64_t hhdm_offset = 0;
uint64_t kernel_phys_base = 0;
uint64_t kernel_virt_base = 0;

void kernel_entry(void) {
    extern char _bss_start, _bss_end;
    for (char *p = &_bss_start; p < &_bss_end; p++) {
        *p = 0;
    }

    hhdm_offset = hhdm_request.response->offset;
    kernel_phys_base = kaddr_request.response->physical_base;
    kernel_virt_base = kaddr_request.response->virtual_base;

    map_page(hhdm_offset + 0xB8000, 0xB8000, 1 << 1 /* writable */);
    
    kernel_main(); //rust
    
    while (1) {
        asm("hlt"); //non dovrebbe mai andare
    }
}