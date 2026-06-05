#include "../include/poggi_mem.h"

#define HEAP_SIZE (128 * 1024)
static char poggi_heap[HEAP_SIZE];
static int next_free_index = 0;

void poggi_mem_init(void)
{
    next_free_index = 0;
}

void *poggi_alloc(int size)
{
    if (size % 4 != 0)
        size = size + (4 - (size % 4));
    if (next_free_index + size > HEAP_SIZE)
        return (void *)0;
    void *allocated_ptr = (void *)&poggi_heap[next_free_index];
    next_free_index += size;
    return allocated_ptr;
}

int poggi_mem_used(void) { return next_free_index; }
int poggi_mem_free(void) { return HEAP_SIZE - next_free_index; }