#ifndef MALLOC_H
#define MALLOC_H

#define SIZE_PAGE sysconf(_SC_PAGESIZE)
#include <stddef.h>

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
    struct chunk *next_free;
};

struct meta
{
    void *my_heap;
    void *first_free;
    pthread_mutex_t lock;
};

extern struct meta meta;

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);

size_t word_align(size_t n);
struct chunk* get_chunk(void *ptr);
char *get_ptr(struct chunk *chunk);
void split_chunk(struct chunk *chunk, size_t size);
void remove_free_list(struct chunk *chunk);

#endif /* ! MALLOC_H */
