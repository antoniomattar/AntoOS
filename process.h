#ifndef PROCESS_H
#define PROCESS_H

#include "inttypes.h"

// Taille de la pile et de la table des processus
#define CALL_STACK_SIZE 512
#define PROCESSES_TABLE_SIZE 8

// États possibles des processus
typedef enum
{
    PICKED,
    AVAILABLE,
    WAITING,
    ZOMBIE
} State;

// Structure du processus
typedef struct process
{
    int32_t pid;                      // Identifiant unique du processus
    char name[10];                    // Nom du processus
    State status;                     // État du processus
    uint32_t saved_registers[5];      // Registres sauvegardés
    uint32_t call_stack[CALL_STACK_SIZE]; // Pile d'appels du processus
    uint32_t wakeup_time;             // Heure de réveil (pour les processus en attente)
    struct process *next;             // Pointeur vers le prochain processus dans une file
} Process;

// Déclarations externes pour les variables globales
extern Process *processes_table[PROCESSES_TABLE_SIZE];

extern Process *head_activable_processes;
extern Process *tail_activable_processes;

extern Process *head_waiting_processes;
extern Process *tail_waiting_processes;

extern Process *head_zombies_processes;
extern Process *tail_zombies_processes;

extern Process *current_process;

// Déclarations des fonctions externes
int32_t mon_pid();                     // Retourne le PID du processus courant
char *mon_nom();                       // Retourne le nom du processus courant

void init_processes_table();           // Initialise la table des processus
int32_t cree_processus(void (*code)(void), char *nom); // Crée un nouveau processus

Process *extract_first_activable_process(); // Extrait le premier processus activable
void insert_to_activable_processes(Process *process); // Insère un processus dans la liste des activables

Process *extract_first_waiting_process();   // Extrait le premier processus en attente
void insert_to_waiting_processes(Process *process);   // Insère un processus dans la liste des attentes
void wake_up_waiting_processes();          // Réveille les processus en attente si le temps est écoulé

Process *extract_first_zombie_process();   // Extrait le premier processus zombie
void insert_to_zombies_processes(Process *process);   // Insère un processus dans la liste des zombies

void idle(void);                           // Processus idle

void proc1(void);
void proc2(void);
void proc3(void);

void ordonnance();                         // Planifie et active le prochain processus
void dors(uint32_t nbr_secs);              // Met le processus courant en attente pour un certain temps
void fin_processus();                      // Termine le processus courant

// Déclaration pour le changement de contexte
extern void ctx_sw(uint32_t *old_sp, uint32_t *new_sp);

#endif // PROCESS_H
