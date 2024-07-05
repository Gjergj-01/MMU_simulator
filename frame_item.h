#pragma once
#include <stdint.h>
#include "linked_list.h"
#include "mmu.h"

// we create a record that stores the number of the frame and
// the pid of the process it is associted to
typedef struct FrameItem {
    ListItem list;
    int pid;
    uint32_t frame_num;
} FrameItem;

FrameItem* FrameItem_alloc();
int FrameItem_free(FrameItem* item);
void FrameItem_init(FrameItem* item, int pid, uint32_t frame_num);
