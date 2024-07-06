#pragma once

#include <stdint.h>
#include "linked_list.h"
#include "frame_item.h"

/*
    Here we create two records. The first one stores the informations
    about the process allocating it so we need:
        1. the pid
        2. the page number, this way we will be able to retrieve the 
           page corresponding to that frame in the paging table
    
    The second recored simply stores a list keeping track of each 
    DiskItem allocated
*/


typedef struct DiskMemory {
    ListHead diskFrame_list;
} DiskMemory;

void DiskMemory_init();
void DiskMemory_shutdown();

//FrameItem* add_FrameItem(int pid, uint32_t page_num);
void add_FrameDiskItem(FrameItem* item);
FrameItem* Find_FrameDiskItem(int pid, uint32_t page_num);
FrameItem* remove_FrameDiskItem(FrameItem* disk_item);