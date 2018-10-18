#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdint.h>
#include "malloc.h"

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
