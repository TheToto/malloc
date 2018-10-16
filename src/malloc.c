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
    return tmp - sizeof(struct chunk);
}

static void *get_ptr(struct chunk *chunk){
    char *tmp = chunk;
    return tmp + sizeof(struct chunk);
}


struct chunk *ask_chunk(size_t size)
{
    struct chunk *i = my_heap;
    if (!my_heap)
        return NULL;
    while (i && !i->free && i->size < size)
        i = i->next;
    return i;
}

static void split_chunk(struct chunk *chunk, size_t size)
{
    if (chunk->size >= size + sizeof(struct chunk) + sizeof(size_t))
    {
        
    }
    struct chunk *new;
    new = (struct chunk*)(b->data + size);
    new->size = b->size - s - sizeof(struct chunk) ;
    new->next = b->next;
    new->free = 1;
    chunk->size = size;
    chunk->next = new;
}


    __attribute__((visibility("default")))
void *malloc(size_t size)
{
    struct chunk *ask = ask_chunk(size);
    if (ask->free && ask->size >= size)
    {
        //split
    }
    else
    {
        ask = allocate_page(ask);
        //split
    }
    return NULL;
}

    __attribute__((visibility("default")))
void free(void *ptr)
{
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
