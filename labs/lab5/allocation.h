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
    address_t adr;// address on the hole at the begining
    int sz;//size of the hole
    struct hole *next;//next hole
    struct hole *prev;// previous hole
} hole_t;

typedef struct {
    hole_t* root; // holes list
    int page_table[NUMBER_PAGE];//pages in process
} mem_t; // dynamically allocates a mem_t structure and initializes its content


typedef struct ram
{
    byte_t RAM[SIZE];//Physical memory
    int frame[NUMBER_FRAME];//frames
} ram_t;


mem_t *initMem();

hole_t* allocHole(address_t p, int sz, hole_t* prev, hole_t* next);

// allocates space in bytes (byte_t) using First-Fit, Best-Fit or Worst-Fit
hole_t* firstFit(hole_t* m_hole, int sz);
hole_t* bestFit(hole_t* m_hole, int sz);
hole_t* worstFit(hole_t* m_hole, int sz);

address_t myContAlloc(mem_t *mp, int sz); // release memory that has already been allocated previously
hole_t* myContFree(mem_t *mp, address_t p, int sz); // assign a value to a byte

address_t myAlloc(mem_t *mp, int sz); // release memory that has already been allocated previously
void myFree(mem_t *mp, address_t p, int sz); // assign a value to a byte

void myWrite(mem_t *mp, address_t p, byte_t val); // read memory from a byte
byte_t myRead(mem_t *mp, address_t p);

//free all allocated memory
void freeMem(mem_t *virtualMemory);

#endif //It should be possible to use this memory as indicated in the main :