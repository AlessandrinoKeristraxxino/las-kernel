// crate/src/drivers/vga.c

#include <stdint.h>

/* Very small VGA text mode helpers */
void vga_init(void){
	/* noop for now */
}

void vga_putchar(uint8_t c){
	volatile uint16_t *buf=(uint16_t*)0xB8000;
	static uint16_t pos=0;
	uint8_t attr=0x07; /*light grey on black*/
	buf[pos++]=((uint16_t)attr<<8)|c;
}

void vga_write(const char *s){
	if(!s)return;
	while(*s){
		vga_putchar((uint8_t)*s);
		s++;
	}
}

void vga_set_color(uint8_t fg, uint8_t bg){

}

void vga_clear(){
    
}