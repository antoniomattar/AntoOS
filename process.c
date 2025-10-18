#include "process.h"
#include "tinyalloc.h"
#include "string.h"
#include "stdio.h"
#include "it.h"
#include "time.h"

// Variables globales pour la gestion des processus
Process *processes_table[PROCESSES_TABLE_SIZE];

Process *head_activable_processes = NULL;
Process *tail_activable_processes = NULL;

Process *head_waiting_processes = NULL;
Process *tail_waiting_processes = NULL;

Process *head_zombies_processes = NULL;
Process *tail_zombies_processes = NULL;

Process *current_process = NULL;

static uint32_t current_process_pid = 0;

// Initialisation et gestion des processus
void idle(void)
{
    for (;;)
    {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        sti();
        hlt();
        cli();
    }
}

void proc1(void)
{
    for (int i = 0; i < 2; ++i)
    {
        printf("[%s] pid = %i [temps = %u]\n", mon_nom(), mon_pid(), nbr_secondes());
        dors(4);
    }
    fin_processus();
}

void proc2(void)
{
    for (int i = 0; i < 2; ++i)
    {
        printf("[%s] pid = %i [temps = %u]\n", mon_nom(), mon_pid(), nbr_secondes());
        dors(5);
    }
    fin_processus();
}

void proc3(void)
{
    for (int i = 0; i < 2; ++i)
    {
        printf("[%s] pid = %i [temps = %u]\n", mon_nom(), mon_pid(), nbr_secondes());
        dors(6);
    }
    fin_processus();
}

int32_t mon_pid()
{
    return current_process ? current_process->pid : -1;
}

char *mon_nom()
{
    return current_process ? current_process->name : "undefined";
}

void init_processes_table()
{
    cree_processus(idle, "idle");
    processes_table[0]->status = PICKED;
    current_process = processes_table[0];

    // Crée les autres processus
    if (cree_processus(proc1, "proc1") < 0) return;
    if (cree_processus(proc2, "proc2") < 0) return;
    if (cree_processus(proc3, "proc3") < 0) return;
}


int32_t cree_processus(void (*code)(void), char *nom)
{
    if (current_process_pid >= PROCESSES_TABLE_SIZE)
    {
        printf("Pas d'espace pour de nouveaux processus\n");
        return -1;
    }

    Process *new_process = (Process *)malloc(sizeof(Process));
    if (!new_process)
    {
        printf("Échec d'allocation mémoire\n");
        return -1;
    }

    // Initialisation du processus
    new_process->pid = current_process_pid;
    strncpy(new_process->name, nom, sizeof(new_process->name) - 1);
    new_process->name[sizeof(new_process->name) - 1] = '\0';
    new_process->status = AVAILABLE;
    new_process->wakeup_time = 0;
    new_process->next = NULL;
    memset(new_process->call_stack, 0, sizeof(new_process->call_stack));
    memset(new_process->saved_registers, 0, sizeof(new_process->saved_registers));
    new_process->call_stack[CALL_STACK_SIZE - 1] = (uint32_t)code;
    new_process->saved_registers[1] = (uint32_t)&new_process->call_stack[CALL_STACK_SIZE - 1];

    processes_table[current_process_pid] = new_process;

    if (current_process_pid != 0)
    {
        insert_to_activable_processes(new_process);
    }

    return current_process_pid++;
}

void insert_to_activable_processes(Process *process)
{
    if (!process)
        return;

    process->status = AVAILABLE;
    process->next = NULL;

    if (!head_activable_processes)
    {
        head_activable_processes = tail_activable_processes = process;
    }
    else
    {
        tail_activable_processes->next = process;
        tail_activable_processes = process;
    }
}

Process *extract_first_activable_process()
{
    if (!head_activable_processes)
        return NULL;

    Process *process = head_activable_processes;
    head_activable_processes = head_activable_processes->next;

    if (!head_activable_processes)
        tail_activable_processes = NULL;

    process->status = PICKED;
    process->next = NULL;
    return process;
}

void insert_to_waiting_processes(Process *process)
{
    if (!process)
        return;

    process->status = WAITING;
    process->next = NULL;

    if (!head_waiting_processes)
    {
        head_waiting_processes = tail_waiting_processes = process;
    }
    else
    {
        tail_waiting_processes->next = process;
        tail_waiting_processes = process;
    }
}

void wake_up_waiting_processes()
{
    Process *process = head_waiting_processes;
    Process *prev = NULL;

    while (process)
    {
        if (process->wakeup_time <= nbr_secondes())
        {
            // Réveille le processus
            if (prev)
            {
                prev->next = process->next;
            }
            else
            {
                head_waiting_processes = process->next;
            }

            if (process == tail_waiting_processes)
            {
                tail_waiting_processes = prev;
            }

            Process *next = process->next;
            process->next = NULL;
            insert_to_activable_processes(process); // Ajoute au processus activable
            process = next; // Passe au processus suivant
        }
        else
        {
            // Passe au suivant sans le réveiller
            prev = process;
            process = process->next;
        }
    }
}


Process *extract_first_zombie_process()
{
    if (!head_zombies_processes)
        return NULL;

    Process *zombie_process = head_zombies_processes;
    head_zombies_processes = head_zombies_processes->next;

    if (!head_zombies_processes)
        tail_zombies_processes = NULL;

    zombie_process->status = AVAILABLE;
    zombie_process->next = NULL;

    return zombie_process;
}

void insert_to_zombies_processes(Process *process)
{
    if (!process)
        return;

    process->status = ZOMBIE;
    process->next = NULL;

    if (!head_zombies_processes)
    {
        head_zombies_processes = tail_zombies_processes = process;
    }
    else
    {
        tail_zombies_processes->next = process;
        tail_zombies_processes = process;
    }
}

void fin_processus()
{
    if (!current_process || current_process->pid == 0)
        return;

    current_process->status = ZOMBIE;
    insert_to_zombies_processes(current_process);
    ordonnance();
}

void ordonnance()
{
    wake_up_waiting_processes();

    Process *next_process = extract_first_activable_process();
    if (!next_process)
    {
        next_process = processes_table[0]; // Revenir au processus idle
    }

    Process *old_process = current_process;
    current_process = next_process;

    // Vérifie que le processus précédent est en état valide
    if (old_process && old_process->status != ZOMBIE)
    {
        insert_to_activable_processes(old_process);
    }

    // Assure que le changement de contexte est appelé avec des registres valides
    ctx_sw((uint32_t *)old_process->saved_registers, (uint32_t *)current_process->saved_registers);
}


void dors(uint32_t sleep_duration)
{
    if (!current_process || current_process->pid == 0)
        return;

    current_process->wakeup_time = nbr_secondes() + sleep_duration * 60;
    current_process->status = WAITING;
    insert_to_waiting_processes(current_process);

    current_process = extract_first_activable_process();
    if (!current_process)
    {
        current_process = processes_table[0]; // Revenir au processus idle si aucun activable
    }

    ctx_sw((uint32_t *)processes_table[current_process->pid]->saved_registers,
           (uint32_t *)current_process->saved_registers);
}

