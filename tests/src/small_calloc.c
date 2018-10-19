#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    setbuf(stdout, NULL);
    int **test = calloc(sizeof(int*), 6);
    for (int i = 0; i < 6; i++)
    {
        printf("calloc 1040\n");
        test[i] = calloc(1, 1040);
        if (!test[i])
        {
            printf("CALLOC ERROR");
            return 1;
        }
    }
    for (int i = 0; i < 6; i++)
    {
        printf("free %p\n", test[i]);
        free(test[i]);
    }
    free(test);
    printf("\nResume : 6 calloc of 1024 + free all\n");
    return 0;
}
