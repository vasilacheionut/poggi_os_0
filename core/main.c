#include "../include/poggi_hal.h"
#include "../include/poggi_string.h"
#include "../include/poggi_mem.h"
#include "../include/poggi_fs.h"
#include "../include/poggi_task.h"
#include "../include/poggi_ipc.h" // NOU!

static int task_a_counter = 0;

// Funcție ajutătoare de conversie număr -> text
void int_to_string(int num, char *str)
{
    int i = 0;
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    while (num != 0)
    {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }
    str[i] = '\0';
    int start = 0, end = i - 1;
    while (start < end)
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void task_a_func(void)
{
    task_a_counter++;

    // Generăm o dată de test (ex: contorul înmulțit cu 10)
    int secret_data = task_a_counter * 10;

    hal_print_string(" -> [Task A] Am generat valoarea: ");
    char buf[12];
    int_to_string(secret_data, buf);
    hal_print_string(buf);
    hal_print_string(". O trimit prin IPC...\n");

    // Trimitem valoarea în Mailbox-ul Kernelului
    if (poggi_ipc_send(secret_data))
    {
        hal_print_string(" -> [Task A] Mesaj trimis cu succes.\n");
    }
    else
    {
        hal_print_string(" -> [Task A] Mailbox-ul este plin, reincerc tura urmatoare.\n");
    }

    if (task_a_counter >= 3)
    {
        task_a_counter = 0;
        poggi_yield(); // Predăm ștafeta lui Task B
    }
}

void task_b_func(void)
{
    hal_print_string(" -> [Task B] Verific Mailbox-ul kernelului...\n");

    int received_msg = 0;
    // Încercăm să citim din Mailbox
    if (poggi_ipc_receive(&received_msg))
    {
        hal_print_string(" -> [Task B] SUC-CES! Am citit prin IPC valoarea: ");
        char buf[12];
        int_to_string(received_msg, buf);
        hal_print_string(buf);
        hal_print_string("\n");
    }
    else
    {
        hal_print_string(" -> [Task B] Mailbox-ul este gol. Nimic de citit.\n");
    }

    // Îi dăm rândul înapoi lui Task A imediat după verificare
    poggi_yield();
}

// Restul funcțiilor utilitare din sistem
void ptr_to_hexstring(void *ptr, char *str)
{
    unsigned long addr = (unsigned long)ptr;
    char hex_digits[] = "0123456789ABCDEF";
    int i = 0;
    if (addr == 0)
    {
        str[0] = '0';
        str[1] = 'x';
        str[2] = '0';
        str[3] = '\0';
        return;
    }
    while (addr > 0)
    {
        str[i++] = hex_digits[addr % 16];
        addr /= 16;
    }
    str[i++] = 'x';
    str[i++] = '0';
    str[i] = '\0';
    int start = 0, end = i - 1;
    while (start < end)
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

int starts_with(const char *str, const char *prefix)
{
    while (*prefix)
    {
        if (*prefix != *str)
            return 0;
        prefix++;
        str++;
    }
    return 1;
}

int main(void)
{
    char input_buffer[128];
    char num_buffer[32];

    hal_init();
    poggi_mem_init();
    poggi_fs_init();
    poggi_scheduler_init();
    poggi_ipc_init(); // NOU!

    hal_print_string("\n---------------------------------------\n");
    hal_print_string("      Poggi OS Shell - Versiunea 0.7   \n");
    hal_print_string(" Multitasking & Comunicare IPC Active  \n");
    hal_print_string(" Scrieti 'help' pentru lista de comenzi.\n");
    hal_print_string("---------------------------------------\n\n");

    while (1)
    {
        hal_print_string("PoggiOS> ");
        hal_read_line(input_buffer, sizeof(input_buffer));

        if (poggi_strlen(input_buffer) == 0)
            continue;

        if (poggi_strcmp(input_buffer, "help") == 0)
        {
            hal_print_string("Comenzi disponibile in Poggi OS:\n");
            hal_print_string("  help               - Afiseaza acest ajutor\n");
            hal_print_string("  hardware           - Informatii RAM fizica & Uptime\n");
            hal_print_string("  memstatus          - Starea Heap-ului intern\n");
            hal_print_string("  ls                 - Listeaza toate fisierele\n");
            hal_print_string("  cat [nume_fisier]  - Citeste continutul unui fisier\n");
            hal_print_string("  run                - Porneste testul IPC + Multitasking\n");
            hal_print_string("  clear              - Curata ecranul\n");
            hal_print_string("  exit               - Inchide simulatorul\n");
        }
        else if (poggi_strcmp(input_buffer, "hardware") == 0)
        {
            hal_print_string(" Memorie Fizica (Simulata): ");
            int_to_string(hal_get_total_memory(), num_buffer);
            hal_print_string(num_buffer);
            hal_print_string(" MB\n Uptime: ");
            int_to_string(hal_get_uptime(), num_buffer);
            hal_print_string(num_buffer);
            hal_print_string(" secunde\n");
        }
        else if (poggi_strcmp(input_buffer, "memstatus") == 0)
        {
            hal_print_string("--- STARE HEAP POGGI OS ---\n");
            hal_print_string(" Memorie Utilizata: ");
            int_to_string(poggi_mem_used(), num_buffer);
            hal_print_string(num_buffer);
            hal_print_string(" bytes\n Memorie Libera:    ");
            int_to_string(poggi_mem_free(), num_buffer);
            hal_print_string(num_buffer);
            hal_print_string(" bytes\n");
        }
        else if (poggi_strcmp(input_buffer, "run") == 0)
        {
            hal_print_string("Inregistram task-urile in Scheduler...\n");
            poggi_scheduler_init();
            poggi_ipc_init(); // Resetam si cutia postala

            poggi_task_create("Procesul_A_Calcul", task_a_func);
            poggi_task_create("Procesul_B_Monitor", task_b_func);

            poggi_scheduler_start();
        }
        else if (poggi_strcmp(input_buffer, "ls") == 0)
        {
            poggi_fs_list();
        }
        else if (starts_with(input_buffer, "cat "))
        {
            poggi_fs_read(input_buffer + 4);
        }
        else if (poggi_strcmp(input_buffer, "clear") == 0)
        {
            hal_print_string("\033[H\033[J");
        }
        else if (poggi_strcmp(input_buffer, "exit") == 0)
        {
            break;
        }
        else
        {
            hal_print_string("Eroare: Comanda '");
            hal_print_string(input_buffer);
            hal_print_string("' nu este recunoscuta.\n");
        }
    }
    return 0;
}