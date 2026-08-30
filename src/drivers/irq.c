// crate/src/drivers/irq.c

#include <stdint.h>

#define PIC1_CMD 0x20
#define PIC1_DATA 0x21
#define PIC2_CMD 0xA0
#define PIC2_DATA 0xA1

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

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

static uint8_t inb(uint16_t port) { // in pratica mette il dato che si trova al registro (port) nella variabile ret 
    uint16_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port)); 
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) { // l'assembly per scrivere in un certo registro (port) il valore di val
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void pic_remap(void) {
    uint8_t mask1 = inb(PIC1_DATA), mask2 = inb(PIC2_DATA);

    // ICW1
    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);

    // ICW2
    outb(PIC1_DATA, 32);
    outb(PIC2_DATA, 40);

    // ICW3
    outb(PIC1_DATA, 0b0100);
    outb(PIC2_DATA, 0b0010);

    // ICW4
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);

    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

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

    extern void irq1_stub(void);
    idt_set_gate(33, (uint64_t)irq1_stub);

    pic_remap();

    idt_load();
}

inline void irq_enable() {asm volatile("sti");}

inline void irq_disable() {asm volatile("cli");}
