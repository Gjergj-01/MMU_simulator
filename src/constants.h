// bits used for logical address
#define LOGICAL_ADDRESS_NBITS 24

// bits used as index into the page table
#define PAGE_NBITS 14

#define PHYSICAL_MEMORY_SIZE (1<<20)

/*
    offset = 24 - 14 = 10 
    thus, since we have 1 MB of physical memory,
    a physical address will be composed this way:

        [frame | offset]
        10 bits   10 bits

    On physical memory we can have a maximum of 1024 frames
*/