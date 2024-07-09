#pragma once

#include <stdint.h>
#include "linked_list.h"
#include "frame_item.h"

/*
    Here we create a record that stores the memory on disk.
    It simply stores a diskFrame_list that keeps track of 
    the FrameItems mapped on disk memory.
*/


typedef struct DiskMemory {
    ListHead diskFrame_list;
} DiskMemory;

void DiskMemory_init();
void DiskMemory_shutdown();
void print_DiskMemory();

void add_FrameDiskItem(FrameItem* item);
FrameItem* Find_FrameDiskItem(int pid, uint32_t page_num);
FrameItem* remove_FrameDiskItem(FrameItem* disk_item);