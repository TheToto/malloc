#define _GNU_SOURCE

#include <stddef.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>

#define SIZE_PAGE sysconf(_SC_PAGESIZE)

struct chunck *my_heap = NULL;

struct chunk {
    size_t size;
    int free;
    struct chunk *next; // prev pour merge ?
};

size_t word_align(size_t n)
{
    return (sizeof(size_t) + n - 1) & -(sizeof(size_t)); 
}

struct chunk *allocate_page(struct chunk *prev_page)
{
    struct chunk *new_page = mmap(0, SIZE_PAGE,
            PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (my_heap != MAP_FAILED)
    {
        new_page->next = NULL;
        new_page->free = 1;
        new_page->size = SIZE_PAGE - sizeof(struct chunk);
        if (prev_page)
            prev_page->next = new_page;
        return new_page;
    }
    return NULL;
}

static struct chunk* get_chunk(void *ptr){
    char *tmp = ptr;
    void *res = tmp - sizeof(struct chunk);
    return res;
}

static char *get_ptr(struct chunk *chunk){
    void *tmp = chunk;
    char *res = tmp;
    return res + sizeof(struct chunk);
}


struct chunk *ask_chunk(size_t size)
{
    struct chunk *i = my_heap;
    if (!i)
        return NULL;
    while (i && !i->free && i->size < size)
        i = i->next;
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
        new->free = 1;
        chunk->size = size;
        chunk->next = new;
    }
}


    __attribute__((visibility("default")))
void *malloc(size_t size)
{
    size = word_align(size);
    struct chunk *ask = ask_chunk(size);
    if (ask && ask->free && ask->size >= size)
    {
        split_chunk(ask, size);
    }
    else
    {
        ask = allocate_page(ask);
        if (ask == NULL)
            return NULL;
        split_chunk(ask, size);
    }
    ask->free = 0;
    return ask;
}

    __attribute__((visibility("default")))
void free(void *ptr)
{
    struct chunk *chunk = get_chunk(ptr);
    chunk->free = 1;
    //merge
}

    __attribute__((visibility("default")))
void *realloc(void *ptr, size_t size)
{
    return NULL;
}

    __attribute__((visibility("default")))
void *calloc(size_t nmemb, size_t size)
{
    return NULL;
}
