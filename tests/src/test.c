#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    setbuf(stdout, NULL);
    printf("malloc 6 *int");
    int **test = malloc(sizeof(int*) * 6);
    for (int i = 0; i < 6; i++)
    {
        printf("malloc 1040 %p\n", test[i]);
        test[i] = malloc(1040);
    }
    printf("\n");
    for (int i = 0; i < 6; i++)
    {
        printf("realloc %p 2040\n", test[i]);
        test[i] = realloc(test[i], 2040);
    }
    for (int i = 0; i < 6; i++)
    {
        printf("free %p\n", test[i]);
        free(test[i]);
    }
    free(test);
    printf("Everything is free\n");
}
