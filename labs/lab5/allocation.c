#include "allocation.h"
#include <stdlib.h>

//It should be possible to use this memory as indicated in the main :
int main() {
    mem_t *mem = initMem();

    address_t adr1 = myAlloc(mem, 5);
    address_t adr2 = myAlloc(mem, 10);
    address_t adr3 = myAlloc(mem, 100);

    myFree(mem, adr2, 10);
    myFree(mem, adr1, 5);

    myWrite(mem, adr3, 543);  // write on the 1st byte
    myWrite(mem, adr3+9, 34); // write on the 10th byte

    byte_t  val1 = myRead(mem, adr3);
    byte_t val2 = myRead(mem, adr3+9);


}


mem_t *initMem()
{
    mem_t* virtualMemory = (mem_t*)malloc(sizeof(mem_t));

    for(address_t i =0;i<SIZE;i++)
    {
        virtualMemory->mem[i]=0;
    }

    virtualMemory->root = (hole_t*)malloc(sizeof(hole_t));

    virtualMemory->root->adr = 0;
    virtualMemory->root->next = NULL;
    virtualMemory->root->prev = NULL;
    virtualMemory->root->sz = SIZE;

    return virtualMemory;
}

// release memory that has already been allocated previously
address_t myAlloc(mem_t *mp, int sz)
{
    address_t m_allocate_space;

    hole_t* m_hole = &mp->root;

///Select which hole can match data
    while (m_hole != NULL && m_hole->sz<sz)
    {
        m_hole = m_hole->next;
    }
//If none exit failure data
    if(!m_hole)
    {
        printf("Memory ERROR");
        exit(EXIT_FAILURE);
    }

//Allocating data
    m_allocate_space = m_hole->adr;
    for(int i = m_allocate_space; i< m_allocate_space+sz;i++)
    {
        mp->mem[i]=1;
    }

//Managing holes
    if(m_hole->sz == sz)//Same size -> suppress hole
    {
        if(m_hole->prev != NULL)
        {
            hole_t* prev = m_hole->prev;
            prev->next = m_hole->next;
        }
        else
        {
            
        }
    }
    else
    {
        m_hole->adr = m_allocate_space + sz;

    }

    return m_allocate_space;
}

// assign a value to a byte
void myFree(mem_t *mp, address_t p, int sz)
{

}

// read memory from a byte
void myWrite(mem_t *mp, address_t p, byte_t val)
{

}


byte_t myRead(mem_t *mp, address_t p)
{

}