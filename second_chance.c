#include "memory_manager.h"
#include "disk.h"

void MMU_exception(MMU* mmu, int pos, int pid) {
    second_chance(mmu, pos, pid);
}

void second_chance(MMU* mmu, int pos, int pid) {
    FrameItem* disk_item = Find_FrameDiskItem(pid, pos);
    // we check that the frame on disk is valid
    assert(disk_item && "frame on disk not valid");

    int found = 0;
    int i = 0;
    while (!found) {
        if (i >= NUM_FRAMES) {
            i = 0;
        }

        PageEntry* entry = mmu->pages+i;
        // page is valid but read, unswappable and write flag
        // are 0 so we choose this frame as victim
        if (entry->flags == Valid) {    
            found = 1; 
            // we swap pages
            uint32_t frame_num = entry->frame_number;
            FrameItem* frame_item = Find_frame(pid, frame_num);

            // now we swap frame_item with disk_item

            remove_Frame(frame_item);
            // now we add the frame just removed in the disk memory area
            add_FrameItem(frame_item);

            remove_FrameDiskItem(disk_item);
            add_Frame(disk_item);
            uint32_t frame_num = disk_item->frame_num;
            // RICORDATI DI RISOLVERE QUESTO!!!!!!!!!!!!!!
            //m.frame_to_pid[frame_num] = disk_item->pid;

        }
        // otherwise we set the bit to zero and move on to the next page
        else if (entry->flags != Valid + Unswappable) {
            MMU_writeByte(mmu, pos, 0x1);
        }

        // Note: we don't cosider pages where the flag Unswappable is set;
        //  if this the case the page must remain in memory

        i++;

    }
}