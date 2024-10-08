#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory_manager.h"
#include "disk.h"

GlobalMemoryLayout m;

/*
    At the beginning the physical memory is all free and 
    the process list contains no processes. So:
    1. we initialize the frames with pid -1 (meaning that they have not 
        been assigned to any process)
    2. we clear the process list
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
    if (m.process_list.size > 0) {
        while (m.process_list.first) {
            ProcessMemoryItem* process = (ProcessMemoryItem*) m.process_list.first;
            Memory_destroyProcessMemoryItem(process);
        }
    }

    if (m.frame_list.size > 0) {
        while (m.frame_list.first) {
        FrameItem* frame = (FrameItem*) m.frame_list.first;
        List_detach(&m.frame_list, (ListItem*) frame);
        free(frame);
        }
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
    2. Free all the frames associated to that process
    3. Detach the ProcessMemoryItem from the process_list
*/

void Memory_destroyProcessMemoryItem(ProcessMemoryItem* pmem) {
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (m.frame_to_pid[i] == pmem->pid) {
            m.frame_to_pid[i] = -1;
        }
    }
    while (pmem->frame_list.first) {
        FrameItem* frame = (FrameItem*) pmem->frame_list.first;
        List_detach(&pmem->frame_list, (ListItem*) frame);
        free(frame);
    }

    List_detach(&m.process_list, (ListItem *) pmem);
    free(pmem);
}

/*
    Prints the physical memory still available
*/

void print_PhysicalMemory() {
    ListItem* aux = m.frame_list.first;
    while (aux) {
        FrameItem* frame = (FrameItem*) aux;
        printf("FRAME_NUM: %d\n", frame->frame_num);
        aux = aux->next;
    }

}

/*
    Given the frame_num we retireve the frame_item 
    (if this is available in the physical memory)
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
    This function maps pages into frames. Until we have enough physical memory
    we map it in the physical memory, then we "map" the pages in the disk memory.
    NOTE: Here the disk memory holds FrameItems but in this case the field "frame_num"
          memorizes the index of the paging table and not the frame number. We do this
          because when we need to swap-in a frame we want to know to which process
          (we do this thanks to the pid) and to which page in the paging table corresponds
          the frame.
    When a prcocess requests the assignment of a page with flag unswappable, we need to map
    this in the physical memory. If we don't have enough free space we first map the page 
    in the disk memory, then we call an MMU_exception that swaps-in the page.
*/

void assign_pages(ProcessMemoryItem* pmem, int num_pages, uint32_t flags) {
    int index = -1;
    // we check for the index from which we can start writing new pages
    for (int i = 0; i < NUM_PAGES; i++) {
        if (pmem->pages[i].flags == Invalid) {
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
            pmem->pages[i+index].flags = flags;
            m.frame_to_pid[new_frame->frame_num] = pmem->pid;
            List_detach(&m.frame_list, (ListItem*) new_frame);
            List_insert(&pmem->frame_list, pmem->frame_list.last, (ListItem*) new_frame);
        }
        else if (flags == (Valid + Unswappable)) { 
            // we map the pages on disk 
            pmem->pages[i+index].flags = (Valid + Unswappable);
            FrameItem* new_frame = FrameItem_alloc();
            FrameItem_init(new_frame, pmem->pid, i+index);
            //printf("ASSING -> Page_index: %d, PID: %d   ", new_frame->frame_num, new_frame->pid);
            add_FrameDiskItem(new_frame);
            // now, since the page is Unswappable, we need
            // to swap it in, on physical memory
            MMU_exception(pmem, index+i);
        }
        else {
            // we finished space so we need to "map" the pages on disk
            pmem->pages[i+index].flags = Valid;
            FrameItem* new_frame = FrameItem_alloc();
            FrameItem_init(new_frame, pmem->pid, i+index);
            add_FrameDiskItem(new_frame);
        }
    }

}

/*
    If a page has flag 'Invalid', it means it has not been allocated so if we try
    to access that page we generate a SEGMENTATION_FALUT and terminate the program.
    If a page has flag 'Valid' but frame_num 0, it means that the page has been allocated
    but it's stored on disk memory, so we call an MMU_exception and swap it in.
*/

