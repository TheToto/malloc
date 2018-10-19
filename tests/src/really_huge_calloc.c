#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    setbuf(stdout, NULL);
    int **test = calloc(sizeof(int*), 60000);
    for (int i = 0; i < 60000; i++)
    {
        printf("calloc 100000\n");
        test[i] = calloc(1, 100000);
        if (!test[i])
        {
            printf("CALLOC ERROR");
            return 1;
        }
    }
    for (int i = 0; i < 60000; i++)
    {
        printf("free %p\n", test[i]);
        free(test[i]);
    }
    free(test);
    printf("\nResume : 60000 calloc of 100000 + free all\n");
    return 0;
}
