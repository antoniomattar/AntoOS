#include "it.h"
#include "segment.h"


void init_traitant_IT(uint32_t num_IT, void (*traitant)(void)) {
    uint32_t* premier_mot = (uint32_t* ) (0x1000 + 2*num_IT*4);
    *premier_mot = (KERNEL_CS << 16) | ((uint32_t) traitant & 0xFFFF);

    uint32_t* second_mot = (uint32_t*) (premier_mot + 1);
    *second_mot = ((uint32_t) traitant & 0xFFFF0000) | 0x8E00;
}

void masque_IRQ(uint32_t num_IRQ, bool masque) {
    uint8_t masqueur = inb(0x21);
    if (masque) {
        masqueur |= 1 << num_IRQ;
    } else {
        masqueur &= ~(1 << num_IRQ);
    }
    outb(masqueur, 0x21);
}