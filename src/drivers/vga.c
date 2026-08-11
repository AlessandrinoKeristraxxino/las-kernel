// crate/src/drivers/vga.c

#include <stdint.h>
#include <stddef.h>

#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80     
#define VGA_HEIGHT 25 

// puntatore per la prima cella del terminale
static uint16_t *vga_buffer = (uint16_t *)VGA_MEMORY; 

// inizializzazione del posizione del cursore
static size_t vga_row = 0; //y
static size_t vga_col = 0; //x

// inizializzazione dei colori (bianco e nero)
static uint8_t vga_fg_color = 0x0F;
static uint8_t vga_bg_color = 0x00;

void vga_init(void) {

    for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++) {
        vga_buffer[i] = vga_bg_color | ' ';   // <- questo non funziona così i bit vanno messi in modo diverso ma non so come e non lo trovo da nessuna parte quindi aspetto claudio
    }

    vga_col = 0;
    vga_row = 0;
}


void vga_putchar(uint8_t c) {

    size_t idx = vga_row*VGA_WIDTH + vga_col;
    vga_buffer[idx] = vga_fg_color | c; // <- questo non funziona così i bit vanno messi in modo diverso ma non so come e non lo trovo da nessuna parte quindi aspetto claudio

    if (vga_col == VGA_WIDTH-1 && vga_row == VGA_HEIGHT-1) return; // per ora il cursore rimane bloccato nell'ultimo carattere

    vga_col = vga_col + 1;
    if (vga_col >= VGA_WIDTH) {
        vga_row = vga_row + 1;
        vga_col = 0;
    }
}

void vga_write(const char *s) {}

void vga_set_color(uint8_t fg, uint8_t bg) {

    vga_bg_color = bg;
    vga_fg_color = fg;
    
}

void vga_clear() {}