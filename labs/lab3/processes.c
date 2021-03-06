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
#define REPEAT 1000

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