#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    setbuf(stdout, NULL);
    int **test = malloc(sizeof(int*) * 6000);
    for (int i = 0; i < 6000; i++)
    {
        printf("malloc 100000\n");
        test[i] = malloc(100000);
        if (!test[i])
        {
            printf("MALLOC ERROR\n");
            return 1;
        }
    }
    for (int i = 0; i < 6000; i++)
    {
        printf("realloc %p 200000\n", test[i]);
        test[i] = realloc(test[i], 200000);
        if (!test[i])
        {
            printf("REALLOC ERROR\n");
            return 1;
        }
    }
    for (int i = 0; i < 6000; i++)
    {
        printf("free %p\n", test[i]);
        free(test[i]);
    }
    free(test);
    printf("\nResume : 6000 malloc of 10000 + 6000 realloc of 200000 + free all\n");
    return 0;
}
