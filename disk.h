#pragma once

#include <stdint.h>
#include "linked_list.h"

/*
    Here we create two records. The first one stores the informations
    about the process allocating it so we need:
        1. the pid
        2. the page number, this way we will be able to retrieve the 
           page corresponding to that frame in the paging table
    
    The second recored simply stores a list keeping track of each 
    DiskItem allocated
*/

typedef struct DiskItem {
    ListItem list;
    int pid;
    uint32_t page_num;
} DiskItem;

typedef struct DiskMemory {
    ListHead diskFrame_list;
} DiskMemory;

void DiskMemory_init();
void DiskMemory_shutdown();
DiskItem* DiskItem_alloc();
DiskItem* add_DiskItem(int pid, uint32_t page_num);
DiskItem* Find_DiskItem(int pid, uint32_t page_num);

