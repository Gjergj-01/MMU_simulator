#include <assert.h>
#include <stdio.h>
#include "mmu.h"

uint32_t getPhysicalAddress(MMU* mmu, LogicalAddress logical_address) {
    // we check that the page is in the page table
    
    assert((logical_address.page_number >= 0 && logical_address.page_number < mmu->num_pages) && "invalid page");

    PhysicalAddress physical_address;

    PageEntry page_entry = mmu->pages[logical_address.page_number];
    uint32_t frame_number = page_entry.frame_number;

    // Now we can retrieve the physical address
    // phyiscal address = [ frame | offset ]

    physical_address = (frame_number << OFFSET)|logical_address.offset;

    return physical_address;
}



