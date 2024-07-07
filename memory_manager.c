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
        FrameItem_init(pitem, -1, i);
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
    //ProcessMemoryItem* process = Memory_byPid(pid);
    assert(!Memory_byPid(pid) && "invalid_pid");
    //assert(process->pid != pid && "invalid bit");
    //printf("ALL FINE\n");

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
        printf("PID: %d, FRAME_NUM: %d\n", frame->pid, frame->frame_num);
        aux = aux->next;
    }

}

/*
    Given the pid and the frame_num we retireve the 
    frame_item
*/

FrameItem* Find_frame(int pid, uint32_t frame_num) {
    ListItem* aux = m.frame_list.first;

    while (aux) {
        FrameItem* item = (FrameItem*) aux;
        if (item->frame_num == frame_num && item->pid == pid) {
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

// this could become assign pages to frames;
// void add_Pages(ProcessMemoryItem* pmem, int num_pages, uint32_t flags) {
//     int index = -1;
//     // we check for the index from which we can start writing 
//     // new pages
//     for (int i = 0; i < NUM_PAGES; i++) {
//         if (pmem->pages[i].flags == (~Valid)) {
//             index = i;
//             break;
//         }
//     }
//     assert((index + num_pages) < NUM_PAGES && "not enough memory");
//     for (int i = 0; i < num_pages; i++) {
//         if (Memory_freePages() > 0) {
//             // here we look for a free frame
//             FrameItem* new_frame;
//             ListItem* aux = m.frame_list.first;
//             while(aux) {
//                 FrameItem* frame = (FrameItem*) aux;
//                 if (frame->pid == -1) {
//                     new_frame = frame;
//                     break;
//                 }
//             }
//             pmem->pages[i+index].frame_number = new_frame->frame_num;
//             pmem->pages[i+index].flags |= flags;
//         }
        
//     }

// }

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
        if (pmem->pages[i].frame_number == -1) {
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
            pmem->pages[i+index].frame_number = new_frame->frame_num;
            pmem->pages[i+index].flags |= flags;
            m.frame_to_pid[new_frame->frame_num] = pmem->pid;
            List_detach(&m.frame_list, (ListItem*) new_frame);
            List_insert(&pmem->frame_list, pmem->frame_list.last, (ListItem*) new_frame);
        }
        else {
            // we finished space so we need to "map" the pages on disk
            FrameItem* new_frame = FrameItem_alloc();
            FrameItem_init(new_frame, pmem->pid, i+index);
            add_FrameDiskItem(new_frame);
        }
    }

}