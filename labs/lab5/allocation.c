#include "allocation.h"
#include <stdlib.h>
#include <stdio.h>
#define PROCESS_ID 4262
ram_t ram;
//It should be possible to use this memory as indicated in the main :
int main() {
    mem_t *mem = initMem();

    address_t adr1 = myAlloc(mem, 5); //allocate 5 byte into address 1
    address_t adr2 = myAlloc(mem, 10);
    address_t adr3 = myAlloc(mem, 100);

    myFree(mem, adr2, 10); // free address 2 à the 9 following adresses
    myFree(mem, adr1, 5);

    myWrite(mem, adr3, 543);  // write on the 1st byte
    myWrite(mem, adr3+9, 34); // write on the 10th byte

    byte_t  val1 = myRead(mem, adr3); // read value on address 3
    byte_t val2 = myRead(mem, adr3+9);

    printf("Val1 : %d\n", val1);
    printf("Val2 : %d\n", val2);

}



//Exercise
//initialization of virtual memory
mem_t *initMem()
{
    //creating virtual memory
    mem_t* virtualMemory = (mem_t*)malloc(sizeof(mem_t));

    for(int i=0; i<NUMBER_FRAME;i++)
    {
        ram.free[i]=0;
    }

//------Initialization of free space indication
    virtualMemory->root = (hole_t*)malloc(sizeof(hole_t)); 
    for(int i = 0; i< NUMBER_PAGE ; i++)
    {
        virtualMemory->page_table[i]=-1;
    }

    virtualMemory->root->adr = 0; // Adresse of the begining of free spaces
    // Only one big hole at the begining
    virtualMemory->root->next = NULL;
    virtualMemory->root->prev = NULL;
    // SIZE free spaces
    virtualMemory->root->sz = SIZE;

    return virtualMemory;
}

hole_t* allocHole(address_t p, int sz, hole_t* prev, hole_t* next)
{
    hole_t* new = NULL;
    new = (hole_t*)malloc(sizeof(hole_t));

    new->adr=p;
    new->sz = sz;
    new->prev = prev;
    new->next = next;

    if(prev)
    {
        prev->next = new;
    }

    if(next)
    {
        next->prev = new;
    }

    return new;
}

// first big enough hole
hole_t* firstFit(hole_t* m_hole, int sz)
{
    while (m_hole != NULL && m_hole->sz < sz)
    {
        m_hole = m_hole->next;
    }

    return m_hole;
}
// take the hole with the smaller difference with it
hole_t* bestFit(hole_t* m_hole, int sz)
{
    hole_t* temp=NULL;
    while (m_hole != NULL)
    {
        if((!temp || abs(m_hole->sz-sz) < abs(temp->sz-sz)) && m_hole->sz>=sz)
        {
            temp = m_hole;
        }
        m_hole = m_hole->next;
    }
    m_hole = temp;

    return m_hole;
}
// Should take the bigger space and go in the middle of it
hole_t* worstFit(hole_t* m_hole, int sz)
{
    hole_t* temp=NULL;
    while (m_hole != NULL)
    {
        if((!temp || abs(m_hole->sz-sz) > abs(temp->sz-sz)) && m_hole->sz>=sz)
        {
            temp = m_hole;
        }
        m_hole = m_hole->next;
    }
    m_hole = temp;

    return m_hole;
}

address_t myContAlloc(mem_t *mp, int sz)
{
    address_t m_allocate_space;
    hole_t* m_hole = mp->root;
    //Choose the position of the hole
    m_hole = firstFit(m_hole,sz);
    //m_hole = bestFit(m_hole, sz);
    //m_hole = worstFit(m_hole, sz);

//If none exit failure data
    if(!m_hole)
    {
        printf("Memory ERROR");
        exit(EXIT_FAILURE);
    }
    hole_t* prev = m_hole->prev;
    hole_t* next = m_hole->next;

//Allocating data
    m_allocate_space = m_hole->adr;

//Managing holes
    if(m_hole->sz == sz)//Same size -> suppress hole
    {
        if(prev != NULL) //if there is a previous
        {
            prev->next = next;
        }
        else
        {
            mp->root = next;
        }
        if(next!=NULL)//If there is a next
        {
            next->prev = prev;
        }
        free((void*)m_hole); //delete dynamic memory
    }
    else //reducing hole size
    {
        m_hole->adr += sz;
        m_hole->sz -= sz;
    }
    //printf("Address : %d / Size : %d\nHole Address : %d/ Size : %d\n\n",m_allocate_space, sz, mp->root->adr, mp->root->sz );
    return m_allocate_space;
}

