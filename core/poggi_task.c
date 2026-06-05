#include "../include/poggi_task.h"
#include "../include/poggi_hal.h"

static PoggiTask task_table[MAX_TASKS];
static int task_count = 0;
static int current_task_id = -1;

void poggi_scheduler_init(void)
{
    task_count = 0;
    current_task_id = -1;
    for (int i = 0; i < MAX_TASKS; i++)
    {
        task_table[i].id = -1;
        task_table[i].state = TASK_SLEEPING;
    }
}

int poggi_task_create(const char *name, void (*func)(void))
{
    if (task_count >= MAX_TASKS)
        return -1;

    task_table[task_count].id = task_count;
    task_table[task_count].name = name;
    task_table[task_count].task_function = func;
    task_table[task_count].state = TASK_READY;

    task_count++;
    return task_count - 1;
}

void poggi_yield(void)
{
    if (task_count <= 1)
        return;

    // Doar calculăm cine urmează, FĂRĂ să apelăm funcția direct!
    int next_task_id = (current_task_id + 1) % task_count;

    task_table[current_task_id].state = TASK_READY;
    current_task_id = next_task_id;
    task_table[current_task_id].state = TASK_RUNNING;

    hal_print_string("\n[Kernel] Context Switch -> Schimbam pe: ");
    hal_print_string(task_table[current_task_id].name);
    hal_print_string("\n");
}

void poggi_scheduler_start(void)
{
    if (task_count == 0)
        return;

    hal_print_string("[Kernel] Se porneste planificatorul de task-uri...\n");
    current_task_id = 0;
    task_table[current_task_id].state = TASK_RUNNING;

    // Super-bucla Scheduler-ului (Aici controlăm execuția curat)
    // Deoarece nu avem încă stive ASM separate pentru fiecare task,
    // lăsăm scheduler-ul să apeleze bucăți mici din task-uri.
    while (1)
    {
        task_table[current_task_id].task_function();
    }
}