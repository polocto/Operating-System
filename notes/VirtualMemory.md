# Virtual Memory

RAM = Physical Memory

- Allocate (All about it)
- Release (not complex)
- Read (not complex)
- Write (not complex)

## Allocation

### 1st Approach

`Allocate(nb /*of words*/)`

#### Algorithm

- find `nb` of available free contiguous words
- Return the address of the 1st word (slow down the read and write process)

### 2nd Approach (Virtual memory/Paging)

#### Algorithm

- find `nb` of available free contiguous words inside logical memory
- For each allocated word in 1, find a free word inside the Ram & update the mapping
- Return the address of the 1st word (slow down the read and write process)