void MMU_writeByte(ProcessMemoryItem* pmem, int pos, char c) {
    // we check that the index is valid 
    assert((pos >= 0 && pos < NUM_PAGES) && "invalid index");

    // if we try to access a page not allocated we genrate a PAGE_FAULT
    assert(pmem->pages[pos].flags != Invalid && "page_fault(segmentation_fault)");

    if (pmem->pages[pos].flags == Valid && pmem->pages[pos].frame_number == 0) {
        // we need to swap in the page that is on disk memory
        MMU_exception(pmem, pos);
    }
    else {
        // the page is in physical memory so we can simply write the character
        pmem->pages[pos].c = c;

    }

    // we set the flags to write_bit;
    pmem->pages[pos].flags |= write_bit;

}

char MMU_readByte(ProcessMemoryItem* pmem, int pos) {
    // we check that the index is valid 
    assert((pos >= 0 && pos < NUM_PAGES) && "index out of range");

    // if we try to access a page not allocated we genrate a PAGE_FAULT
    assert(pmem->pages[pos].flags != Invalid && "page_fault(segmentation_fault)");

    // we read the flag associated to the corresponding page
    if (pmem->pages[pos].flags == Valid && pmem->pages[pos].frame_number == 0) {
        // we need to swap in the page that is on disk memory
        MMU_exception(pmem, pos);
    }
    
    // now we can read the character
    char c = 0;
    c = pmem->pages[pos].c;

    // we set the flags to read_bit
    pmem->pages[pos].flags |= read_bit;

    return c;
}

void MMU_exception(ProcessMemoryItem* pmem, int pos) {
    printf("MMU_exception\n");
    second_chance(pmem, pos);
}

/*
    To access the physical memory we need to access each ProcessMemoryItem and
    explore each frame_list. If we find a page with flag 'Valid' we choose that
    frame as victim; if the flag is 'Unswappable' or 'Invalid' we skip to the next 
    page otherwise we set the flag to 'Valid' and move on to the next page.
*/

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
    while(!found) {

        // here we start exploring the processes
        ListItem* item = m.process_list.first;
        while (item) {
            if (found) break;

            ProcessMemoryItem* process = (ProcessMemoryItem*) item;
            int victim_pid = process->pid;

            // now for each process we explore his frames
            ListItem* aux = process->frame_list.first;
            while (aux) {
                if (found) break;

                FrameItem* victim = (FrameItem*) aux;
                // now we need to retrieve the PageEntry 
                uint32_t v_frame_num = victim->frame_num;
                for (int k = 0; k < NUM_PAGES; k++) {
                    PageEntry entry = process->pages[k];
                    if (entry.frame_number == v_frame_num) {
                        // once we find the correspondig page we check
                        // the flags
                        if (entry.flags == Valid) {
                            found = 1;
                            // we swap pages
                            process->pages[k].frame_number = 0;
                            List_detach(&process->frame_list, (ListItem*) victim);
                            // now we save the frame victim on memory disk
                            victim->frame_num = k;
                            add_FrameDiskItem(victim);

                            ProcessMemoryItem_addFrame(&pmem->frame_list, pmem->frame_list.last, disk_item);
                            m.frame_to_pid[v_frame_num] = pmem->pid;

                            printf("[Swapped-out page with index %d of process with PID %d\n",
                                k, victim_pid);
                            printf(" Swapped-in frame corresponding to page %d of process with PID %d]\n\n",
                                disk_item->frame_num, pid);
                            break;
                        }
                    }
                    else if (entry.flags == (Valid + Unswappable) || entry.flags == (Valid + Unswappable + read_bit)
                                || entry.flags == (Valid + Unswappable + write_bit) || entry.flags == (Valid + Unswappable + read_bit + write_bit)) {
                        continue;
                    }
                    else if (entry.flags == Invalid) {
                        continue;
                    }
                    else {
                        process->pages[k].flags = Valid;
                    }
                }
                aux = aux->next;
            }

            item = item->next;
        }
    }
}