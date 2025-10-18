//
// Created by mattara on 05/09/24.
//
#include <stdlib.h>
#include <inttypes.h>
#include "string.h"
#include "cpu.h"

unsigned short *ptr_mem(unsigned int lig, unsigned int col);

void ecrit_car(uint32_t lig, uint32_t col, char c, uint16_t fond, uint16_t police);

void efface_ecran(void);

void place_curseur(unsigned int lig, unsigned int col);

void traite_car(char c);

void defilement(void);

void console_putbytes(const char *s, int len);

void ecrit_temps(char* temps, int taille);