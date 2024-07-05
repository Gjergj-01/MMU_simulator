#pragma once 
#include "frame_item.h"

typedef struct ProcessMemoryItem {
    ListItem list;
    int pid;
    PageEntry pages[NUM_PAGES];
    
} ProcessMemoryItem;

ProcessMemoryItem* ProcessMemoryItem_alloc();
ProcessMemoryItem* ProcessMemoryItem_byPid(ListHead* head, int pid);

int ProcessMemoryItem_free(ProcessMemoryItem* pmem);
void ProcessMemoryItem_init(ProcessMemoryItem* pmem, int pid);
void ProcessMemoryItem_print(ProcessMemoryItem* pmem);