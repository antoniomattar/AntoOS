#include "inttypes.h"
#define CALL_STACK_SIZE 512
#define PROCESSES_TABLE_SIZE 8

enum State
{
    PICKED,
    AVAILABLE,
    WAITING,
    ZOMBIE
};

typedef struct process
{
    int32_t pid;
    char name[10];
    enum State status;
    // reg0:ebx,reg1: esp, reg2: ebp, reg3: esi, reg4: edi
    uint32_t saved_registers[5];
    uint32_t call_stack[CALL_STACK_SIZE];
    uint32_t wakeup_time;
    struct process *next;
} Process;

extern void ctx_sw(uint32_t *old_sp, uint32_t *new_sp);

extern Process *processes_table[PROCESSES_TABLE_SIZE];

extern Process *head_activable_processes;
extern Process *tail_activable_processes;

extern Process *head_waiting_processes;
extern Process *tail_waiting_processes;

extern Process *head_zombies_processes;
extern Process *tail_zombies_processes;

extern Process *current_process;

int32_t mon_pid();
char *mon_nom();

extern void init_processes_table();
int32_t cree_processus(void (*code)(void), char *nom);

Process *extract_first_activable_process();
void insert_to_activable_processes(Process *process);

Process *extract_first_waiting_process();
void insert_to_waiting_processes(Process *process);
void wake_up_waiting_processes();

Process *extract_first_zombie_process();
void insert_to_zombies_processes(Process *process);

extern void idle(void);
extern void proc1(void);
extern void proc2(void);
extern void proc3(void);

void ordonnance();
void dors(uint32_t nbr_secs);
void fin_processus();