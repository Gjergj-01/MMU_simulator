#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "memory_manager.h"

GlobalMemoryLayout m;

//FrameItem physical_memory[NUM_FRAMES];

/*
    At the beginning the physical memory is all free and 
    the process list contains no processes. So:
    1. we fill the pages with pid -1 (invalid)
    2. we initialize all the buffer representing the 
        physical memory available
    3. we clear the process list
*/
void Memory_init() {
    for (int i = 0; i < NUM_PAGES; i++) {
        m.frame_to_pid[i] = -1;
    }

    for (int i = 0; i < NUM_FRAMES; i++) {
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
    ProcessMemoryItem* process = Memory_byPid(pid);
    assert(process->pid != pid && "invalid bit");

    ProcessMemoryItem* new_process = ProcessMemoryItem_alloc(pid);
    assert(new_process && "can't allocate a ProcessMemoryItem");
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