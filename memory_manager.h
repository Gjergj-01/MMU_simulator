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
    is assigned each frame
*/

typedef struct GlobalMemoryLayout {
    int frame_to_pid[NUM_FRAMES];
    ListHead frame_list;        // physical memory
    ListHead process_list;
} GlobalMemoryLayout;

//extern GlobalMemoryLayout m;

void Memory_init();
void Memory_shutdown();
ProcessMemoryItem* Memory_byPid(int pid);
ProcessMemoryItem* Memory_addProcessItem(int pid);
uint32_t Memory_freePages();
void Memory_destroyProcessMemoryItem(ProcessMemoryItem* item);
void print_PhysicalMemory();

FrameItem* Find_frame(int pid, uint32_t frame_num);
FrameItem* remove_Frame(FrameItem* item);
FrameItem* add_Frame(FrameItem* item);

//void add_Pages(ProcessMemoryItem* pmem, int num_pages, uint32_t flags);

void assign_pages(ProcessMemoryItem* pmem, int num_pages, uint32_t flags);
void simulate_work(ProcessMemoryItem* pmem, int pos, uint32_t flags);

void MMU_writeByte(ProcessMemoryItem* pmem, int pos, char c);
char* MMU_readByte(ProcessMemoryItem* pmem, int pos);
void MMU_exception(ProcessMemoryItem* pmem, int pos);
void second_chance(ProcessMemoryItem* pmem, int pos);