#pragma once 
#include <assert.h>
#include <stdio.h>
#include "process_memory_item.h"
#include "linked_list.h"

/*
    Here we keep track of the free phyiscal memory and the 
    processes. The maximum size of the physical memory is 
    1 MB and it can store maximum 1024 pages.
    We also initialize an array to tell as to which process 
    is assigned each page in the paging table
*/

typedef struct GlobalMemoryLayout {
    int frame_to_pid[NUM_PAGES];
    ListHead frame_list;
    ListHead process_list;
} GlobalMemoryLayout;


void Memory_init();
void Memory_shutdown();
ProcessMemoryItem* Memory_byPid(int pid);
ProcessMemoryItem* Memory_addProcessItem(int pid);
void Memory_freePages();
void Memory_destroyProcessMemoryItem(ProcessMemoryItem* item);


