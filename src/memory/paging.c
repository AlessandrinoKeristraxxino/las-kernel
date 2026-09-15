// crate/src/memory/paging.c

#include <stdint.h>
#include <stddef.h>

extern uint64_t hhdm_offset;
extern uint64_t kernel_phys_base;
extern uint64_t kernel_virt_base;

#define PAGE_PRESENT  (1ULL << 0)
#define PAGE_WRITABLE (1ULL << 1)
#define PAGE_SIZE     4096

#define PT_INDEX(addr)   (((addr) >> 12) & 0x1FF)
#define PD_INDEX(addr)   (((addr) >> 21) & 0x1FF)
#define PDPT_INDEX(addr) (((addr) >> 30) & 0x1FF)
#define PML4_INDEX(addr) (((addr) >> 39) & 0x1FF)

#define PHYS_ADDR_MASK 0x000FFFFFFFFFF000ULL

// Pool statico di pagine per nuove tabelle intermedie
#define PAGE_POOL_SIZE 8
static uint8_t page_pool[PAGE_POOL_SIZE][PAGE_SIZE] __attribute__((aligned(4096)));
static size_t page_pool_used = 0;

// Converte un indirizzo FISICO in un indirizzo virtuale accessibile
// (tramite l'HHDM che Limine ha già mappato per tutta la RAM normale)
static uint64_t *phys_to_virt(uint64_t phys) {
    return (uint64_t *)(phys + hhdm_offset);
}

// Alloca una nuova pagina dal pool statico, la azzera, e ritorna il suo indirizzo FISICO
static uint64_t alloc_page_table(void) {
    if (page_pool_used >= PAGE_POOL_SIZE) {
        // Pool esaurito: per ora blocchiamo (in futuro qui va un vero allocatore fisico)
        while (1) { asm volatile("hlt"); }
    }
    uint8_t *page = page_pool[page_pool_used++];
    for (size_t i = 0; i < PAGE_SIZE; i++) page[i] = 0;

    // Il pool vive nel kernel (higher-half), quindi per sapere il suo indirizzo
    // fisico dobbiamo "sottrarre" l'offset di link e "aggiungere" la base fisica
    uint64_t virt = (uint64_t)page;
    return virt - kernel_virt_base + kernel_phys_base;
}

// Mappa virt_addr -> phys_addr con i flag dati, creando le tabelle intermedie se mancano
void map_page(uint64_t virt_addr, uint64_t phys_addr, uint64_t flags) {
    uint64_t cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));

    uint64_t *pml4 = phys_to_virt(cr3 & PHYS_ADDR_MASK);

    uint64_t pml4_idx = PML4_INDEX(virt_addr);
    if (!(pml4[pml4_idx] & PAGE_PRESENT)) {
        uint64_t new_table = alloc_page_table();
        pml4[pml4_idx] = new_table | PAGE_PRESENT | PAGE_WRITABLE;
    }
    uint64_t *pdpt = phys_to_virt(pml4[pml4_idx] & PHYS_ADDR_MASK);

    uint64_t pdpt_idx = PDPT_INDEX(virt_addr);
    if (!(pdpt[pdpt_idx] & PAGE_PRESENT)) {
        uint64_t new_table = alloc_page_table();
        pdpt[pdpt_idx] = new_table | PAGE_PRESENT | PAGE_WRITABLE;
    }
    uint64_t *pd = phys_to_virt(pdpt[pdpt_idx] & PHYS_ADDR_MASK);

    uint64_t pd_idx = PD_INDEX(virt_addr);
    if (!(pd[pd_idx] & PAGE_PRESENT)) {
        uint64_t new_table = alloc_page_table();
        pd[pd_idx] = new_table | PAGE_PRESENT | PAGE_WRITABLE;
    }
    uint64_t *pt = phys_to_virt(pd[pd_idx] & PHYS_ADDR_MASK);

    uint64_t pt_idx = PT_INDEX(virt_addr);
    pt[pt_idx] = (phys_addr & PHYS_ADDR_MASK) | flags | PAGE_PRESENT;

    // Invalida la cache TLB per questo indirizzo, altrimenti la CPU potrebbe
    // continuare a usare una traduzione vecchia (o "non presente") in cache
    asm volatile("invlpg (%0)" : : "r"(virt_addr) : "memory");
}