#define _GNU_SOURCE

#include <stddef.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

#include <stdio.h>

#define SIZE_PAGE sysconf(_SC_PAGESIZE)

enum free_state
{
    FREE = 1,
    FIRST_CHUNK = 2,
    ALONE_CHUNK = 4
};

struct chunk
{
    size_t size;
    char free;
    struct chunk *next;
    struct chunk *prev;
};

void *my_heap = NULL;
pthread_mutex_t lock;

size_t word_align(size_t n)
{
    return (sizeof(size_t) + n - 1) & -(sizeof(size_t)); 
}

struct chunk *allocate_page(struct chunk *prev_page)
{
    struct chunk *new_page = mmap(NULL, SIZE_PAGE,
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (new_page != MAP_FAILED)
    {
        new_page->next = NULL;
        new_page->prev = prev_page;
        new_page->free = FREE | FIRST_CHUNK;
        new_page->size = SIZE_PAGE - sizeof(struct chunk);
        if (prev_page)
            prev_page->next = new_page;
        if (!prev_page && !my_heap)
            my_heap = new_page;
        return new_page;
    }
    return NULL;
}

static struct chunk* get_chunk(void *ptr)
{
    char *tmp = ptr;
    void *res = tmp - sizeof(struct chunk);
    return res;
}

static char *get_ptr(struct chunk *chunk)
{
    void *tmp = chunk;
    char *res = tmp;
    return res + sizeof(struct chunk);
}


struct chunk *ask_chunk(size_t size)
{
    struct chunk *i = my_heap;
    if (!i)
        return NULL;
    while (i->next)
    {
        if ((i->next->free & FREE) && i->next->size >= size)
            return i->next;
        i = i->next;
    }
    return i;
}

static void split_chunk(struct chunk *chunk, size_t size)
{
    if (chunk->size >= size + sizeof(struct chunk) + sizeof(size_t))
    {
        void *tmp = get_ptr(chunk) + size;
        struct chunk *new = tmp;
        new->size = chunk->size - size - sizeof(struct chunk) ;
        new->next = chunk->next;
        new->prev = chunk;
        new->free = FREE;
        chunk->size = size;
        chunk->next = new;
    }
}

static void *allocate_big(size_t size)
{
    struct chunk *chunk = mmap(NULL, size + sizeof(struct chunk),
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    chunk->next = NULL;
    chunk->prev = NULL;
    chunk->size = size;
    chunk->free = ALONE_CHUNK;
    pthread_mutex_unlock(&lock);
    return get_ptr(chunk);
}

/*void print_all(void)
{
    struct chunk *chunk = my_heap;
    printf("start %ld\n", sizeof(struct chunk));
    while (chunk)
    {
        printf("chunk %p : %d : %ld\n", get_ptr(chunk), chunk->free, chunk->size);
        if (chunk->free & FREE)
            printf("yes free\n");
        chunk = chunk->next;
    }
    printf("end\n");
}*/

    __attribute__((visibility("default")))
void *malloc(size_t size)
{
    if (size <= 0)
        return NULL;
    if (!my_heap && pthread_mutex_init(&lock, NULL) != 0)
        return NULL;
    pthread_mutex_lock(&lock);
    size = word_align(size);
    size_t max_size = SIZE_PAGE - 1024;
    if (size >= max_size)
        return allocate_big(size);
    struct chunk *ask = ask_chunk(size);
    if (ask && (ask->free & FREE) && ask->size >= size)
    {
        split_chunk(ask, size);
    }
    else
    {
        ask = allocate_page(ask);
        if (ask == NULL)
        {
            pthread_mutex_unlock(&lock);
            return NULL;
        }
        split_chunk(ask, size);
    }
    ask->free = ask->free & ~FREE;
    pthread_mutex_unlock(&lock);
    return get_ptr(ask);
}

static struct chunk *merge_chunk(struct chunk *chunk)
{
    if (chunk->prev && (chunk->prev->free & FREE)
        && !(chunk->free & FIRST_CHUNK))
    {
        chunk = chunk->prev;
        chunk->size = chunk->size + chunk->next->size + sizeof(struct chunk);
        chunk->next = chunk->next->next;
        if (chunk->next)
            chunk->next->prev = chunk;
    }
    if (chunk->next && (chunk->next->free & FREE)
        && !(chunk->next->free & FIRST_CHUNK))
    {
        chunk->size = chunk->size + chunk->next->size + sizeof(struct chunk);
        chunk->next = chunk->next->next;
        if (chunk->next)
            chunk->next->prev = chunk;
    }
    return chunk;
}

    __attribute__((visibility("default")))
void free(void *ptr)
{
    if (!ptr)
        return;
    pthread_mutex_lock(&lock);
    struct chunk *chunk = get_chunk(ptr);
    if (chunk->free & ALONE_CHUNK)
    {
        munmap(chunk, chunk->size + sizeof(struct chunk));
        pthread_mutex_unlock(&lock);
        return;
    }
    chunk->free |= FREE;
    //merge and munmap
    chunk = merge_chunk(chunk);

    size_t empty_size = SIZE_PAGE - sizeof(struct chunk);
    if ((chunk->free & FIRST_CHUNK) && chunk->size == empty_size)
    {
        if (my_heap == chunk)
            my_heap = chunk->next;
        if (chunk->prev)
            chunk->prev->next = chunk->next;
        if (chunk->next)
            chunk->next->prev = chunk->prev;
        munmap(chunk, SIZE_PAGE);
    }
    if (!my_heap)
        pthread_mutex_destroy(&lock);
    pthread_mutex_unlock(&lock);
}

    __attribute__((visibility("default")))
void *realloc(void *ptr, size_t size)
{
    if (!ptr)
        return malloc(size);
    if (size == 0)
    {
        free(ptr);
        return NULL;
    }
    pthread_mutex_lock(&lock);
    size = word_align(size);
    size_t max_size = SIZE_PAGE - 1024;
    struct chunk *chunk = get_chunk(ptr);
    if (chunk->next != NULL && (chunk->next->free & FREE)
        && !(chunk->next->free & FIRST_CHUNK) && size < max_size
        && (chunk->size + chunk->next->size + sizeof(struct chunk) >= size))
    {
        chunk->size = chunk->size + chunk->next->size + sizeof(struct chunk);
        chunk->next = chunk->next->next;
        if (chunk->next)
            chunk->next->prev = chunk;
        split_chunk(chunk, size);
        pthread_mutex_unlock(&lock);
        return get_ptr(chunk);
    }
    else
    {
        pthread_mutex_unlock(&lock);
        char *new = malloc(size);
        if (!new)
            return NULL;
        char *old = ptr;
        for (size_t i = 0; i < chunk->size && i < size; i++)
        {
            *(new + i) = *(old + i);
        }
        free(ptr);
        return new;
    }
    return NULL;
}

    __attribute__((visibility("default")))
void *calloc(size_t nmemb, size_t size)
{
    char *ptr = malloc(nmemb * size);
    if (!ptr)
        return NULL;
    for (size_t i = 0; i < nmemb * size; i++)
    {
        *(ptr + i) = 0;
    }
    return ptr;
}

