//
// Created by mattara on 05/09/24.
//
#include "ecran.h"
#include "process.h"

unsigned short *ptr_mem(unsigned int lig, unsigned int col)
{
    return (unsigned short *)(0xB8000 + 2 * (80 * lig + col));
}

void ecrit_car(unsigned int lig, unsigned int col, char c, unsigned int fond, unsigned int police)
{
    unsigned short *ptr = ptr_mem(lig, col);
    *ptr = (fond << 12) | (police << 8) | c;
}

void efface_ecran(void)
{
    for (int i = 0; i < 80 * 25; i++)
    {
        ecrit_car(i / 80, i % 80, ' ', 0, 15);
    }
}

void place_curseur(unsigned int lig, unsigned int col)
{
    unsigned short pos = 80 * lig + col;
    outb(0x0F, 0x3D4);
    outb((unsigned char)(pos & 0xFF), 0x3D5);

    outb(0x0E, 0x3D4);
    outb((unsigned char)((pos >> 8) & 0xFF), 0x3D5);
}

static unsigned int lig = 0;
static unsigned int col = 0;

void traite_car(char c)
{
    switch (c)
    {
        case '\b':
            if (col != 0)
            {
                col--;
            }
            break;
        case '\t':
            col = (col + 8) % 80;
            break;
        case '\n':
            col = 0;
            lig = (lig +1) % 25;
            break;
        case '\f':
            efface_ecran();
            col = 0;
            lig = 0;
            break;
        case '\r':
            col = 0;
            break;
        default:
            ecrit_car(lig, col, c, 0, 15);
            col++;
            break;
    }

    place_curseur(lig, col);

    if (col == 80)
    {
        lig++;
        col = 0;
    }

    if (lig == 25)
    {
        defilement();
        lig = 24;
    }
}

void defilement(void)
{
    // on utilise memmvoe pr deplacer les lignes de 1 q 25 d'un rang
    //memmove(ptr_mem(0, 0), ptr_mem(1, 0), 80 * 24 * 2);

    //for (int i = 0; i < 80; ++i) {
    //    ecrit_car(24,i,' ',0,15);
    //}
    for (uint8_t ligne = 0; ligne < 25; ligne++) // pour chaque ligne
    {
        memmove(ptr_mem(ligne,0),ptr_mem(ligne+1,0),80*2); // on deplace la ligne d'apres sur celle la
    }
    memset(ptr_mem(25,0),20,80*2); // efface la dernière ligne
}

void console_putbytes(const char *s, int len)
{
    for (int i = 0; i < len; i++)
    {
        traite_car(s[i]);
    }
}

void affiche_etat(Process* process, int lig) {
    for (int i = 0; i < 10; i++)
    {
        // 
        ecrit_car(lig, 80 - 11 + i, process->name[i], 0, 15);

        // print le statut de ce process( qui est un enum State normalement un entier)
        ecrit_car(lig, 80 - 1, process->status + '0', 0, 15);

    }

}

void ecrit_temps(char *temps, int taille)
{
    for (int i = 0; i < taille; i++)
    {
        ecrit_car(0, 80 - taille + i, temps[i], 0, 15);
    }

    for (int i = 0; i < PROCESSES_TABLE_SIZE; ++i) {
        Process* current = processes_table[i];
        if (current) {
            affiche_etat(current, 1+i);
        }
        else {
            break;
        }
    }

}