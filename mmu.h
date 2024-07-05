#pragma once
#include "constants.h"
#include <stdint.h>

#define OFFSET (LOGICAL_ADDRESS_NBITS - PAGE_NBITS)

// size of a memory page
#define PAGE_SIZE (1<<OFFSET)

#define NUM_FRAMES (PHYSICAL_MEMORY_SIZE / PAGE_SIZE)

#define PAGE_FLAGS_NBITS 3

// number of pages 
#define NUM_PAGES (1<<PAGE_NBITS)

// max memory
#define MAX_MEMORY (1<<LOGICAL_ADDRESS_NBITS)


typedef enum {
    Valid = 0x1,
    Unswappable = 0x2,
    read_bit = 0x3,
    write_bit = 0x4
} Flags;

typedef struct LogicalAddress {
    uint32_t page_number: PAGE_NBITS;
    uint32_t offset: OFFSET;
} LogicalAddress;

typedef struct PageEntry {
    uint32_t frame_number: PAGE_NBITS;
    uint32_t flags: PAGE_FLAGS_NBITS;
} PageEntry;

typedef struct MMU {
    PageEntry* pages;   // paging table
    uint32_t num_pages;
} MMU;

typedef uint32_t PhysicalAddress;

uint32_t getPhysicalAddress(MMU* mmu, LogicalAddress logical_address);

void MMU_writeByte(MMU* mmu, int pos, char c);

char* MMU_readByte(MMU* mmu, int pos);

void MMU_exception(MMU* mmu, int pos);