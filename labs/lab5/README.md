# Paging

**Paging** main intrest is to **isolate process memory** so it can't not access other processes memory.

With paging we can identify 2 different levels where memory is manage in different ways:
- Logical Memory
- Physical Memory

## Logical Memory
Logical memory, use **contiguous allocation**, this memory is not physical. We manage it with holes, that tells us what memory can be allocated to the process. And to free it we use **contiguous free**. Logical memory is what we can perceive from inside a process, example: all **cases of a table are contiguous**. Eventhough it's countigous, it is **divided in pages**. Pages is useful in isolating the process, will see how later.


## Physical Memory

On the other hand Physical Memory, for one process is not contigous, it is divided by **frames** we a define frame size *(page size)*.

## Paging

Paging is the **bridge between logical and physical memory**. Logical memory is divided in pages and Physical memory is divied in frames. **Frames and Pages have the same size**. 
When we allocate space in the logical memory, we are allocate memory in pages, and pages have to know in which frame we are allocate memory.
>One frame can only be allocated to one Process

Pages of processes will keep the corresponding index of the frame in order to be abale to write data in the coresponding allocated space in physical memory.