#include "../include/poggi_fs.h"
#include "../include/poggi_hal.h"
#include "../include/poggi_string.h"

static PoggiFile file_system[MAX_FILES];

static void local_strcpy(char *dest, const char *src)
{
    while ((*dest++ = *src++))
        ;
}

void poggi_fs_init(void)
{
    for (int i = 0; i < MAX_FILES; i++)
        file_system[i].is_used = 0;
    poggi_fs_create("autor.txt", "Acest OS a fost creat de Vasilache.");
    poggi_fs_create("readme.txt", "Poggi OS v0.4 - Ruleaza stabil pe structura HAL.");
}

int poggi_fs_create(const char *name, const char *content)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (!file_system[i].is_used)
        {
            local_strcpy(file_system[i].name, name);
            local_strcpy(file_system[i].content, content);
            file_system[i].size = poggi_strlen(content);
            file_system[i].is_used = 1;
            return 1;
        }
    }
    return 0;
}

void poggi_fs_list(void)
{
    hal_print_string(" Nume Fisier          Dimensiune\n");
    hal_print_string(" -------------------------------\n");
    char size_buf[12];
    int files_found = 0;

    for (int i = 0; i < MAX_FILES; i++)
    {
        if (file_system[i].is_used)
        {
            hal_print_string(" ");
            hal_print_string(file_system[i].name);
            int spaces = 20 - poggi_strlen(file_system[i].name);
            for (int s = 0; s < spaces; s++)
                hal_print_char(' ');

            int num = file_system[i].size;
            int idx = 0;
            if (num == 0)
                size_buf[idx++] = '0';
            while (num > 0)
            {
                size_buf[idx++] = (num % 10) + '0';
                num /= 10;
            }
            size_buf[idx] = '\0';
            for (int start = 0, end = idx - 1; start < end; start++, end--)
            {
                char t = size_buf[start];
                size_buf[start] = size_buf[end];
                size_buf[end] = t;
            }
            hal_print_string(size_buf);
            hal_print_string(" bytes\n");
            files_found++;
        }
    }
    if (files_found == 0)
        hal_print_string(" (Sistemul de fisiere este gol)\n");
}

void poggi_fs_read(const char *name)
{
    for (int i = 0; i < MAX_FILES; i++)
    {
        if (file_system[i].is_used && poggi_strcmp(file_system[i].name, name) == 0)
        {
            hal_print_string(file_system[i].content);
            hal_print_string("\n");
            return;
        }
    }
    hal_print_string("Eroare: Fisierul nu a fost gasit.\n");
}