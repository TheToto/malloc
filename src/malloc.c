#define _GNU_SOURCE
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include "malloc.h"

struct meta meta =
{
    NULL,
    NULL,
    PTHREAD_MUTEX_INITIALIZER
};

static struct chunk *allocate_page(void)
{
    struct chunk *new_page = mmap(NULL, SIZE_PAGE,
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (new_page != MAP_FAILED)
    {
        new_page->next = meta.my_heap;
        if (new_page->next)
            new_page->next->prev = new_page;
        new_page->prev = NULL;
        new_page->free = FREE | FIRST_CHUNK;
        new_page->size = SIZE_PAGE - sizeof(struct chunk);
        meta.my_heap = new_page;

        return new_page;
    }
    return NULL;
}

static struct chunk *ask_chunk(size_t size)
{
    struct chunk *i = meta.first_free;
    if (!i)
        return NULL;
    if (i->size >= size)
    {
        struct chunk *res = i;
        meta.first_free = i->next_free;
        return res;

    }
    while (i->next_free)
    {
        if (i->next_free->size >= size)
        {
            struct chunk *res = i->next_free;
            i->next_free = res->next_free;
            return res;
        }
        i = i->next_free;
    }
    return NULL;
}

static void *allocate_big(size_t size)
{
    struct chunk *chunk = mmap(NULL, size + sizeof(struct chunk),
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (chunk == MAP_FAILED)
        return NULL;
    chunk->next = NULL;
    chunk->prev = NULL;
    chunk->size = size;
    chunk->free = ALONE_CHUNK;
    pthread_mutex_unlock(&meta.lock);
    return get_ptr(chunk);
}

    __attribute__((visibility("default")))
void *malloc(size_t size)
{
    if (size <= 0)
        return NULL;
    if (!meta.my_heap && pthread_mutex_init(&meta.lock, NULL) != 0)
        return NULL;
    pthread_mutex_lock(&meta.lock);
    size = word_align(size);
    size_t max_size = SIZE_PAGE - 1024;
    if (size >= max_size)
        return allocate_big(size);
    struct chunk *ask = ask_chunk(size);
    if (!ask)
    {
        ask = allocate_page();
        if (ask == NULL)
        {
            pthread_mutex_unlock(&meta.lock);
            return NULL;
        }
    }
    split_chunk(ask, size);

    ask->free = ask->free & ~FREE;
    pthread_mutex_unlock(&meta.lock);
    return get_ptr(ask);
}
