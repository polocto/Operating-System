# Rapport

>How you implemented the parallelized calculation ?

We implemented the two codes following the **same schema**. For **each** **thread** created, using `pthread_create()`  in [threads.c](#threads.c), we **create a new process** in [processes.c](#processes.c), using `fork()`. All the necessary variables to the intial code (which have been made for the equation) have been **created inside the loop** `for()` **in both files** *processes.c* and  *threads.c*. We use the **same values** for the different variables.

>List the differences between the process and the thread versions and which one is more adapted in this case ?

| Process                                                                               | Thread
|:-                                                                                     |:-
|Process means any program is in execution.	                                            |Thread means segment of a process.
|Process takes more time to terminate.	                                                |Thread takes less time to terminate.
|It takes more time for creation.                                                       |It takes less time for creation.
|It also takes more time for context switching.	                                        |It takes less time for context switching.
|Process is less efficient in term of communication.                                    |Thread is more efficient in term of communication.
|Process consume more resources.	                                                    |Thread consume less resources.
|Process is isolated.|                                                                  Threads share memory.
|Process is called heavy weight process.                                                |Thread is called light weight process.
|Process switching uses interface in operating system.	                                |Thread switching does not require to call a operating system and cause an interrupt to the kernel.
|If one process is blocked then it will not effect the execution of other process 	    |Second thread in the same task couldnot run, while one server thread is blocked.
|Process has its own Process Control Block, Stack and Address Space.	                |Thread has Parents’ PCB, its own Thread Control Block and Stack and common Address space.

>Explain how performance measurement has been conducted and state which version of the solution is better based on your results.

Both codes have been tested using the **same librairies**, **same machine** and **same environement**. In order to be able to measure the ***time*** we **repeat** the calculation n times (here n=1000),

### Threads
```sh
Result : 39
210000.000000 usec
221346 usec
Volonter context switch : 2716-0=2716
Involonter context switch : 1853-0=1853
In : 0
Out : 0
```

### Processes
```sh
Result : 39
1880000.000000 usec
1888355 usec
Volonter context switch : 1988-0=1988
Involonter context switch : 2547-0=2547
In : 0
Out : 0
```
Based on our results the **threads are more adapted** to this usage. Processes are here about **10 times** longer than threads.


>Give the result concerning I/O and context switches. Is there a difference between the two versions and why ?

We obtain **no I/O** with both programs because we have **no** **input** or **output** using the **filesystem**. 
For **context switch** their is **2 categories**, **involontary** and **volontary**.
In processes if we use only the `while()` and no `wait()` we will obtain **no volontary** **context switch**. In our ***processes.c*** we call the function `wait()`. Finally we have about the same number of volontary or involontary context switch using either ***processes.C*** or ***threads.c***.

## Code
### threads.c

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <unistd.h>

#define REPEAT 100

typedef struct 
{
    /* data */
    int a;
    int b;
    int result;
}Somme;

clock_t times(struct tms *buf); 

void verrif(int iret);
void *soustraction(void *ptr);
void *division(void *ptr);
void *somme( void *ptr );
void *multiplication(void *ptr);

int main()
{
     struct tms start, end;
    struct rusage rstart, rend;
    int i, resultat;
    times(&start);
    getrusage(RUSAGE_SELF, &rstart);
    for(i = 0; i< REPEAT; i++){
        pthread_t thread1, thread2, thread3, thread4, thread5, thread6;
        Somme a,b,c,d;
        a.a = 2;    a.b = 3; // Somme => 5
        b.a = 6;    b.b = 2; // Multiplication => 12
        c.a = 1;    c.b = 10; // Soustraction => -9
        d.a = 21;   d.b = 12; // Somme => 33
        int  iret1, iret2, iret3, iret4, iret5, iret6;
        /* Create independent threads each of which will execute function */

        iret1 = pthread_create( &thread1, NULL, somme, (void*) &a);
        verrif(iret1);

        iret2 = pthread_create( &thread2, NULL, multiplication, (void*) &b);
        verrif(iret2);

        iret3 = pthread_create( &thread3, NULL, soustraction, (void*) &c);
        verrif(iret3);

        iret4 = pthread_create( &thread4, NULL, somme, (void*) &d);
        verrif(iret4);

        /* Wait till threads are complete before main continues. Unless we  */
        /* wait we run the risk of executing an exit which will terminate   */
        /* the process and all threads before the threads have completed.   */
        pthread_join( thread1, NULL);//Wait
        pthread_join( thread4, NULL);
        a.a = a.result;
        a.b = d.result;
        iret5 = pthread_create( &thread5, NULL, somme, (void*) &a);
        verrif(iret5);
        
        pthread_join( thread2, NULL); //wait end of multiplication
        pthread_join( thread3, NULL);//wait end of substraction
        b.a = b.result;
        b.b = c.result;
        iret6 = pthread_create( &thread6, NULL, division, (void*) &b);
        verrif(iret6);
        pthread_join( thread5, NULL);
        pthread_join( thread6, NULL);

        resultat = a.result -b.result;
    }
    times(&end);
    getrusage(RUSAGE_SELF, &rend);
    printf("Result : %d\n",resultat);
    printf("%lf usec\n", (end.tms_utime+end.tms_stime-start.tms_utime-start.tms_stime)*1000000.0/sysconf(_SC_CLK_TCK));
    
    printf("%ld usec\n", (rend.ru_utime.tv_sec-rstart.ru_utime.tv_sec)*1000000 +(rend.ru_utime.tv_usec-rstart.ru_utime.tv_usec)+(rend.ru_stime.tv_sec-rstart.ru_stime.tv_sec)*1000000 +(rend.ru_stime.tv_usec-rstart.ru_stime.tv_usec));
    printf("Volonter context switch : %ld-%ld=%ld\n", rend.ru_nvcsw, rstart.ru_nvcsw, rend.ru_nvcsw - rstart.ru_nvcsw);
    printf("Involonter context switch : %ld-%ld=%ld\n", rend.ru_nivcsw, rstart.ru_nivcsw, rend.ru_nivcsw - rstart.ru_nivcsw);
    printf("In : %ld\n",rend.ru_inblock - rstart.ru_inblock);
    printf("Out : %ld\n",rend.ru_oublock - rstart.ru_oublock);
     return 0;
}

void verrif(int iret)
{
    if(iret)
        {
            fprintf(stderr,"Error - pthread_create() return code: %d\n",iret);
            exit(EXIT_FAILURE);
        }
}

void *somme( void *ptr )
{
     Somme *calcul;
     calcul = (Somme *) ptr;
     calcul->result = calcul->a + calcul->b;
}

void *soustraction(void *ptr)
{
    Somme *calcul;
     calcul = (Somme *) ptr;
     calcul->result = calcul->a - calcul->b;
}

void *multiplication(void *ptr)
{
    Somme *calcul;
     calcul = (Somme *) ptr;
     calcul->result = calcul->a * calcul->b;
}

void *division(void *ptr)
{
    Somme *calcul;
     calcul = (Somme *) ptr;
     calcul->result = calcul->a / calcul->b;
}
```

### processes.c

```c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>

clock_t times(struct tms *buf);

#define KEY1 4567
#define KEY2 4568
#define KEY3 4569
#define KEY4 4570
#define KEY5 4571
#define KEY6 4572
#define PERMS 0660
#define REPEAT 100

typedef struct 
{
    /* data */
    int a;
    int b;
}Somme;

int soustraction(Somme calcul);
int somme(Somme calcul);
int division(Somme calcul);
int multiplication(Somme calcul);

int main(int argc, char **argv)
{
    struct tms start, end;
    struct rusage rstart, rend;
    int i;
    int final;

    times(&start);
    getrusage(RUSAGE_SELF, &rstart);
    for (i = 0; i < REPEAT; i++)
    {

        int id1, id2, id3, idFlag1, idFlag2, idFlag3;
        int i = 0;
        int *ptr1 = NULL, *ptr2 = NULL, *ptr3 = NULL, *flag1 = NULL, *flag2 = NULL, *flag3 = NULL;

        // system("ipcs -m");                                  //display shared memory
        id1 = shmget(KEY1, sizeof(int), IPC_CREAT | PERMS); //create share memory space
        // system("ipcs -m");                                  //display shared memory
        id2 = shmget(KEY2, sizeof(int), IPC_CREAT | PERMS); //create share memory space
        // system("ipcs -m");                                  //display shared memory
        id3 = shmget(KEY3, sizeof(int), IPC_CREAT | PERMS);

        idFlag1 = shmget(KEY4, sizeof(int), IPC_CREAT | PERMS);
        idFlag2 = shmget(KEY5, sizeof(int), IPC_CREAT | PERMS);
        idFlag3 = shmget(KEY6, sizeof(int), IPC_CREAT | PERMS);

        //! Flag creation

        flag1 = (int *)shmat(idFlag1, NULL, 0); //register flag in the shared memory
        flag2 = (int *)shmat(idFlag2, NULL, 0); //register flag in the shared memory
        flag3 = (int *)shmat(idFlag3, NULL, 0);

        ptr1 = (int *)shmat(id1, NULL, 0); //register ptr in the shared memory
        ptr2 = (int *)shmat(id2, NULL, 0); //register ptr in the shared memory
        ptr3 = (int *)shmat(id3, NULL, 0);

        *flag1 = 0;
        *flag2 = 0;
        *flag3 = 0;
        Somme a,b,c,d;
        a.a = 2;    a.b = 3; // Somme => 5
        b.a = 6;    b.b = 2; // Multiplication => 12
        c.a = 1;    c.b = 10; // Soustraction => -9
        d.a = 21;   d.b = 12; // Somme => 33
        if (fork() == 0)
        {
            if (fork() == 0)
            {
                if (fork() == 0)
                {
                    *ptr1 = somme(a);
                    *flag1 = 1;
                    exit(0);
                }
                else
                {
                    *ptr2 = multiplication(b);
                    *flag2 = 1;
                    exit(0);
                }
            }
            else
            {
                *ptr3 = soustraction(c);
                *flag3 = 1;
                exit(0);
            }
        }
        else
        {
            int result = somme(d);

            while (*flag1 == 0 || *flag2 == 0 || *flag3 == 0)
                wait(NULL);

            if(fork()==0){
                *ptr1 = result + (*ptr1); // => 38
                exit(0);
            }
            else
            {

                int Total = (*ptr2) / (*ptr3);
                wait(NULL);
                final = (*ptr1) - Total;
                shmdt(ptr1);
                shmdt(ptr2);
                shmdt(ptr3);
                shmdt(flag1);
                shmdt(flag2);
                shmdt(flag3);
                
                shmctl(id2, IPC_RMID, NULL); // mark ptr to be destroy from the share memory
                shmctl(id1, IPC_RMID, NULL); // mark ptr to be destroy from the share memory
                shmctl(id3, IPC_RMID, NULL);
                shmctl(idFlag2, IPC_RMID, NULL); // mark ptr to be destroy from the share memory
                shmctl(idFlag1, IPC_RMID, NULL); // mark ptr to be destroy from the share memory
                shmctl(idFlag3, IPC_RMID, NULL);
            }
        }
    }
    times(&end);
    getrusage(RUSAGE_SELF, &rend);
    printf("Result : %d\n", final);
    printf("%lf usec\n", (end.tms_utime + end.tms_stime - start.tms_utime - start.tms_stime) * 1000000.0 / sysconf(_SC_CLK_TCK));

    printf("%ld usec\n", (rend.ru_utime.tv_sec - rstart.ru_utime.tv_sec) * 1000000 + (rend.ru_utime.tv_usec - rstart.ru_utime.tv_usec) + (rend.ru_stime.tv_sec - rstart.ru_stime.tv_sec) * 1000000 + (rend.ru_stime.tv_usec - rstart.ru_stime.tv_usec));
    printf("Volonter context switch : %ld-%ld=%ld\n", rend.ru_nvcsw, rstart.ru_nvcsw, rend.ru_nvcsw - rstart.ru_nvcsw);
    printf("Involonter context switch : %ld-%ld=%ld\n", rend.ru_nivcsw, rstart.ru_nivcsw, rend.ru_nivcsw - rstart.ru_nivcsw);
    printf("In : %ld\n",rend.ru_inblock - rstart.ru_inblock);
    printf("Out : %ld\n",rend.ru_oublock - rstart.ru_oublock);
}


int somme(Somme calcul)
{
    return calcul.a + calcul.b;
}

int soustraction(Somme calcul)
{
    return calcul.a - calcul.b;
}

int multiplication(Somme calcul)
{
    return calcul.a * calcul.b;
}

int division(Somme calcul)
{
    return calcul.a / calcul.b;
}
```