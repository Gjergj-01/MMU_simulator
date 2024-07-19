#include <stdio.h>
#include "memory_manager.h"
#include "disk.h"

int main(int argc, char** argv) {
    // we initialize both memories (RAM and DISK)
    Memory_init();
    DiskMemory_init();

    printf("\nMEMORY INITIALIZED!\n\n");

    // create first process
    ProcessMemoryItem* p1 = Memory_addProcessItem(0);
    assign_pages(p1, 10, Valid);
    assign_pages(p1, 3, (Valid+Unswappable));
    assign_pages(p1, 300, Valid);
    assign_pages(p1, 10, (Valid+Unswappable));
    assign_pages(p1, 700, Valid);

    for (int i = 0; i < 1023; i++) {
        MMU_writeByte(p1, i, 'a');
    }


    int free_frames = Memory_freePages();
    printf("MEMORY_FREE_FRAMES: %d\n", free_frames);

    // create second process
    ProcessMemoryItem* p2 = Memory_addProcessItem(1);
    assign_pages(p2, 30, Valid);
    //print_ProcessMemoryItem(p2);

    for (int i = 0; i < 10; i++) {
        MMU_readByte(p2, i);
    }

    free_frames = Memory_freePages();
    printf("MEMORY_FREE_FRAMES: %d\n", free_frames);

    MMU_writeByte(p2, 28, 'a');
    MMU_writeByte(p2, 29, 'a');
    MMU_writeByte(p1, 500, 'b');
    MMU_readByte(p2, 28);

    // crete third process 
    ProcessMemoryItem* p3 = Memory_addProcessItem(2);
    assign_pages(p3, 5, Valid);

    assign_pages(p1, 5, Valid);
    assign_pages(p2, 8, Valid);
    // print_ProcessMemoryItem(p2);

    // now we simulate some other work
    MMU_writeByte(p2, 32, 'b');
    MMU_readByte(p3, 1);
    MMU_readByte(p3, 2);
    MMU_writeByte(p3, 3, 'c');

    ProcessMemoryItem* p4 = Memory_addProcessItem(3);
    assign_pages(p4, 10, (Valid + Unswappable));

    for (int i = 0; i < 8; i++) {
        if (i % 2 == 0) {
            MMU_readByte(p4, i);
        }
        else {
            MMU_writeByte(p4, i, '4');
        }
    }

    // THIS SHOULD GENERATE A SEGMENTATION FAULT
    //MMU_writeByte(p4, 15, 'c');

    // Here we print the DiskMemory status
    printf("DISK MEMORY: \n");
    print_DiskMemory();

    // WE SHUTDOWN THE MEMORY
    printf("SHUTTING MEMORY DOWN!\n");
    Memory_shutdown();
    DiskMemory_shutdown();

    return 0;
}