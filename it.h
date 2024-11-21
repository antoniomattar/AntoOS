#include "inttypes.h"
#include "stdbool.h"
#include "cpu.h"

void init_traitant_IT(uint32_t num_IT, void (*traitant)(void));

void masque_IRQ(uint32_t num_IRQ, bool masque);

void traitant_IT_32(void);