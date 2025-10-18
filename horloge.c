#include "horloge.h"

void set_clock_freq(uint16_t freq) {
    outb(0x34, 0x43);
    outb((QUARTZ / CLOCKFREQ) & 0xFF, 0x40);
    outb(((QUARTZ / CLOCKFREQ) & 0xFF00) >> 8, 0x40);
}