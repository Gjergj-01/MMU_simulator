#pragma once 
#include "frame_item.h"

typedef struct ProcessMemoryItem {
    ListItem list;
    int pid;
    PageEntry pages[NUM_PAGES];
    ListHead frame_list;
    
} ProcessMemoryItem;

ProcessMemoryItem* ProcessMemoryItem_alloc();
ProcessMemoryItem* ProcessMemoryItem_byPid(ListHead* head, int pid);
FrameItem* ProcessMemoryItem_findFrame(ListHead* head, uint32_t frame_num);
void ProcessMemoryItem_addFrame(ListHead* head, ListItem* prev, FrameItem* frame);

int ProcessMemoryItem_free(ProcessMemoryItem* pmem);
void ProcessMemoryItem_init(ProcessMemoryItem* pmem, int pid);
void print_ProcessMemoryItem(ProcessMemoryItem* pmem);