#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include "malloc.h"

size_t word_align(size_t n)
{
    return (sizeof(size_t) + n - 1) & -(sizeof(size_t));
}

struct chunk* get_chunk(void *ptr)
{
    char *tmp = ptr;
    void *res = tmp - sizeof(struct chunk);
    return res;
}

char *get_ptr(struct chunk *chunk)
{
    void *tmp = chunk;
    char *res = tmp;
    return res + sizeof(struct chunk);
}

void split_chunk(struct chunk *chunk, size_t size)
{
    if (chunk->size >= size + sizeof(struct chunk) + sizeof(size_t))
    {
        void *tmp = get_ptr(chunk) + size;
        struct chunk *new = tmp;
        new->size = chunk->size - size - sizeof(struct chunk) ;
        new->next = chunk->next;
        new->prev = chunk;
        new->free = FREE;
        if (new->next)
            new->next->prev = new;
        chunk->size = size;
        chunk->next = new;

        new->next_free = meta.first_free;
        meta.first_free = new;
    }
}

void remove_free_list(struct chunk *chunk)
{
    struct chunk *i = meta.first_free;
    if (!i)
        return;
    if (i == chunk)
    {
        meta.first_free = i->next_free;
        return;
    }
    while (i->next_free)
    {
        if (i->next_free == chunk)
        {
            i->next_free = i->next_free->next_free;
            return;
        }
        i = i->next_free;
    }
}
