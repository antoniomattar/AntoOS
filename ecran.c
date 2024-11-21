//
// Created by mattara on 05/09/24.
//
#include "ecran.h"
#include "process.h"

#include <inttypes.h>
#include "cpu.h"
#include "ecran.h"
#include <string.h>

uint16_t COLONNE=0;
uint16_t LIGNE=0;

uint16_t *ptr_mem(uint32_t lig, uint32_t col){
    return (uint16_t*) (0xB8000 + 2 * (lig * 80 + col));
}


void ecrit_car(uint32_t lig, uint32_t col, char c, uint8_t bg_color, uint8_t color){
    uint16_t* pointeur = ptr_mem(lig,col);
    if (bg_color<=8)
    {
        if (color<=15)
        {
            *pointeur =(0b0<<15) + ((uint16_t)bg_color<<12)+ ((uint16_t) color<<8) + c;
            // *pointeur = ((0b0<<3|bg_color&)<<3|color)<<8|c; // mets le chiffre en binaire puis le caractère (car 0 ou c =c)
        }
        else{
            *pointeur =(0b0<<15) + ((uint16_t)bg_color<<12)+ ((uint16_t) 0b111<<8) + c;
        }
    }
    else{
        if (color<=15)
        {
            *pointeur =(0b0<<15) + ((uint16_t) color<<8) + c;
            // *pointeur = ((0b0<<3|bg_color&)<<3|color)<<8|c; // mets le chiffre en binaire puis le caractère (car 0 ou c =c)
        }
        else{
            *pointeur =(0b0<<15) + ((uint16_t) 0b111<<8)+ c;
        }
    }

}

void efface_ecran(void){
    for (uint8_t i = 0; i < 25; i++)
    {
        for (uint8_t j = 0; j < 80; j++)
        {
            // pour chaque caractères à l'ecran met un espace
            ecrit_car(i,j,' ',0,15);
        }
    }
}

void place_curseur(uint32_t lig, uint32_t col){
    COLONNE=col;
    LIGNE=lig;
    uint16_t nombre = (uint16_t) (col+lig*80);
    outb((uint8_t) 0x0F,(uint16_t) 0x3D4); // on envoie la partie basse
    outb((uint8_t) nombre,(uint16_t) 0x3D5);
    outb((uint8_t) 0x0E,(uint16_t) 0x3D4); // envoie la partie Haute
    outb((uint8_t) (nombre >> 8),(uint16_t) 0x3D5);
}

void traite_car(char c){
    if ((c>31) && (c<127)) // si c'est un caractère ASCII
    {
        ecrit_car(LIGNE,COLONNE,c,0,15);
        COLONNE++;
        if (COLONNE==80)
        {
            if (LIGNE>=24)
            {
                defilement();
            }
            traite_car(10);
        }
    }
    else{
        switch (c)
        {
            case 8: // \b : Recule le curseur d’une colonne (si colonne̸ = 0)
                if(COLONNE!=0){
                    place_curseur(LIGNE,COLONNE-1);
                }
                break;
            case 9: // \t Avance à la prochaine tabulation (colonnes 0, 8, 16, ..., 72, 79)
                if(COLONNE<80){
                    place_curseur(LIGNE,(((COLONNE)/8)+1)*8);
                }
                else{
                    place_curseur(LIGNE,79);
                }
                break;
            case 10: // \n Déplace le curseur sur la ligne suivante, colonne 0
                if(LIGNE<25){
                    place_curseur(LIGNE+1,0);
                }
                break;
            case 12: // \f Efface l’écran et place le curseur sur la colonne 0 de la ligne 0
                efface_ecran();
                place_curseur(0,0);
                break;
            case 13: // \r Déplace le curseur sur la ligne actuelle, colonne 0
                place_curseur(LIGNE,0);
                break;
            default:
                break;
        }
    }
}

void defilement(void){
    for (uint8_t ligne = 0; ligne < 25; ligne++) // pour chaque ligne
    {
        memmove(ptr_mem(ligne,0),ptr_mem(ligne+1,0),80*2); // on deplace la ligne d'apres sur celle la
    }
    memset(ptr_mem(25,0),20,80*2); // efface la dernière ligne
    LIGNE--;
    place_curseur(LIGNE,COLONNE); // remonte le curseur
}

void console_putbytes(const char *s, int len){
    for (int i = 0; i < len; i++)
    {
        traite_car(s[i]);
        place_curseur(LIGNE,COLONNE);
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