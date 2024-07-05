#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <memory_manager.h>

GlobalMemoryLayout m;

//FrameItem physical_memory[]

/*
    At the beginning the physical memory is all free and 
    the process list contains no processes. So:
    1. we fill the pages with pid -1 (invalid)
    2. we initialize all the buffer representing the 
        physical memory available
*/
void Memory_init() {
    for (int i = 0; i < NUM_PAGES; i++) {
        m.frame_to_pid[i] = -1;
    }

}