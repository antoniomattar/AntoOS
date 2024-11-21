#include "horloge.h"
#include "cpu.h"


void set_clock_freq(uint16_t freq) {
    uint16_t div = QUARTZ / freq;
    outb(0x34, 0x43);
    outb( (uint8_t) (div & 0xFF), 0x40);
    outb((uint8_t) (div >> 8), 0x40);
}