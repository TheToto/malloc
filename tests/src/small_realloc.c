#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    setbuf(stdout, NULL);
    int **test = malloc(sizeof(int*) * 6);
    for (int i = 0; i < 6; i++)
    {
        printf("malloc 1040\n");
        test[i] = malloc(1040);
        if (!test[i])
        {
            printf("MALLOC ERROR\n");
            return 1;
        }
    }
    for (int i = 0; i < 6; i++)
    {
        printf("realloc %p 2040\n", test[i]);
        test[i] = realloc(test[i], 2040);
        if (!test[i])
        {
            printf("REALLOC ERROR\n");
            return 1;
        }
    }
    for (int i = 0; i < 6; i++)
    {
        printf("free %p\n", test[i]);
        free(test[i]);
    }
    free(test);
    printf("\nResume : 6 malloc of 1040 + 6 realloc of 2040 + free all\n");
    return 0;
}
