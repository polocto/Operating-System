# Rapport

## Concurrent Access To Shared Memory :  Race Problems

>1. Using two tasks, create a shared variable 'i' and initialize it 65; one task should increment the variable and the other one should decrement it.

### Code:
```c
#include <stdio.h>
#include <pthread.h>

#define REPEAT 1000000

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
        if(i!=65)
        printf("%d\n",i);
    }
    return 0;
}

void* increment(void *number)
{
    (*(int*)number)++;
}

void* decrement(void *number)
{
    (*(int*)number)--;
}
```

### Results :

#### Test 1
```sh
polocto@polocto-desktop:/media/polocto/Commun/Documents/ING4/Operating-System/labs/lab4$ ./race.o
64
66
64
64
64
64
64
66
64
```
#### Test 2
```sh
polocto@polocto-desktop:/media/polocto/Commun/Documents/ING4/Operating-System/labs/lab4$ ./race.o
66
64
64
66
```

### Explanation

...


>2. Explain why the following code could lead to an error

```c
eg = i;
sleep(for_some_time); 
// your choice 
Reg++; //(or Reg--); 
// depending on the task 
i = Reg;
```

### Code:
```c
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
```
### Results:
```sh
I1
I2
D1
D2
I3
I4
D3
D4
64
```
or
```sh
I1
I2
D1
D2
D3
D4
I3
I4
66
```

### Explaination
...

## Solving the Problem : Synchronizing access using semaphores
>1. Use semaphores to enforce mutual exlusion and solve the race problem in the first exercise