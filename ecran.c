#include "ecran.h"
#include "process.h"

static uint32_t lig = 0;
static uint32_t col = 0;

uint16_t* ptr_mem(uint32_t lig, uint32_t col) {
    return (uint16_t*) (0xB8000 + 2*( 80* lig + col));
}

void ecrit_car(uint32_t lig, uint32_t col, char  c, uint16_t bg_color, uint16_t font_color) {
    uint16_t* curr_char_ptr = ptr_mem(lig,col);
    *curr_char_ptr = (bg_color << 12) | (font_color << 8) | c;
}

void efface_ecran(void) {
    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 80; ++j) {
            ecrit_car(i,j,' ',0,15);
        }
    }
}

void place_curseur(uint32_t lig, uint32_t col) {
    uint16_t cursor_pos = 80*lig + col;
    outb(0x0F,0x3D4);
    outb((unsigned char)(cursor_pos & 0x00FF), 0x3D5);

    outb(0x0E,0x3D4);
    outb((unsigned char)( (cursor_pos >> 8) & 0x00FF), 0x3D5);
    lig = lig;
    col = col;
}

void traite_car(char c) {
    switch (c) {
        case '\b':
            if (col>0)
                col--;
            place_curseur(lig,col);
            break;

        case '\t':
            col = (col + 8) % 80;
            break;

        case '\n':
            lig++;
            col=0;
            break;

        case '\f':
            efface_ecran();
            col=0;
            lig=0;
            place_curseur(lig,col);
            break;

        case '\r':
            col=0;
            break;

        default:
            ecrit_car(lig,col,c,0,15);
            col++;
            break;

    }

    if (col >= 80) {
        col=0;
        lig++;
    }

    if (lig >= 25) {
        defilement();
        lig=24;
    }

    place_curseur(lig,col);
}

void defilement(void) {
    memmove(
            ptr_mem(0,0),
            ptr_mem(1,0),
            80*24*2
    );
    for (int col = 0; col < 80; ++col) {
        ecrit_car(24,col,' ',0,15);
    }
}

void console_putbytes(const char* s, int len) {
    for (int i = 0; i < len; ++i) {
        traite_car(s[i]);
    }
}

// Affiche l'état d'un processus sur une ligne spécifique
void affiche_etat(Process *process, int lig)
{
    if (!process) return;

    // Affiche le nom
    for (int i = 0; i < 10 && process->name[i] != '\0'; i++)
    {
        ecrit_car(lig, 70 + i, process->name[i], 0, 15); // Écrire dans la dernière colonne (70-79)
    }

    // Affiche le statut (converti en caractère)
    char status_char = '0' + process->status;
    ecrit_car(lig, 79, status_char, 0, 15); // Dernier caractère pour le statut
}

// Affiche le temps et l'état des processus
void ecrit_temps(char *temps, int taille)
{
    // Affiche l'heure en haut à droite
    for (int i = 0; i < taille; i++)
    {
        ecrit_car(0, 80 - taille + i, temps[i], 0, 15);
    }

    // Affiche l'état des processus
    for (int i = 0; i < PROCESSES_TABLE_SIZE; i++)
    {
        Process *current = processes_table[i];
        if (current)
        {
            affiche_etat(current, 1 + i); // Affiche chaque processus à partir de la ligne 1
        }
    }
}
