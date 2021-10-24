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

*The results show only the results that are different from the one expected*

In the previous code, there si nothing that order the action. So we have some random result due to critical section. We access a variable at the same time from two different program, this let us with a result that can be not the one expected. The context switch is done before the varaible is equal to the temporary value but after the temporary value of `number+1` or `number-1` has been created. The value of the variable `number` correspond to the last action where `number = temporary value`.


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
In the previous code we volounterly let a context switch happen in order to show a possible consequence if there is no mutual exclusion. We are calling the function `sleep` during the changement of value which lead to a context switch. By doing this the two function `decrement` and `increment` access to the same value of `number`. Looking to the results we can see that a context switch are more often in the critical section since we call the `sleep` function. However those context switch are not volounter context switch so some time as in the second result there is no apparent context switch in the critical section of the function `decrement`. The thing that could happen is that the final could be either decremented, incremented or equal to its initial value.

## Solving the Problem : Synchronizing access using semaphores
>1. Use semaphores to enforce mutual exlusion and solve the race problem in the first exercise

### Code:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define REPEAT 1000000

sem_t semaphore;
void* increment(void *number);
void* decrement(void *number);

int main()
{
    int i;
    pthread_t thread1, thread2;

    if(sem_init(&semaphore,0,1)){
        return EXIT_FAILURE;
    }

    for(int j = 0; j<REPEAT; j++){
        i = 65;
        pthread_create( &thread1, NULL, increment, (void*)&i);
        pthread_create( &thread2, NULL, decrement, (void*)&i);

        pthread_join( thread1, NULL);
        pthread_join( thread2, NULL);
        if(i!=65)
            printf("%d\n\n",i);
    }
    return sem_destroy(&semaphore);
}

void* increment(void *number)
{
    sem_wait(&semaphore);
    int reg =*((int*)number);
    reg++;
    *((int*)number) = reg;
    sem_post(&semaphore);
}

void* decrement(void *number)
{
    sem_wait(&semaphore);
    int reg =*((int*)number);
    reg--;
    *((int*)number) = reg;
    sem_post(&semaphore);
}
```
### Explanation
In the previous code there is no particular order but once on of the two functions `increment` or `decrement` is called, the second one will have to wait until the first one finishes by calling the function `sem_post`.

First we initialie the semaphore variable to 1. Then by calling the function `sem_wait` in one of the function, the semaphore variable `semaphore` drecrease to 0. So the other function when it will call the function `sem_wait` it will have to wait for the semaphore value to increase to 1 before being able to run. By doing this with have mutual exclusion and no more critical section.

- > What if we had more than two processes ? Is there something else to do to enforce mutual exclusion ?  Explain and experiment using three processes.

    ### Code
    ```c
    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <pthread.h>
    #include <semaphore.h>
    #include <sys/ipc.h>
    #include <sys/shm.h>
    #include <wait.h>

    #define REPEAT 1000000

    //SHARED MEMORY
    #define KEY1 4567
    #define KEY2 4568
    #define KEY3 4569
    #define PERMS 0660
    sem_t *increase, *decrease;
    void* increment(void *number);
    void* decrement(void *number);

    int main()
    {
        int *value = NULL;
        int id1, id2, id3;
        pthread_t thread;

        id1 = shmget(KEY1, sizeof(int), IPC_CREAT | PERMS);
        id2 = shmget(KEY2, sizeof(int), IPC_CREAT | PERMS);
        id3 = shmget(KEY3, sizeof(int), IPC_CREAT | PERMS);

        increase = (sem_t *)shmat(id1, NULL, 0);
        value = (int *)shmat(id2, NULL, 0);
        decrease = (sem_t *)shmat(id3, NULL, 0);
        if(sem_init(increase,1,0)){
            return EXIT_FAILURE;
        }
        if(sem_init(decrease,1,1)){
            return EXIT_FAILURE;
        }

            *value = 65;
            if(fork()==0){
                if(fork()==0){
                    pthread_create( &thread, NULL, increment, (void*)value);
                    pthread_join( thread, NULL);
                    exit(0);
                } else {
                    pthread_create( &thread, NULL, decrement, (void*)value);
                    pthread_join( thread, NULL);
                    exit(0);
                }
            } else {
                pthread_create( &thread, NULL, decrement, (void*)value);
                pthread_join( thread, NULL);
            }
            
        wait(NULL);
        printf("%d\n",*value);
        //Supression thread & shared memory
        sem_destroy(increase);
        sem_destroy(decrease);
        shmdt(value);
        shmctl(id1, IPC_RMID, NULL);
        shmctl(id2, IPC_RMID, NULL);
        shmctl(id3, IPC_RMID, NULL);

        return 0;
    }

    void* increment(void *number)
    {
        sem_wait(increase);
        printf("I: %d\n", *((int*)number));
        int reg =*((int*)number);
        reg++;
        *((int*)number) = reg;
        printf("I: %d\n", *((int*)number));
        sem_post(decrease);
    }

    void* decrement(void *number)
    {
        sem_wait(decrease);
        printf("D: %d\n", *((int*)number));
        int reg =*((int*)number);
        reg--;
        *((int*)number) = reg;
        printf("D: %d\n", *((int*)number));
        sem_post(increase);
    }
    ```
    ### Explanation
    Here we have shared semaphore it is not necessary to do something in order to enforce mutual exclusion. If you want to order the function you should use other semaphore.

>A deadlock is a situation in which a process is waiting for some resource held by another process waiting for it to release another resource, there by forming a loop of blocked processes ! Use semaphores to force a deadlock situation using three processes.

### Code:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define REPEAT 1000000

sem_t increase, decrease, multiply;
void* increment(void *number);
void* decrement(void *number);
void* square(void *number);

int main()
{
    int i;
    pthread_t thread1, thread2, thread3;

    if(sem_init(&increase,0,0)){
        return EXIT_FAILURE;
    }
    if(sem_init(&decrease,0,0)){
        return EXIT_FAILURE;
    }

    if(sem_init(&multiply,0,0)){
        return EXIT_FAILURE;
    }

    i = 65;
    pthread_create( &thread1, NULL, increment, (void*)&i);
    pthread_create( &thread3, NULL, square, (void*)&i);
    pthread_create( &thread2, NULL, decrement, (void*)&i);

    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);
    pthread_join( thread3, NULL);

    printf("%d\n\n",i);

    sem_destroy(&increase);
    sem_destroy(&decrease);
    sem_destroy(&multiply);

    return 0;
}

void* firefox(void *number)
{
    printf("Start I\n");
    sem_wait(&increase);
    int reg =*((int*)number);
    reg++;
    *((int*)number) = reg;
    printf("In I\n");
    sem_post(&decrease);
    printf("End I\n");
}

void* decrement(void *arg)
{
    printf("Start D\n");
    sem_wait(&decrease);
    printf("In D\n");
    int reg =*((int*)number);
    reg--;
    *((int*)number) = reg;
    sem_post(&multiply);
    printf("End D\n");
}

void* square(void *number)
{
    printf("Start S\n");
    sem_wait(&multiply);
    int reg =*((int*)number);
    printf("In S\n");
    reg*=reg;
    *((int*)number) = reg;
    sem_post(&increase);
    printf("End S\n");
}
```

