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

int main(int argc, char **argv)
{
    struct tms start, end;
    struct rusage rstart, rend;
    int i;

    times(&start);
    getrusage(RUSAGE_SELF, &rstart);
    for (i = 0; i < 10000; i++)
    {
        /* code */

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

        if (fork() == 0)
        {
            if (fork() == 0)
            {
                if (fork() == 0)
                {
                    int a = 25, b = 2;
                    *ptr1 = a + b;
                    *flag1 = 1;
                }
                else
                {

                    int c = 3, d = 7;
                    *ptr2 = c * d;
                    *flag2 = 1;
                }
            }
            else
            {
                int e = 5;
                int f = 10;
                *ptr3 = e - f;
                *flag3 = 1;
            }
        }
        else
        {
            int g = 2;
            int h = 1;
            int result = g + h;

            while (*flag1 == 0 || *flag2 == 0 || *flag3 == 0)
                ;

            // printf("a : %d , b : %d\n",a,b);
            // printf("*ptr1 : %d\n", *ptr1);
            int Total = (*ptr1) - (*ptr2) / (*ptr3) + result;
            printf("%d - %d / %d + %d = %d\n", *ptr1, *ptr2, *ptr3, result, Total); //display results
            shmctl(id2, IPC_RMID, NULL);                                            // mark ptr to be destroy from the share memory
            shmctl(id1, IPC_RMID, NULL);                                            // mark ptr to be destroy from the share memory
            shmctl(id3, IPC_RMID, NULL);
            shmctl(idFlag2, IPC_RMID, NULL); // mark ptr to be destroy from the share memory
            shmctl(idFlag1, IPC_RMID, NULL); // mark ptr to be destroy from the share memory
            shmctl(idFlag3, IPC_RMID, NULL);
        }
    }
    times(&end);
    getrusage(RUSAGE_SELF, &rend);

    printf("%lf usec\n", (end.tms_utime + end.tms_stime - start.tms_utime - start.tms_stime) * 1000000.0 / sysconf(_SC_CLK_TCK));

    printf("%ld usec\n", (rend.ru_utime.tv_sec - rstart.ru_utime.tv_sec) * 1000000 + (rend.ru_utime.tv_usec - rstart.ru_utime.tv_usec) + (rend.ru_stime.tv_sec - rstart.ru_stime.tv_sec) * 1000000 + (rend.ru_stime.tv_usec - rstart.ru_stime.tv_usec));
}
