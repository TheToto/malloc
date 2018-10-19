#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    setbuf(stdout, NULL);
    int **test = malloc(sizeof(int*) * 600);
    for (int i = 0; i < 600; i++)
    {
        printf("malloc 600\n");
        test[i] = malloc(1040);
        if (!test[i])
        {
            printf("MALLOC ERROR");
            return 1;
        }
    }
    for (int i = 0; i < 600; i++)
    {
        printf("free %p\n", test[i]);
        free(test[i]);
    }
    free(test);
    printf("\nResume : 600 malloc of 1024 + free all\n");
    return 0;
}
