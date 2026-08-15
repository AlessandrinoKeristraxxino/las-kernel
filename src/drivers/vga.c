// crate/src/drivers/vga.c

#include <stdint.h>
#include <stddef.h>

#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80     
#define VGA_HEIGHT 25 

// puntatore per la prima cella del terminale
static uint16_t *vga_buffer = (uint16_t *)VGA_MEMORY; 

// impostazione del vero buffer
static uint16_t *vga_first_buffer = (uint16_t *)(VGA_MEMORY + VGA_WIDTH*VGA_HEIGHT*2);

// inizializzazione del posizione del cursore
static size_t vga_row = 0; //y
static size_t vga_col = 0; //x

// inizializzazione dei colori (nero e bianco)
static uint8_t vga_colors[] = {0x00, 0x0F}; // 0->bg 1->fg

void vga_init(void) {
    vga_clear();
}

static void vga_putchar_color(uint8_t c, uint8_t *clrs) {
    if (c == '\n') { // newline handle
        vga_col = 0;
        vga_row++;
        if (vga_row >= VGA_HEIGHT) {
            vga_scroll('d');
            vga_row = VGA_HEIGHT - 1;  
        }
        return;
    }
    
    size_t idx = vga_row*VGA_WIDTH + vga_col;
    vga_first_buffer[idx] = (((uint16_t)clrs[0] << 4) | clrs[1]) << 8| c; 
    
    vga_col++;
    if (vga_col >= VGA_WIDTH) {
        vga_col = 0;
        vga_row++;
        if (vga_row >= VGA_HEIGHT) {
            vga_scroll('d');
            vga_row = VGA_HEIGHT - 1;
        }
    }
    
    vga_render();
}

inline void vga_putchar(uint8_t c) {
    vga_putchar_color(c, vga_colors);
}

void vga_write(const char *s) {
    for (size_t i = 0; s[i] != '\0'; i++) {
        vga_putchar((uint8_t)s[i]);
    }
}

void vga_writec(const char *s, uint8_t *c) {
    for (size_t i = 0; s[i] != '\0'; i++) {
        vga_putchar_color((uint8_t)s[i], c);
    }
}

void vga_set_color(uint8_t fg, uint8_t bg) {
    vga_colors[0] = bg;
    vga_colors[1] = fg;
}

void vga_clear() {
    uint16_t *start = (uint16_t *)VGA_MEMORY;
    uint16_t *end = (uint16_t *)0xC0000;
    
    for (uint16_t *p = start; p < end; p++) {
        *p = (((uint16_t)vga_colors[0] << 4) | vga_colors[1]) << 8 | ' ';
    }
    
    // reset del fbuffer e del cursore
    vga_first_buffer = (uint16_t *)(VGA_MEMORY + VGA_WIDTH * VGA_HEIGHT * 2);
    vga_col = 0;
    vga_row = 0;
    
    vga_render();
}

void vga_scroll(char uod) {
    uint16_t *min = (uint16_t *)(VGA_MEMORY + VGA_WIDTH * VGA_HEIGHT * 2);
    uint16_t *max = (uint16_t *)0xC0000 - VGA_HEIGHT * VGA_WIDTH;
    
    if (uod == 'u' && vga_first_buffer > min) {
        vga_first_buffer -= VGA_WIDTH;
    } else if (uod == 'd' && vga_first_buffer < max) {
        vga_first_buffer += VGA_WIDTH;
    }
    
    vga_render();
}

static inline void vga_render() {
    for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++) vga_buffer[i] = vga_first_buffer[i];
}



