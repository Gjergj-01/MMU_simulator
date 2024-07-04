// bits used for logical address
#define LOGICAL_ADDRESS_NBITS 24

// bits used as index into the page table
#define PAGE_NBITS 14


/*
    offset = 24 - 14 = 10 
    quindi, avendo a disposizione 1MB di memoria fisica,
    un indirizzo di questa sara composto nel seguente modo:

        [frame | offset]
        10 bit   10 bit

    Nella memoria fisica possiamo quindi avere 1024 frame totali
*/