#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define REPEAT 1000000

void* somme(void *arg);
void* multiplication(void *arg);

typedef struct {
    int a;
    int b;
    int result;
}Equation;

void init(Equation* arg){
    arg->a = 5;
    arg->b = 6;
}

int main()
{
    pthread_t thread1, thread2, thread3, thread4;
    Equation calc1, calc2, calc3, result;

    init(&calc1);
    init(&calc2);
    init(&calc3);

    pthread_create( &thread1, NULL, somme, (void*)&calc1);
    pthread_create( &thread2, NULL, somme, (void*)&calc2);
    pthread_create( &thread3, NULL, somme, (void*)&calc3);
    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);
    result.a = calc1.result;
    result.b = calc2.result;
    
    pthread_create( &thread4, NULL, multiplication, &result);
    pthread_join( thread3, NULL);
    pthread_join( thread4, NULL);
    result.a = calc3.result;
    result.b = result.result;
    
    pthread_create( &thread4, NULL, multiplication, &result);
    pthread_join( thread4, NULL);

    printf("%d\n", result.result);

    return 0;
}

void* somme(void *arg)
{
    Equation* reg = (Equation*)arg;
    reg->result = reg->a + reg->b;
}

void* multiplication(void *arg)
{
    Equation* reg = (Equation*)arg;
    reg->result = reg->a * reg->b;
}
