#include <assert.h>
#include <stdio.h>
#include "mmu.h"

uint32_t getPhysicalAddress(MMU* mmu, LogicalAddress logical_address) {
    //1. check that the page is in the page table
    //printf("page_number: %d\n", logical_address.page_number);
    //printf("num_pages: %d\n", mmu->num_pages);
    assert(logical_address.page_number < mmu->num_pages && "invalid page");

    PhysicalAddress physical_address;

    PageEntry page_entry = mmu->pages[logical_address.page_number];
    uint32_t frame_number = page_entry.frame_number;

    // Now we can retrieve the physical address
    // phyiscal address = [ frame | offset ]

    physical_address = (frame_number << OFFSET)|logical_address.offset;

    return physical_address;
}

void MMU_writeByte(MMU* mmu, int pos, char c) {
    // we check that the index is valid
    assert(pos < mmu->num_pages && "index out of range");

    // we check that the flag is valid 
    // in this case it can be "write_bit" or "read_bit"
    assert((c == write_bit || c == read_bit) && "invalid flag");

    // if flag and pos are valid we associate the flag to the corresponding page
    mmu->pages[pos].flags = c;
    
    return;
}

char* MMU_readByte(MMU* mmu, int pos) {
    // we check that the index is valid
    assert(pos < mmu->num_pages && "index out of range");

    // if so we read the flag associated to the corresponding page
    char* flag = NULL;
    *flag = mmu->pages[pos].flags;

    return flag; 

}

void MMU_exception(MMU* mmu, int pos) {
    
}