### Explanation
Here all semaphore are initialised to 0, all functions are waiting for the semaphore to be increased to 1. But because only other functions can increase the semaphore of another function they are blocked because it's creating a deadlock. Everyone is waiting for each other.

>Use semaphores to run 3 different applications (firefox, emacs, vi) in a predefined sequence no matter in which order they are launched.

### Code:
```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define REPEAT 1000000

sem_t firefox, emacs, vi;
void* appFirefox(void *arg);
void* appEmacs(void *arg);
void* appVi(void *arg);

int main()
{
    pthread_t thread1, thread2, thread3;

    if(sem_init(&firefox,0,0)){
        return EXIT_FAILURE;
    }
    if(sem_init(&emacs,0,1)){
        return EXIT_FAILURE;
    }

    if(sem_init(&vi,0,0)){
        return EXIT_FAILURE;
    }

    pthread_create( &thread1, NULL, appFirefox, NULL);
    pthread_create( &thread3, NULL, appVi, NULL);
    pthread_create( &thread2, NULL, appEmacs, NULL);

    pthread_join( thread1, NULL);
    pthread_join( thread2, NULL);
    pthread_join( thread3, NULL);

    sem_destroy(&firefox);
    sem_destroy(&emacs);
    sem_destroy(&vi);

    return 0;
}

void* appFirefox(void *arg)
{
    sem_wait(&firefox);
    if(fork()==0)
    {
        execl("/bin/firefox", "firefox", NULL);
        exit(0);
    }
    sem_post(&emacs);
}

void* appEmacs(void *arg)
{
    sem_wait(&emacs);
    if(fork==0){
        execl("/bin/emacs", "emacs", NULL);
        exit(0);
    }
    sem_post(&vi);
}

void* appVi(void *arg)
{
    sem_wait(&vi);
    if(fork()==0){
        execl("/bin/vi", "vi", NULL);
        exit(0);
    }
    sem_post(&firefox);
}
```

### Explanation
We are using different semaphore to be able to create an order. We are using three different semaphore variable : 
- emacs
- vi
- firefox

At the begining of each threads we call `sem_wait('corresponding semaphore variable')`. emacs is the first one to be call because `emacs` sempahore variable is initializes to one and all other are initializ to 0. After is vi because at the end of emacs, we call `sem_post(vi)` and finally firefox because at the end of appVi we call `sem_post(firefox)`.

>Use sempahores to implement the following parallelized calculation (a+b)\*(c-d)\*(e+f)
>- T1 runs (a+b) and stores the result in a shared table (1st available spot) 
>- T2 runs (c+d) and stores the result in a shared table (1st available spot) 
>- T3 runs (e+f) and stores the result in a shared table (1st available spot) 
>- T4 waits for two tasks to end and does the corresponding calculation 
>- T4 waits for the remaining task to end and does the final calculation then displays the result

### Code:

```c
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

```
### Explanation
All sum threads are running simultaneously. For the first multiplication we are waiting for two sum to finish in order to be able to make the correct equation. And after this we are waiting for the last sum and the multiplication to finish so we can calculate the last equation in order to obtain our final result.

## Authors

- Paul SADE
- Mathis CAMARD