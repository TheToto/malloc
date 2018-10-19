#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include "malloc.h"

static struct chunk *merge_chunk(struct chunk *chunk)
{
    int in_free = 0;
    if (chunk->prev && (chunk->prev->free & FREE)
        && !(chunk->free & FIRST_CHUNK)) // !!!!
    {
        chunk->prev->size = chunk->size + chunk->prev->size
            + sizeof(struct chunk);
        chunk->prev->next = chunk->next;
        if (chunk->next)
            chunk->next->prev = chunk->prev;
        chunk = chunk->prev;
        in_free = 1;
    }
    if (chunk->next && (chunk->next->free & FREE)
        && !(chunk->next->free & FIRST_CHUNK))
    {
        remove_free_list(chunk->next);

        chunk->size = chunk->size + chunk->next->size + sizeof(struct chunk);
        chunk->next = chunk->next->next;
        if (chunk->next)
            chunk->next->prev = chunk;
    }
    if (in_free == 0)
    {
        chunk->next_free = meta.first_free;
        meta.first_free = chunk;
    }
    return chunk;
}

__attribute__((visibility("default")))
void free(void *ptr)
{
    if (!ptr)
        return;
    pthread_mutex_lock(&meta.lock);
    struct chunk *chunk = get_chunk(ptr);
    if (chunk->free & ALONE_CHUNK)
    {
        munmap(chunk, chunk->size + sizeof(struct chunk));
        pthread_mutex_unlock(&meta.lock);
        return;
    }
    chunk->free |= FREE;
    chunk = merge_chunk(chunk);
    size_t empty_size = SIZE_PAGE - sizeof(struct chunk);
    if ((chunk->free & FIRST_CHUNK) && chunk->size == empty_size)
    {
        if (meta.my_heap == chunk)
            meta.my_heap = chunk->next;
        if (chunk->prev)
            chunk->prev->next = chunk->next;
        if (chunk->next)
            chunk->next->prev = chunk->prev;
        remove_free_list(chunk);
        munmap(chunk, SIZE_PAGE);
    }
    pthread_mutex_unlock(&meta.lock);
    if (!meta.my_heap)
        pthread_mutex_destroy(&meta.lock);
}

