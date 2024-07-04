#include <assert.h>
#include "mmu.h"

uint32_t getPhysicalAddress(MMU* mmu, LogicalAddress logical_address) {
    //1. check that the page is in the page table
    assert(logical_address.page_number < mmu->num_pages && "invalid page");

    PhysicalAddress physical_address;

    PageEntry page_entry = mmu->pages[logical_address.page_number];
    uint32_t frame_number = page_entry.frame_number;

    // Now we can retrieve the physical address
    // phyiscal address = | frame | offset |

    physical_address = (frame_number << FRAME_NBITS)|logical_address.offset;

    return physical_address;
}