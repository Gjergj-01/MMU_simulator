#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "memory_manager.h"
#include "disk.h"

GlobalMemoryLayout m;

/*
    At the beginning the physical memory is all free and 
    the process list contains no processes. So:
    1. we fill the pages with pid -1 (invalid)
    2. we initialize all the buffer representing the 
        physical memory available
    3. we clear the process list
*/
void Memory_init() {
    List_init(&m.frame_list);

    for (int i = 0; i < NUM_FRAMES; i++) {
        m.frame_to_pid[i] = -1;
        FrameItem* pitem = FrameItem_alloc();
        FrameItem_init(pitem, -1, i+1);
        List_insert(&m.frame_list, m.frame_list.last, (ListItem*)pitem);
    }

    List_init(&m.process_list);
    printf("free frames: %d\n", m.frame_list.size);

}

/*
    1. we release all process resources
    2. we relese the frames
*/

void Memory_shutdown() {
    while (m.process_list.first) {
        ProcessMemoryItem* process = (ProcessMemoryItem*) m.process_list.first;
        Memory_destroyProcessMemoryItem(process);
    }

    while (m.frame_list.first) {
        FrameItem* frame = (FrameItem*) m.frame_list.first;
        List_detach(&m.frame_list, m.frame_list.first);
        FrameItem_free(frame);
    }
}


// given the pid we retrieve the ProcessMemoryItem (if exists)

ProcessMemoryItem *Memory_byPid(int pid) {
    return ProcessMemoryItem_byPid(&m.process_list, pid);
}

/*
    Adds a new ProcessMemoryItem to process_list.
    We need to check that a process with the same pid 
    is not already in the list
*/

ProcessMemoryItem* Memory_addProcessItem(int pid) {
    assert(!Memory_byPid(pid) && "invalid_pid");

    // all fine we can add the new process
    ProcessMemoryItem* new_process = ProcessMemoryItem_alloc(pid);
    //assert(new_process && "can't allocate a ProcessMemoryItem");
    ProcessMemoryItem_init(new_process, pid);
    List_insert(&m.process_list, m.process_list.last, (ListItem*) new_process);
    return new_process;

}

uint32_t Memory_freePages() {
    return m.frame_list.size;
}

/*
    To destroy a ProcessMemoryItem we need:
    1. Set to -1 (invalid) the pid associated to each frame
    2. Detach the ProcessMemoryItem from the process_list
*/

void Memory_destroyProcessMemoryItem(ProcessMemoryItem* item) {
    for (int i = 0; i < NUM_PAGES; i++) {
        if (m.frame_to_pid[i] == item->pid) {
            m.frame_to_pid[i] = -1;
        }
    }

    List_detach(&m.process_list, (ListItem *) item);
    ProcessMemoryItem_free(item);
}

void print_PhysicalMemory() {
    ListItem* aux = m.frame_list.first;
    while (aux) {
        FrameItem* frame = (FrameItem*) aux;
        printf("FRAME_NUM: %d\n", frame->frame_num);
        aux = aux->next;
    }

}

/*
    Given the frame_num we retireve the 
    frame_item
*/

FrameItem* Find_frame(int pid, uint32_t frame_num) {
    ListItem* aux = m.frame_list.first;

    while (aux) {
        FrameItem* item = (FrameItem*) aux;
        if (item->frame_num == frame_num) {
            return item;
        }
        aux = aux->next;
    }

    return 0;
}

FrameItem* remove_Frame(FrameItem* item) {
    ListItem* removed_item = List_detach(&m.frame_list, (ListItem*) item);
    return (FrameItem*) removed_item;
}

// we simply add the frame at the end of the frame list

FrameItem* add_Frame(FrameItem* item) {
    List_insert(&m.frame_list, m.frame_list.last, (ListItem*) item);
    return item;
}


/*
    This function mapps pages into frames. Until we have enough physical memory
    we map it in the physical memory, then we "map" the pages in the disk memory.
    NOTE: Here the disk memory holds FrameItems but in this case the field "frame_num"
          memorizes the index of the paging table and not the frame number.
*/

void assign_pages(ProcessMemoryItem* pmem, int num_pages, uint32_t flags) {
    int index = -1;
    // we check for the index from which we can start writing new pages
    for (int i = 0; i < NUM_PAGES; i++) {
        if (pmem->pages[i].frame_number == 0 && pmem->pages[i].flags == 0) {
            index = i;
            break;
        }
    }

    //printf("PID: %d  ->  [index: %d]\n",pmem->pid, index);

    if (index == -1) index = 0;

    assert((index + num_pages) < NUM_PAGES && "not enough memory");

    for (int i = 0; i < num_pages; i++) {
        if (Memory_freePages() > 0) {
            // we look for a free frame 
            FrameItem* new_frame;
            ListItem* aux = m.frame_list.first;
            while (aux) {
                FrameItem* frame = (FrameItem*) aux;
                if (frame->pid == -1) {
                    new_frame = frame;
                    break;
                }
                aux = aux->next;
            }
            new_frame->pid = pmem->pid;
            pmem->pages[i+index].frame_number = new_frame->frame_num;
            pmem->pages[i+index].flags |= flags;
            m.frame_to_pid[new_frame->frame_num] = pmem->pid;
            List_detach(&m.frame_list, (ListItem*) new_frame);
            List_insert(&pmem->frame_list, pmem->frame_list.last, (ListItem*) new_frame);
        }
        else {
            // we finished space so we need to "map" the pages on disk
            pmem->pages[i].flags = Valid;
            FrameItem* new_frame = FrameItem_alloc();
            FrameItem_init(new_frame, pmem->pid, i+index);
            add_FrameDiskItem(new_frame);
        }
    }

}

