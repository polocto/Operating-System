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
//Initializing my frame to free
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
//Create a new hole
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
// take the hole with the smaller difference with it and big enough
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
// Take the bigger space
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
        return mp->root;
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
    //Paging
    int first_page = virtual / PAGE_SIZE ; //define my first page use by my allocate memory
    int last_page = (virtual+sz -1) / PAGE_SIZE;//define my last page use by my allocate memory
    int free_frame = 0; //define it here, help us to not start for each page from the begining
    
    
    for( int page = first_page; page<=last_page; page++) //for each page from the first to the last
    {
        if(mp->page_table[page]<0)//if no frame allocated for the page
        {
            //Ask for frame
            while(ram.free[free_frame]!=0 && free_frame < (NUMBER_FRAME))free_frame ++ ;
            //If No free Frame
            if(free_frame>=(NUMBER_FRAME))
                exit(EXIT_FAILURE);
            ram.free[free_frame] = PROCESS_ID; //define which process runs on which frame (optional/only idicate it's taken fine)
            mp->page_table[page]=free_frame; //add new frame to page
        }
    }

    return virtual;
}
void myFree(mem_t *mp, address_t p, int sz){
    hole_t*actual = myContFree(mp,p,sz);
    //Paging
    if(!actual)//If allocation didn't work
        exit(EXIT_FAILURE);
    address_t begin = actual->adr;//first address of my hole
    address_t end = begin + actual->sz - 1;//last address of my hole

    int first_page = begin / PAGE_SIZE ;//page where is my first address
    int last_page = end / PAGE_SIZE;//page where is my last address
    
    //delete all page between first and la page of my hole excluding my last and first page
    for( int page = first_page+1; page<last_page; page++)
    {
        ram.free[mp->page_table[page]]=0;
        mp->page_table[page]=-1;
    }
    //Suppress first page if hole take it all
    if(begin%PAGE_SIZE == 0 && (end%PAGE_SIZE==PAGE_SIZE-1 || last_page > first_page ))
    {
        ram.free[mp->page_table[first_page]]=0;
        mp->page_table[first_page] = -1;
    }
    //Suppress last page if hole take it all
    if(end%PAGE_SIZE==PAGE_SIZE-1 && last_page > first_page)
    {
        ram.free[mp->page_table[last_page]]=0;
        mp->page_table[last_page]=-1;
    }

}

// assign a value to a byte
void myWrite(mem_t *mp, address_t p, byte_t val)
{
    address_t address_frame = mp->page_table[p/PAGE_SIZE]* PAGE_SIZE ;
    address_t offset = p%PAGE_SIZE;
    ram.RAM[address_frame+offset] = val;
}


// read memory from a byte
byte_t myRead(mem_t *mp, address_t p)
{
    address_t address_frame = mp->page_table[p/PAGE_SIZE]* PAGE_SIZE ;
    address_t offset = p%PAGE_SIZE;
    return ram.RAM[address_frame+offset];
}
