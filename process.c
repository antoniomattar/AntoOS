#include "process.h"
#include "tinyalloc.h"
#include "string.h"
#include "stdio.h"
#include "it.h"
#include "time.h"

Process *processes_table[PROCESSES_TABLE_SIZE];

Process *head_activable_processes = NULL;
Process *tail_activable_processes = NULL;

Process *head_waiting_processes = NULL;
Process *tail_waiting_processes = NULL;

Process *head_zombies_processes = NULL;
Process *tail_zombies_processes = NULL;

Process *current_process = NULL;

static uint32_t current_process_pid = 0;

// PROCESSES ------------------------------------------------

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
        dors(2);
    }
    fin_processus();
}

void proc2(void)
{
    for (int i = 0; i < 2; ++i)
    {
        printf("[%s] pid = %i [temps = %u]\n", mon_nom(), mon_pid(), nbr_secondes());
        dors(3);
    }
    fin_processus();
}

void proc3(void)
{
    for (int i = 0; i < 2; ++i)
    {
        printf("[%s] pid = %i [temps = %u]\n", mon_nom(), mon_pid(), nbr_secondes());
        dors(5);
    }
    fin_processus();
}
// ----------------------------------------------------------

int32_t mon_pid()
{
    return current_process->pid;
}

char *mon_nom()
{
    return current_process->name;
}

void init_processes_table()
{
    cree_processus(idle, "idle");

    // set the status of the first process to PICKED
    processes_table[0]->status = PICKED;
    current_process = processes_table[0];

    cree_processus(proc1, "proc1");
    cree_processus(proc2, "proc2");
    cree_processus(proc3, "proc3");
}

int32_t cree_processus(void (*code)(void), char *nom)
{
    if (current_process_pid >= PROCESSES_TABLE_SIZE)
    {
        printf("No more space for new processes\n");
        return -1;
    }
    Process *new_process = malloc(sizeof(Process));
    new_process->pid = current_process_pid;
    strcpy(new_process->name, nom);
    new_process->status = AVAILABLE;
    new_process->call_stack[CALL_STACK_SIZE - 1] = (uint32_t)code;
    new_process->saved_registers[1] = (uint32_t) & (new_process->call_stack[CALL_STACK_SIZE - 1]);
    // new_process->wakeup_time = 0;

    processes_table[current_process_pid] = new_process;
    if (current_process_pid != 0)
    {
        insert_to_activable_processes(new_process);
    }

    return current_process_pid++;
}

void insert_to_activable_processes(Process *process)
{
    if (head_activable_processes == NULL)
    {
        head_activable_processes = process;
        tail_activable_processes = process;
        return;
    }

    tail_activable_processes->next = process;
    tail_activable_processes = process;
    process->status = AVAILABLE;
}

Process *extract_first_activable_process()
{
    if (head_activable_processes == NULL)
        return NULL;

    Process *process = head_activable_processes;
    if (head_activable_processes->next)
    {
        head_activable_processes = head_activable_processes->next;
    }
    else
    {
        head_activable_processes = NULL;
        tail_activable_processes = NULL;
    }

    process->status = PICKED;
    process->next = NULL;
    return process;
}

void insert_to_waiting_processes(Process *process)
{
    if (head_waiting_processes == NULL)
    {
        head_waiting_processes = process;
        tail_waiting_processes = process;
        return;
    }

    tail_waiting_processes->next = process;
    tail_waiting_processes = process;
    process->status = WAITING;
}

Process *extract_first_waiting_process()
{
    // check if activable_processes is empty
    if (head_waiting_processes == NULL)
        return NULL;

    Process *process = head_waiting_processes;
    if (head_waiting_processes->next)
    {
        head_waiting_processes = head_waiting_processes->next;
    }
    else
    {
        head_waiting_processes = NULL;
        tail_waiting_processes = NULL;
    }

    process->status = AVAILABLE;
    return process;
}

void insert_to_zombies_processes(Process *process)
{
    if (head_zombies_processes == NULL)
    {
        head_zombies_processes = process;
        tail_zombies_processes = process;
        return;
    }

    tail_zombies_processes->next = process;
    tail_zombies_processes = process;
    process->status = ZOMBIE;
}

Process *extract_first_zombie_process()
{
    // check if activable_processes is empty
    if (head_zombies_processes == NULL)
        return NULL;

    Process *process = head_zombies_processes;
    if (head_zombies_processes->next)
    {
        head_zombies_processes = head_zombies_processes->next;
    }
    else
    {
        head_zombies_processes = NULL;
        tail_zombies_processes = NULL;
    }

    process->status = AVAILABLE;
    return process;
}

void wake_up_waiting_processes()
{
    Process *process = head_waiting_processes;
    Process *previous_process = NULL;
    while (process != NULL)
    {
        if (process->wakeup_time <= nbr_secondes())
        {
            if (previous_process == NULL)
            {
                head_waiting_processes = process->next;
            }
            else
            {
                previous_process->next = process->next;
            }
            insert_to_activable_processes(process);
        }
        previous_process = process;
        process = process->next;
    }
}

void ordonnance()
{
    Process *process_to_be_slept = current_process;
    Process *process_to_be_activated;

    wake_up_waiting_processes();

    process_to_be_activated = extract_first_activable_process();

    if (process_to_be_activated == NULL)
    {
        process_to_be_activated = processes_table[0];
    }
    if (current_process == NULL)
        current_process = processes_table[0];
    else if (current_process->status == ZOMBIE)
    {
        ordonnance();
    }

    current_process = process_to_be_activated;
    // if (current_process->pid != 0)
    //     insert_to_waiting_processes(process_to_be_slept);

    ctx_sw(((uint32_t *)process_to_be_slept->saved_registers),
           ((uint32_t *)current_process->saved_registers));
}

void dors(uint32_t sleep_duration)
{
    Process *process_to_be_slept = current_process;

    process_to_be_slept->wakeup_time = nbr_secondes() + sleep_duration * 60;
    process_to_be_slept->status = WAITING;

    insert_to_waiting_processes(process_to_be_slept);

    current_process = extract_first_activable_process();
    if (current_process == NULL)
        current_process = processes_table[0];

    ctx_sw((uint32_t *)(process_to_be_slept->saved_registers),
           (uint32_t *)(current_process->saved_registers));
}

void fin_processus()
{
    // si le processus courant est le processus idle, on ne fait rien
    if (current_process->pid == 0)
    {
        return;
    }
    else
    {
        // sinon, on termine le processus et on le met en status de zombie
        insert_to_zombies_processes(current_process);
    }
    ordonnance();
}
