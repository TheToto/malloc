#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include "malloc.h"

static void *realloc_merge(struct chunk *chunk, size_t size)
{
    remove_free_list(chunk->next);
    chunk->size = chunk->size + chunk->next->size + sizeof(struct chunk);
    chunk->next = chunk->next->next;
    if (chunk->next)
        chunk->next->prev = chunk;
    split_chunk(chunk, size);
    pthread_mutex_unlock(&meta.lock);
    return get_ptr(chunk);
}

    __attribute__((visibility("default")))
void *realloc(void *ptr, size_t size)
{
    if (!ptr)
        return malloc(size);
    if (size <= 0)
    {
        free(ptr);
        return NULL;
    }
    pthread_mutex_lock(&meta.lock);
    size = word_align(size);
    size_t max_size = SIZE_PAGE - 1024;
    struct chunk *chunk = get_chunk(ptr);
    if (chunk->next != NULL && (chunk->next->free & FREE)
        && !(chunk->next->free & FIRST_CHUNK) && size < max_size
        && (chunk->size + chunk->next->size + sizeof(struct chunk) >= size))
    {
        return realloc_merge(chunk, size);
    }
    pthread_mutex_unlock(&meta.lock);
    char *new = malloc(size);
    if (!new)
        return NULL;
    char *old = ptr;
    for (size_t i = 0; i < chunk->size && i < size; i++)
        *(new + i) = *(old + i);
    free(ptr);
    return new;
}
