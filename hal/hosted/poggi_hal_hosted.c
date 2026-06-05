#include "../../include/poggi_hal.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static time_t start_time;

void hal_init(void)
{
    start_time = time(NULL);
    printf("[HAL Hosted] Inițializare sistem pe Debian 13...\n");
}

void hal_print_char(char c)
{
    putchar(c);
}

void hal_print_string(const char *str)
{
    printf("%s", str);
}

char hal_get_char(void)
{
    return getchar();
}

void hal_read_line(char *buffer, int max_len)
{
    if (fgets(buffer, max_len, stdin) != NULL)
    {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n')
        {
            buffer[len - 1] = '\0';
        }
    }
}

int hal_get_uptime(void)
{
    return (int)(time(NULL) - start_time);
}

int hal_get_total_memory(void)
{
    return 4096;
}