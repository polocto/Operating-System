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