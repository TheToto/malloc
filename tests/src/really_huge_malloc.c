#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    setbuf(stdout, NULL);
    int **test = malloc(sizeof(int*) * 60000);
    for (int i = 0; i < 60000; i++)
    {
        printf("malloc 100000\n");
        test[i] = malloc(100000);
        if (!test[i])
        {
            printf("MALLOC ERROR");
            return 1;
        }
    }
    for (int i = 0; i < 60000; i++)
    {
        printf("free %p\n", test[i]);
        free(test[i]);
    }
    free(test);
    printf("\nResume : 60000 malloc of 100000 + free all\n");
    return 0;
}
