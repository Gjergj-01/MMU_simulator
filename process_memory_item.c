#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "process_memory_item.h"

ProcessMemoryItem* ProcessMemoryItem_alloc() {
    return (ProcessMemoryItem*) malloc(sizeof(ProcessMemoryItem));
}

ProcessMemoryItem* ProcessMemoryItem_byPid(ListHead* head, int pid) {
    ListItem* aux = head->first;
    while(aux) {
        ProcessMemoryItem* item = (ProcessMemoryItem*) aux;
        if (item->pid == pid) {
            return item;
        }
        aux = aux->next;
    }
    return 0;
}

int ProcessMemoryItem_free(ProcessMemoryItem* item) {
    free(item);
    return 0;
}

void ProcessMemoryItem_init(ProcessMemoryItem* item, int pid) {
    List_init(&item->frame_list);
    item->list.prev = item->list.next = 0;
    item->pid = pid;
    // void *memset(void *s, int c, size_t n);
    // the memset fills the first n bytes of the memory area pointed
    // by s with the constant c
    // we initialize the page table setting each page 
    // number to -1
    memset(item->pages, 0, sizeof(PageEntry)*NUM_PAGES);

    // we set the flags of each page to invalid
    //for (int i = 0; i < NUM_PAGES; i++) {
    //    item->pages[i].flags &= (~Valid); 
    //}
}

FrameItem* ProcessMemoryItem_findFrame(ListHead* head, uint32_t frame_num) {
    ListItem* aux = head->first;
    while (aux) {
        FrameItem* frame = (FrameItem*) aux;
        if (frame->frame_num == frame_num) {
            return frame;
        }
        aux = aux->next;
    }
    return 0;
}

void ProcessMemoryItem_addFrame(ListHead* head, ListItem* prev, FrameItem* frame) {
    List_insert(head, prev, (ListItem*) frame);
}

void print_ProcessMemoryItem(ProcessMemoryItem* item) {
    printf("Process with PID: %d\n", item->pid);

    ListItem* aux = item->frame_list.first;
    while (aux) {
        FrameItem* frame = (FrameItem*) aux;
        printf("FRAME_NUM: %d\n", frame->frame_num);
        aux = aux->next;
    }
    int k = -1;
    int index = 0;
    for (int i = 0; i < NUM_PAGES; i++) {
        if (item->pages[i].frame_number == 0 && item->pages[i].flags == Valid) {
            if (k == -1) {
                k = i;
                index = i-1;
            }
            index++;
        }
    }
    printf("Pages from index %d to index %d are mapped on disk\n", k, index);
    printf("index of first free page: ");
    index = 0;
    for (int i = 0; i < NUM_PAGES; i++) {
        if (item->pages[i].frame_number == 0 && item->pages[i].flags == 0) {
            index = i;
            break;
        }
    }
    printf("%d\n\n", index);

}