#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/times.h>
#include <sys/resource.h>
#include <unistd.h>

#define REPEAT 1000

typedef struct 
{
    /* data */
    int a;
    int b;
    int result;
}Somme;

clock_t times(struct tms *buf); 

void *somme( void *ptr );

int main()
{
     struct tms start, end;
    struct rusage rstart, rend;
    int i, resultat;



    times(&start);
    getrusage(RUSAGE_SELF, &rstart);
    /////////////////////////////////
    for(i = 0; i< REPEAT; i++){
        pthread_t thread1, thread2, thread3;
        Somme a,b,c;
        a.a = 2;
        b.a = 6;
        c.a = 1;
        a.b = 3;
        b.b = 2;
        c.b = 10;
        int  iret1, iret2, iret3;
        /* Create independent threads each of which will execute function */

        iret1 = pthread_create( &thread1, NULL, somme, (void*) &a);
        if(iret1)
        {
            fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
            exit(EXIT_FAILURE);
        }

        iret2 = pthread_create( &thread2, NULL, somme, (void*) &b);
        if(iret2)
        {
            fprintf(stderr,"Error - pthread_create() return code: %d\n",iret2);
            exit(EXIT_FAILURE);
        }

        iret3 = pthread_create( &thread3, NULL, somme, (void*) &c);
        if(iret3)
        {
            fprintf(stderr,"Error - pthread_create() return code: %d\n",iret2);
            exit(EXIT_FAILURE);
        }

        /* Wait till threads are complete before main continues. Unless we  */
        /* wait we run the risk of executing an exit which will terminate   */
        /* the process and all threads before the threads have completed.   */

        pthread_join( thread1, NULL);
        pthread_join( thread2, NULL);
        pthread_join( thread3, NULL);
        resultat = a.result * b.result * c.result;
    }
    ////////////////////////////////////
    times(&end);
    getrusage(RUSAGE_SELF, &rend);

    printf("%lf usec\n", (end.tms_utime+end.tms_stime-start.tms_utime-start.tms_stime)*1000000.0/sysconf(_SC_CLK_TCK));
    
    printf("%ld usec\n", (rend.ru_utime.tv_sec-rstart.ru_utime.tv_sec)*1000000 +(rend.ru_utime.tv_usec-rstart.ru_utime.tv_usec)+(rend.ru_stime.tv_sec-rstart.ru_stime.tv_sec)*1000000 +(rend.ru_stime.tv_usec-rstart.ru_stime.tv_usec));
    printf("%d\n", resultat);

     return 0;
}

void *somme( void *ptr )
{
     Somme *calcul;
     calcul = (Somme *) ptr;
     calcul->result = calcul->a + calcul->b;
}
