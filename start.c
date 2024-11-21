#include "cpu.h"
#include <inttypes.h>
#include <stdio.h>
#include "ecran.h"
#include "time.h"
#include "it.h"
#include "horloge.h"
#include "process.h"

// on peut s'entrainer a utiliser GDB avec ce code de base
// par exemple afficher les valeurs de x, n et res avec la commande display

// une fonction bien connue
uint32_t fact(uint32_t n)
{
    uint32_t res;
    if (n <= 1)
    {
        res = 1;
    }
    else
    {
        res = fact(n - 1) * n;
    }
    return res;
}


void kernel_start(void)
{
    efface_ecran();
    // TP 1 ----------------------------------------

    // uint32_t x = fact(5);
    // quand on saura gerer l'ecran, on pourra afficher x
    // printf("%d \n",x);
    // printf("hi \nI am Antonio, low-level coding is fucking cool!");

    // TP 2 ----------------------------------------

    // intialisations
    init_traitant_IT(32, traitant_IT_32);
    set_clock_freq(CLOCKFREQ);
    masque_IRQ(0, 0);

    // démasquage des interruptions externes
    // sti(); // desactive pour le moment pr garantir pas avoir d'interferences

    init_processes_table();
    idle();

    while (1)
    {
        // cette fonction arrete le processeur
        hlt();
    }
}
