// crate/src/drivers/keyboard.c

#include <stdint.h>

#define KEYBOARD_DATA 0x60
#define KEYBOARD_STATUS 0x64

static uint8_t kb_buffer[256], kb_head = 0, kb_tail = 0;

typedef enum {
    GENERIC_LAYOUT,
    LAYOUT_UK,
    LAYOUT_IT
} KeyboardLayout;

static KeyboardLayout current_layout = LAYOUT_UK;

static uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void keyboard_init(void) {
    asm("cli");
    while (inb(KEYBOARD_STATUS) & 0x01) {
        inb(KEYBOARD_DATA); // blud questa parte è di assembly a quanto pare (secondo claude) se faccio inb allo specifico indirizzo di lettura dei dati della tastiera il dato viene cancellato
    }
    asm("sti");
}

uint8_t keyboard_getscncd(void) { // questa ti returna lo scancode se facciamo tutta la tastiera (e non solo i tasti principali) dobbiamo usare anche quella extended uk (la mia suppongo tu abbia la italiana)
    if (kb_head == kb_tail) return 0;
    uint8_t sc = kb_buffer[kb_tail];
    kb_tail = (kb_tail + 1) % 256;
    return sc;
}

void keyboard_set_layout(KeyboardLayout layout) {
    current_layout = layout;
}

static char scancode_to_ascii(uint8_t sc) {
    static const char generic[] = {
    0,    27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    0,    0,   '\b','\t','q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o',  'p', 0,   0,   '\n',0,   'a', 's', 'd', 'f', 'g', 'h',
    'j',  'k', 'l', 0,   0,   0,   0,   0,   'z', 'x', 'c', 'v',
    'b',  'n', 'm', 0,   0,   0,   0,   0,   0,   ' '
        };
    static const char uk[] = {
    0,    27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '-',  '=', '\b','\t','q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o',  'p', '[', ']', '\n',0,   'a', 's', 'd', 'f', 'g', 'h',
    'j',  'k', 'l', ';', '\'','`', 0,   '#', 'z', 'x', 'c', 'v',
    'b',  'n', 'm', ',', '.', '/', 0,   '*', 0,   ' '
        };
    static const char it[] = {
    0,    27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
    '\'', '?', '\b','\t','q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o',  'p', '?', '+', '\n',0,   'a', 's', 'd', 'f', 'g', 'h',
    'j',  'k', 'l', '?', '?', '\\',0,   0,   'z', 'x', 'c', 'v',
    'b',  'n', 'm', ',', '.', '-', 0,   '*', 0,   ' '
        };

    if (sc >= (sizeof(generic) / sizeof(generic[0])))
    {
        return 0;
    }
    
    switch (current_layout)
    {
    case GENERIC_LAYOUT:
        return generic[sc];
        break;
    
    case LAYOUT_UK:
        return uk[sc];
        break;
    
    case LAYOUT_IT:
        return it[sc];
        break;
    
    default:
        return generic[sc];
        break;
    }
}

char keyboard_getchar(void) {
    uint8_t sc = keyboard_getscncd();
    if (sc == 0) return 0;
    return scancode_to_ascii(sc);
}

uint8_t keyboard_haschar() {
    return kb_head != kb_tail;
}

void keyboard_interrupt_handler(void) {
    uint8_t sc = inb(KEYBOARD_DATA);
    if (sc & 0x80) return;       // in futuro da migliorare ma per ora questa è la gestione dei key-release
    kb_buffer[kb_head] = sc;
    kb_head = (kb_head + 1) % 256;
}