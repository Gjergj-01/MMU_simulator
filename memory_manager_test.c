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

    for (int i = 0; i < 500; i++) {
        MMU_writeByte(p1, i, 'a');
    }

    //printf("FREE FRAMES: \n");
    //print_PhysicalMemory();

    int free_frames = Memory_freePages();
    printf("MEMORY_FREE_FRAMES: %d\n", free_frames);

    // create second process
    ProcessMemoryItem* p2 = Memory_addProcessItem(1);
    assign_pages(p2, 30, Valid);
    //print_ProcessMemoryItem(p2);

    // crete third process 
    ProcessMemoryItem* p3 = Memory_addProcessItem(2);
    assign_pages(p3, 5, Valid);

    //assign_pages(p1, 5, Valid);
    assign_pages(p2, 8, Valid);
    //print_ProcessMemoryItem(p2);

    // now we simulate some work
    MMU_writeByte(p2, 32, 'b');
    MMU_readByte(p3, 1);
    printf("OK\n");
    MMU_readByte(p3, 2);
    MMU_writeByte(p3, 3, 'c');
    // for (int i = 0; i < 5; i++) {
    //    simulate_work(p2, i, (Valid + Unswappable));
    // }

    //ProcessMemoryItem* p4 = Memory_addProcessItem(3);
    //for (int i = 0; i < 5; i++) {
    //    simulate_work(p4, i, read_bit);
    //}
    //printf("PROCESS 4:\n");
    //print_ProcessMemoryItem(p4);

    //print_PhysicalMemory();
    printf("DISK MEMORY: \n");
    print_DiskMemory();
    return 0;
}