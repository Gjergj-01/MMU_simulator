#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "disk.h"

DiskMemory dm;

void DiskMemory_init() {
    List_init(&dm.diskFrame_list);
}

void DiskMemory_shutdown() {
    while (dm.diskFrame_list.first) {
        FrameItem* disk_frame = (FrameItem*) dm.diskFrame_list.first;
        List_detach(&dm.diskFrame_list, (ListItem*) disk_frame);
        free(disk_frame);
    }
}

void print_DiskMemory() {
    ListItem* aux = dm.diskFrame_list.first;
    while(aux) {
        FrameItem* frame = (FrameItem*) aux;
        printf("PID: %d, PAGE_INDEX: %d\n", frame->pid, frame->frame_num);
        aux = aux->next;
    }
}

FrameItem* DiskItem_alloc() {
    return (FrameItem*) malloc(sizeof(FrameItem));
}

// FrameItem* add_DiskItem(int pid, uint32_t page_num) {
//     // we check that the frame is not already present in the disk
//     assert(!Find_DiskItem(pid, page_num) && "invalid frame");
//     FrameItem* new_item = DiskItem_alloc();

//     // we initialize the new item
//     new_item->list.prev = new_item->list.next = 0;
//     new_item->page_num = page_num;
//     new_item->pid = pid;

//     // now we can insert the new item in the list
//     List_insert(&dm.diskFrame_list, &dm.diskFrame_list.last, (ListItem*) new_item);
// }

void add_FrameDiskItem(FrameItem* item) {
    // we check that the frame is not already present in the disk
    int pid = item->pid;
    //printf("pid: %d index: %d\n", pid, item->frame_num);
    uint32_t frame_num = item->frame_num;
    assert(!Find_FrameDiskItem(pid, frame_num) && "page alredy on disk memory");

    List_insert(&dm.diskFrame_list, dm.diskFrame_list.last, (ListItem*) item);
}

// Given the pid and the page_number finds the corresponding frame

FrameItem* Find_FrameDiskItem(int pid, uint32_t page_num) {
    ListItem* aux = (ListItem*) dm.diskFrame_list.first;

    while (aux) {
        FrameItem* item = (FrameItem*) aux;
        if (item->frame_num == page_num && item->pid == pid) {
            return item;
        }
        aux = aux->next;
    }

    return 0;
}

FrameItem* remove_FrameDiskItem(FrameItem* disk_item) {
    FrameItem* removed_frame = (FrameItem*) List_detach(&dm.diskFrame_list, (ListItem*) disk_item);
    return removed_frame;
}