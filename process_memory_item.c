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
    item->list.prev = item->list.next = 0;
    item->pid = pid;
    // void *memset(void *s, int c, size_t n);
    // the memset fills the first n bytes of the memory area pointed
    // by s with the constant c
    // we initialize the page table setting each page 
    // number to 0
    memset(item->pages, 0, sizeof(PageEntry)*NUM_PAGES);

    // we set the flags of each page to invalid
    for (int i = 0; i < NUM_PAGES; i++) {
        item->pages[i].flags &= (~Valid); // THIS MAY CREATE PROBLEMS
        item->pages[i].frame_number = i;
    }
}

void ProcessMemoryItem_print(ProcessMemoryItem* item) {
    printf("ProcessMemory with pid: %d\n", item->pid);
}