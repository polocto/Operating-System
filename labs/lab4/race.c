#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define REPEAT 10

void* increment(void *number);
void* decrement(void *number);

int main()
{
    int i;

    pthread_t thread1, thread2;

    for(int j = 0; j<REPEAT; j++){
        i = 65;
        pthread_create( &thread1, NULL, increment, (void*)&i);
        pthread_create( &thread2, NULL, decrement, (void*)&i);

        pthread_join( thread1, NULL);
        pthread_join( thread2, NULL);
        printf("%d\n\n\n",i);
    }
    return 0;
}

void* increment(void *number)
{
    int reg =*((int*)number);
    printf("I1\n");
    reg++;
    printf("I2\n");
    sleep(1);
    printf("I3\n");
    *((int*)number) = reg;
    printf("I4\n");
}

void* decrement(void *number)
{
    int reg =*((int*)number);
    printf("D1\n");
    reg--;
    printf("D2\n");
    sleep(1);
    printf("D3\n");
    *((int*)number) = reg;
    printf("D4\n");
}