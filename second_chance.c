#include "memory_manager.h"
#include "disk.h"

void MMU_exception(MMU* mmu, int pos, int pid) {
    second_chance(mmu, pos, pid);
}

void second_chance(MMU* mmu, int pos, int pid) {
    DiskItem* disk_item = Find_DiskItem(pid, pos);
    // we check that the frame on disk is valid
    assert(disk_item && "frame on disk not valid");

    int find = 0;
    int i = 0;
    while (!find) {
        if (i >= NUM_FRAMES) {
            i = 0;
        }

        PageEntry* entry = mmu->pages+i;
        // page is valid but read, unswappable and write flag
        // are 0 so we choose this frame as victim
        if (entry->flags == Valid) {    
            find = 1; 
            // we swap pages
            uint32_t frame_num = entry->frame_number;
            FrameItem* frame_item = Find_frame(pid, frame_num);

            // now we swap frame_item with disk_item

        }
        // otherwise we set the bit to zero and move on to the next page
        else if (entry->flags != Valid + Unswappable) {
            MMU_writeByte(mmu, pos, 0x1);
        }

        i++;

    }
}