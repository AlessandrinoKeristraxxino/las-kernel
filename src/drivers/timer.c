// crate/src/drivers/timer.c

#include <stdint.h>

#define PIT_FREQUENCY 1193182
#define PIT_CH0 0x40
#define PIT_MODE 0x43

static volatile uint64_t ticks = 0;

static inline void outb(uint16_t port, uint8_t val) { // l'assembly per scrivere in un certo registro (port) il valore di val
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void timer_init(uint32_t freq) {
    uint32_t div = PIT_FREQUENCY / freq;

    outb(PIT_MODE, 0b00110110);
    outb(PIT_CH0, div & 0xFF);
    outb(PIT_CH0, (div >> 8) & 0xFF);
}

uint64_t timer_get_ticks(void) {return ticks;}

void timer_interrupt_handler(void) {ticks++;}