// release memory that has already been allocated previously
hole_t* myContFree(mem_t *mp, address_t p, int sz)
{
    hole_t* next = mp->root, * prev = NULL;

    if(!next)
    {
        mp->root = allocHole(p,sz,NULL,NULL);
        return NULL;
    }
    
    while( next != NULL && p > next->adr)//while I have a next hole and that the variable representing the next hole is before me go to the following hole
    {
        prev = next;
        next = next->next;
    }
    hole_t* actual = allocHole(p,sz,prev,next);// allocate space for my hole
// if actual follows the precedent hole then merge precedent in actual
    if(prev && prev->adr + prev->sz == actual->adr)
    {
        prev->sz += actual->sz;
        prev->next = actual->next;
        if(actual->next)
        {
            actual->next->prev = prev;
        }
        free(actual);
        actual = prev;
    }
// if next follows the actual hole then merge next in actual
    if(next && actual->adr + actual->sz == next->adr)
    {
        next->sz += actual->sz;
        next->adr = actual->adr;
        next->prev = actual->prev;
        if(actual->prev)
        {
            actual->prev->next = next;
        }
        free(actual);
        actual = next;
    }

// if actual have no precedent define actual as the root hole
    if(!actual->prev)
    {
        mp->root = actual;
    }
    return actual;
}

address_t myAlloc(mem_t *mp, int sz){
    address_t virtual = myContAlloc(mp,sz);
    int i = virtual / PAGE_SIZE ;
    int j = (virtual+sz -1) / PAGE_SIZE;
    
    
    for( int page = i; page<=j; page++)
    {
        if(mp->page_table[page]<0)
        {
            //Demander frame
            int free_frame = 0;
            while(ram.free[free_frame]!=0 && free_frame < (NUMBER_FRAME))free_frame ++ ;

            if(free_frame>=(NUMBER_FRAME))
                exit(EXIT_FAILURE);
            ram.free[free_frame] = PROCESS_ID;
            mp->page_table[page]=free_frame; //page nouvelle frame
        }
    }

    return virtual;
}
void myFree(mem_t *mp, address_t p, int sz){
    hole_t*actual = myContFree(mp,p,sz);
    if(!actual)
        exit(EXIT_FAILURE);
    address_t begin = actual->adr;
    address_t end = actual->sz - 1;

    int i = begin / PAGE_SIZE ;
    int j = end / PAGE_SIZE;
    
    
    for( int page = i+1; page<j; page++)
    {
        ram.free[mp->page_table[page]]=0;
        mp->page_table[page]=-1;
    }

    if(begin%PAGE_SIZE == 0 && (end%PAGE_SIZE==PAGE_SIZE-1 || j > i ))
    {
        ram.free[mp->page_table[i]]=0;
        mp->page_table[i] = -1;
    }
    if(end%PAGE_SIZE==PAGE_SIZE-1 && j > i)
    {
        ram.free[mp->page_table[j]]=0;
        mp->page_table[j]=-1;
    }

}

// assign a value to a byte
void myWrite(mem_t *mp, address_t p, byte_t val)
{
    //*(mp->mem+p) = val; //mp->mem[p] = val;
    address_t page = mp->page_table[p/PAGE_SIZE]* PAGE_SIZE ;
    address_t offset = p%PAGE_SIZE;
    ram.RAM[page+offset] = val;
}


// read memory from a byte
byte_t myRead(mem_t *mp, address_t p)
{
    //return *(mp->mem+p);
    address_t page = mp->page_table[p/PAGE_SIZE]* PAGE_SIZE ;
    address_t offset = p%PAGE_SIZE;
    return ram.RAM[page+offset];
}