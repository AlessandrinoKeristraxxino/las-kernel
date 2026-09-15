// crate/src/drivers/vga.c

#include <stdint.h>
#include <stddef.h>

extern uint64_t hhdm_offset;
#define VGA_MEMORY (0xB8000 + hhdm_offset)
#define VGA_WIDTH 80     
#define VGA_HEIGHT 25 

// Quante "pagine" di scroll vogliamo tenere in memoria (storia dello scroll)
#define VGA_SCROLL_PAGES 8
#define VGA_HIDDEN_ROWS (VGA_HEIGHT * VGA_SCROLL_PAGES)

// puntatore alla vera memoria video (hardware)
static uint16_t *vga_buffer; 
// un array normale in RAM, non memoria fisica extra
static uint16_t vga_hidden[VGA_HIDDEN_ROWS * VGA_WIDTH];
// puntatore che scorre dentro vga_hidden (sostituisce il vecchio vga_first_buffer)
static uint16_t *vga_first_buffer;

// inizializzazione del posizione del cursore
static size_t vga_row = 0; //y
static size_t vga_col = 0; //x

// inizializzazione dei colori (nero e bianco)
static uint8_t vga_colors[] = {0x00, 0x0F}; // 0->bg 1->fg

static void vga_render(void);
static void vga_scroll(char uod);
void vga_clear(void);

static void vga_putchar_color(uint8_t c, const uint8_t *clrs) {
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

static void vga_render(void) {
    for (size_t i = 0; i < VGA_HEIGHT * VGA_WIDTH; i++) vga_buffer[i] = vga_first_buffer[i];
}

void vga_init(void) {
    vga_buffer = (uint16_t *)VGA_MEMORY;
    vga_first_buffer = vga_hidden;

    vga_clear();
}

void vga_putchar(uint8_t c) {
    vga_putchar_color(c, vga_colors);
}

void vga_write(const char *s) {
    for (size_t i = 0; s[i] != '\0'; i++) {
vga_putchar((uint8_t)s[i]);
}
}

void vga_writec(const char *s, const uint8_t *c) {
    for (size_t i = 0; s[i] != '\0'; i++) {
vga_putchar_color((uint8_t)s[i], c);
}
}

void vga_set_color(uint8_t fg, uint8_t bg) {
    vga_colors[0] = bg;
    vga_colors[1] = fg;
}

void vga_clear(void) {
    // Pulisce tutto il buffer nascosto (in RAM, non memoria fisica)
    for (size_t i = 0; i < VGA_HIDDEN_ROWS * VGA_WIDTH; i++) {
    vga_hidden[i] = (((uint16_t)vga_colors[0] << 4) | vga_colors[1]) << 8 | ' ';
    }

    // reset del fbuffer e del cursore
    vga_first_buffer = vga_hidden;
    vga_col = 0;
    vga_row = 0;

    vga_render();
}

void vga_scroll(char uod) {
    uint16_t *min = vga_hidden;
    uint16_t *max = vga_hidden + (VGA_HIDDEN_ROWS - VGA_HEIGHT) * VGA_WIDTH;

    if (uod == 'u' && vga_first_buffer > min) {
    vga_first_buffer -= VGA_WIDTH;
    } else if (uod == 'd' && vga_first_buffer < max) {
    vga_first_buffer += VGA_WIDTH;
    }

    vga_render();
}
