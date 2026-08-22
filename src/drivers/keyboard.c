// crate/src/drivers/keyboard.c

#include <stdint.h>

#define KEYBOARD_DATA 0x60
#define KEYBOARD_STATUS 0x64

static uint8_t inb(uint16_t port) {
    uint16_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port)); // in pratica mette il dato che si trova al registro (port) nella variabile ret 
    return ret;
}

void keyboard_init(void) {
    asm("cli");
    while (inb(KEYBOARD_STATUS) & 0x01) {
        inb(KEYBOARD_DATA); // blud questa parte è di assembly a quanto pare (secondo claude) se faccio inb allo specifico indirizzo di lettura dei dati della tastiera il dato viene cancellato
    }
    asm("sti");
}

uint8_t keyboard_getchar(void) { // questa ti returna lo scancode se facciamo tutta la tastiera (e non solo i tasti principali) dobbiamo usare anche quella extended uk (la mia suppongo tu abbia la italiana)
    if (inb(KEYBOARD_STATUS) & 0x01) {
        return inb(KEYBOARD_DATA);
    }
    return 0;
}

uint8_t keyboard_haschar(const char c) {return 0;}