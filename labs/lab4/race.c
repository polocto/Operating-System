#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <pthread.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <stdio.h>
#include <unistd.h>

#define KEY 4567
#define PERMS 0660

void increment(int *number);
void decrement(int *number);

// int main()
// {
//     int i;

//     *i = 65;
//     iret1 = pthread_create( &thread1, NULL, increment, i);
//     iret2 = pthread_create( &thread2, NULL, increment, i);

        // pthread_join( thread1, NULL);
        // pthread_join( thread1, NULL);

//     return 0;
// }

int main()
{
    int id, *i = NULL;
    id = shmget(KEY, sizeof(int), IPC_CREAT | PERMS); //create share memory space
    i = (int *)shmat(id, NULL, 0);

    *i = 65;
    if(fork()==0)
    {
        increment(i);
    }
    else
    {
        decrement(i);
    }


    return 0;
}


void increment(int *number)
{
    (*number)++;
}

void decrement(int *number)
{
    (*number)--;
}