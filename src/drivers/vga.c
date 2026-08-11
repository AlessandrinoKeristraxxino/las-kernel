// crate/src/drivers/vga.c

#include <stdint.h>
#include <stddef.h>

#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80     
#define VGA_HEIGHT 25 

// puntatore per la prima cella del terminale
static uint16_t *vga_buffer = (uint16_t *)VGA_MEMORY; 

// inizializzazione del posizione del cursore
static size_t vga_row = 0;
static size_t vga_col = 0;

// inizializzazione dei colori (bianco e nero)
static uint8_t fg_color = 0x0F;
static uint8_t bg_color = 0x00;

void vga_init(void) {

    for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++) {
        vga_buffer[i] = bg_color | ' ';
    }

    vga_col = 0;
    vga_row = 0;
}

void vga_putchar(uint8_t c) {}

void vga_write(const char *s) {}

void vga_set_color(uint8_t fg, uint8_t bg) {}

void vga_clear() {}