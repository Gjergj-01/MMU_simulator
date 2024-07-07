#include <stdio.h>
#include "memory_manager.h"
#include "disk.h"

int main(int argc, char** argv) {
    // we initialize both memories (RAM and DISK)
    Memory_init();
    DiskMemory_init();

    printf("MEMORY INITIALIZED!\n");

    // create first process
    ProcessMemoryItem* p1 = Memory_addProcessItem(0);
    assign_pages(p1, 1000, Valid);
    //print_PhysicalMemory();

    int free_frames = Memory_freePages();
    printf("MEMORY_FREE_FRAMES: %d\n", free_frames);

    // create second process
    ProcessMemoryItem* p2 = Memory_addProcessItem(1);
    assign_pages(p2, 30, Valid);
    print_ProcessMemoryItem(p2);

    // crete third process 
    ProcessMemoryItem* p3 = Memory_addProcessItem(2);
    assign_pages(p3, 5, Valid);

    assign_pages(p1, 5, Valid);
    assign_pages(p2, 8, Valid);
    print_ProcessMemoryItem(p2);

    //print_PhysicalMemory();
    //print_DiskMemory();
    return 0;
}