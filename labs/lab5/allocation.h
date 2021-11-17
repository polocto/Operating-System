#ifndef __MMU__H__

#define __MMU__H__

#define SIZE 65536
#define PAGE_SIZE 128
#define NUMBER_FRAME SIZE/PAGE_SIZE
#define NUMBER_PAGE NUMBER_FRAME

typedef short byte_t;
typedef int address_t;
typedef struct hole 
{
    address_t adr;
    int sz;
    struct hole *next;
    struct hole *prev;
} hole_t;

typedef struct {
    //byte_t mem[SIZE]; // espace pour donnée
    hole_t* root; // holes list
    int page_table[NUMBER_PAGE];
} mem_t; // dynamically allocates a mem_t structure and initializes its content


typedef struct ram
{
    byte_t RAM[SIZE];
    int free[NUMBER_FRAME];
} ram_t;


mem_t *initMem(); 
// allocates space in bytes (byte_t) using First-Fit, Best-Fit or Worst-Fit
address_t myContAlloc(mem_t *mp, int sz); // release memory that has already been allocated previously
hole_t* myContFree(mem_t *mp, address_t p, int sz); // assign a value to a byte

address_t myAlloc(mem_t *mp, int sz); // release memory that has already been allocated previously
void myFree(mem_t *mp, address_t p, int sz); // assign a value to a byte
void myWrite(mem_t *mp, address_t p, byte_t val); // read memory from a byte
byte_t myRead(mem_t *mp, address_t p);

#endif //It should be possible to use this memory as indicated in the main :