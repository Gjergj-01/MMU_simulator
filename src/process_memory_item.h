#pragma once 
#include "frame_item.h"

/*
    For each process we create a record that stores: the pid,
    his virtual memory, and a frame list that stores the frame 
    allocated in the physical memory to that process. 
    In concrete we use this record as the MMU.
*/

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
/*
    The function below initializes a ProcessMemoryItem. We set
    each PageEntry to 0 and then we set each flag to Invalid
*/
void ProcessMemoryItem_init(ProcessMemoryItem* pmem, int pid);
void print_ProcessMemoryItem(ProcessMemoryItem* pmem);