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
static uint8_t vga_fg_color = 0x0F; //15 in esadecimale
static uint8_t vga_bg_color = 0x00; //0 in esadecimale

void vga_init(void) {
    vga_clear();
}

void vga_putchar(uint8_t c) {
    
    if (c == '\n') {
        vga_col = 0;
        if (vga_row < VGA_HEIGHT - 1) { //sempre gestione temporanea del fondoschermo
            vga_row++;
        }
        return;
    }
    
    size_t idx = vga_row*VGA_WIDTH + vga_col;
    vga_buffer[idx] = (((uint16_t)vga_bg_color << 4) | vga_fg_color) << 8| c; 
    
    if (vga_col == VGA_WIDTH-1 && vga_row == VGA_HEIGHT-1) return; // per ora il cursore rimane bloccato nell'ultimo carattere
    
    vga_col++;
    if (vga_col >= VGA_WIDTH) {
        vga_row++;
        vga_col = 0;
    }
}

void vga_write(const char *s) {

    for (size_t i = 0; s[i] != '\0'; i++) {
        vga_putchar((uint8_t)s[i]);
    }
    
}

void vga_set_color(uint8_t fg, uint8_t bg) {

    vga_bg_color = bg;
    vga_fg_color = fg;
    
}

void vga_clear() {
    
        for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            vga_buffer[i] = (((uint16_t)vga_bg_color << 4) | vga_fg_color) << 8 | ' ';
        }
    
        vga_col = 0;
        vga_row = 0;
    
}