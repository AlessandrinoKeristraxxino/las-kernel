// crate/src/drivers/irq.c

#include <stdint.h>

typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed)) IDT_Entry;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) IDT_Reg;;

static IDT_Entry idt[256];

void idt_init(void) { // inizializzazione dell'IDT
    for (int i = 0; i < 256; i++) {
        idt[i].offset_low = 0;
        idt[i].offset_mid = 0;
        idt[i].offset_high = 0;
        idt[i].selector = 0x08;
        idt[i].ist = 0;
        idt[i].type_attr = 0x8E;
        idt[i].reserved = 0;
    }
}

void idt_set_gate(uint8_t idx, uint64_t handler) { // scrive l'indirizzo dell'handler (funzione interrupt)
    idt[idx].offset_low = handler & 0xFFFF;
    idt[idx].offset_low = (handler >> 16) & 0xFFFF;
    idt[idx].offset_low = (handler >> 32) & 0xFFFFFFFF;
}

void idt_load(void) { // loada la tabella IDT
    IDT_Reg idtr;
    idtr.limit = (sizeof(idt) - 1);
    idtr.base = (uint64_t)idt;

    asm volatile("lidt %0" : : "m"(idtr));
}

void irq_init() {
    idt_init();

    extern void irq0_stub(void);
    idt_set_gate(32, (uint64_t)irq0_stub);

    idt_load();
}

inline void irq_enable() {asm volatile("sti");}

inline void irq_disable() {asm volatile("cli");}
