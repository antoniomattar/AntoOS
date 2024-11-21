#include "time.h"
#include "ecran.h"
#include "horloge.h"
#include "process.h"
#include "cpu.h"
#include "stdio.h"

static uint32_t cpt = 0;
static uint32_t sec = 0;
static uint32_t mins = 0;
static uint32_t hours = 0;


void tic_PIT(void)
{
    // signaler au contrôleur d’interruptions
    outb(0x20, 0x20);
    ++cpt;

    ++sec;
    // HH:MM:SS
    if (sec == 60)
    {
        sec = 0;
        ++mins;
    }
    if (mins == 60)
    {
        mins = 0;
        ++hours;
    }
    if (hours == 24)
    {
        hours = 0;
    }

    char temps[9];

    sprintf(temps, "%02d:%02d:%02d", hours, mins, sec);

    ecrit_temps(temps, 9);

    ordonnance();
}

uint32_t nbr_secondes(void)
{
    return cpt;
}