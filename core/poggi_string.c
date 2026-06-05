#include "../include/poggi_string.h"

int poggi_strlen(const char *str)
{
    int len = 0;
    while (str[len] != '\0')
        len++;
    return len;
}

int poggi_strcmp(const char *s1, const char *s2)
{
    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char *poggi_strchr(const char *str, int character)
{
    while (*str != '\0')
    {
        if (*str == character)
        {
            return (char *)str;
        }
        str++;
    }
    return (void *)0;
}