void simulate_work(ProcessMemoryItem* pmem, int pos, uint32_t flags) {
    // we check that the index is Valid
    assert(pos >= 0 && pos < NUM_PAGES && "invalid index");

    // we check that the page has been assigend, if not we check the flag.
    // If this is "unswappable" we must put the page in the physical memory
    // so if there is no free space we call the second chance algorithm
    // otherwise we just assign it to a free frame. If the flag is not 
    // unswappable we can just call the assign_pages function

    if (pmem->pages[pos].flags == 0) {

        if (flags == (Valid + Unswappable)) {
            if (Memory_freePages() > 0) {
                assign_pages(pmem, 1, (Valid + Unswappable));
            }
            else {
                MMU_exception(pmem, pos);
            }
        }
    }

    else {
        if (pmem->pages[pos].frame_number != 0) {
            if (flags == write_bit) {
                MMU_writeByte(pmem, pos, flags);
            }
            else {
                MMU_readByte(pmem, pos);
            }
        }
        else {
            MMU_exception(pmem, pos);
        }
    }


}   

void MMU_writeByte(ProcessMemoryItem* pmem, int pos, char c) {
    // we check that the index is valid 
    assert((pos >= 0 && pos < NUM_PAGES) && "invalid index");

    // we check that the flag is valid 
    // in this case it can be "write_bit" or "read_bit"
    assert((c == write_bit || c == read_bit) && "invalid flag");

    // if flag and pos are valid we associate the flag to the corresponding page
    pmem->pages[pos].flags |= c;

}

char* MMU_readByte(ProcessMemoryItem* pmem, int pos) {
    // we check that the index is valid 
    assert((pos >= 0 && pos < NUM_PAGES) && "index out of range");

    // if so we read the flag associated to the corresponding page
    char* flag = 0;
    *flag = pmem->pages[pos].flags;

    // we also set the flags to read_bit
    pmem->pages[pos].flags |= read_bit;

    return flag;
}

void MMU_exception(ProcessMemoryItem* pmem, int pos) {
    second_chance(pmem, pos);
}


void second_chance(ProcessMemoryItem* pmem, int pos) {
    int pid = pmem->pid;

    FrameItem* disk_item = Find_FrameDiskItem(pid, pos);
    // we check that the frame on disk is valid
    assert(disk_item && "frame on disk not valid");

    /*
        We need to find a victim frame. To do so we need to access
        each ProcessMemoryItem and look in their frame_lists.
    */

    int found = 0;
    //int i = 0;
    while(!found) {
        //if (i >= NUM_FRAMES) i = 0;

        ListItem* item = m.process_list.first;
        while (item) {
            if (found) break;

            ProcessMemoryItem* process = (ProcessMemoryItem*) item;
            int victim_pid = process->pid;

            ListItem* aux = process->frame_list.first;
            while (aux) {
                if (found) break;

                FrameItem* victim = (FrameItem*) aux;
                // now we need to retireve the PageEntry 
                uint32_t v_frame_num = victim->frame_num;
                for (int k = 0; k < NUM_PAGES; k++) {
                    PageEntry entry = process->pages[k];
                    if (entry.frame_number == v_frame_num) {
                        // once we find the correspondig page we check
                        // the flags
                        if (entry.flags == Valid) {
                            found = 1;
                            // we swap pages
                            List_detach(&process->frame_list, (ListItem*) victim);
                            // now we save the frame victim on memory disk
                            victim->frame_num = k;
                            printf("victim_pid %d\n", victim->pid);
                            add_FrameDiskItem(victim);

                            ProcessMemoryItem_addFrame(&pmem->frame_list, pmem->frame_list.last, disk_item);
                            m.frame_to_pid[v_frame_num] = pmem->pid;

                            printf("[Swapped-out page with index %d of porcess with PID %d\n",
                                k, victim_pid);
                            printf(" Swapped-in frame corresponding to page %d of process with PID %d]\n\n",
                                disk_item->frame_num, pid);
                            break;
                        }
                    }
                    else if (entry.flags == (Valid + Unswappable)) {
                        continue;
                    }
                    else {
                        entry.flags = Valid;
                    }
                }
                aux = aux->next;
            }

            item = item->next;
        }


    }
   

}