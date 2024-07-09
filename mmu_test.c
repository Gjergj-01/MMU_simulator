#include <stdio.h>
#include "mmu.h"

int main(int argc, char** argv) {
    printf("ADDRESS_NBITS: %d, size: %d\n", LOGICAL_ADDRESS_NBITS, MAX_MEMORY);
    printf("PAGE_NBITS: %d, num_pages: %d, frame_size: %d\n", PAGE_NBITS, NUM_PAGES, PAGE_SIZE);

    PageEntry pages[NUM_PAGES];
    uint32_t num_pages = NUM_PAGES;

    MMU mmu;
    mmu.pages = pages;
    mmu.num_pages = num_pages;


    // we fill the pages to run some tests
    for (int i = 0; i < NUM_PAGES; i++) {
        PageEntry* entry = pages+i;
        entry->frame_number = i;
        entry->flags = Valid;
    }

    for (int p = 0; p < 2; p++) {
        LogicalAddress logical_address;
        logical_address.page_number = p;
        for (int i = 0; i < PAGE_SIZE; i++) {
            logical_address.offset = i;
            printf("logical address: [p: %x, o: %x] -> ", 
                logical_address.page_number,
                logical_address.offset);
            PhysicalAddress physical_address = getPhysicalAddress(&mmu, logical_address);
            printf("physical_address: [%x] \n", physical_address);

        }
    }

    return 0;

}