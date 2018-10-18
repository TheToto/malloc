#include <stdlib.h>
#include <stdio.h>

void print_all(void);
int main(void)
{
    setbuf(stdout, NULL);
    int **test = malloc(sizeof(int*) * 6);
    int *test2 = malloc(140);
    printf("alloc %p \n", test);
    char *txt = malloc(30);
    printf("alloc %p \n", txt);
    for (int i = 0; i < 6; i++)
    {
        test[i] = malloc(1040);
    }
    printf("before realloc\n");

    print_all();
    for (int i = 0; i < 6; i++)
    {
        test[i] = realloc(test[i],2040);
    }
    printf("before realloc2\n");
    print_all();
    test[0] = realloc(test[0], 5123);
    test[1] = realloc(test[1], 2048);
    test[2] = realloc(test[2], 2048);
    test[3] = realloc(test[3], 3000);
    test[4] = realloc(test[4], 3000);
    test[5] = realloc(test[5], 2048);
    //print_all();
    txt[0] = 'H';
    txt[1] = 'B';
    txt[2] = '\0';
    printf(" %s\n", txt);
    printf("free first\n");
    for (int i = 0; i < 6; i++)
    {
        printf("\n\n");


    print_all();
        free(test[i]);
    }
    free(test);
    free(test2);
    free(txt);
    print_all();

}
