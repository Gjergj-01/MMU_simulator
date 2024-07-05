#include <assert.h>
#include <disk.h>

DiskMemory dm;

void DiskMemory_init() {
    List_init(&dm.diskFrame_list);
}

void DiskMemory_shutdown() {
    while (dm.diskFrame_list.first) {
        DiskItem* disk_frame = (DiskItem*) dm.diskFrame_list.first;
        List_detach(&dm.diskFrame_list.first, (ListItem*) disk_frame);
        free(disk_frame);
    }
}

DiskItem* DiskItem_alloc() {
    return (DiskItem*) malloc(sizeof(DiskItem));
}

DiskItem* add_DiskItem(int pid, uint32_t page_num) {
    // we check that the frame is not already present in the disk
    assert(!Find_DiskItem(pid, page_num) && "invalid frame");
    DiskItem* new_item = DiskItem_alloc();

    // we initialize the new item
    new_item->list.prev = new_item->list.next = 0;
    new_item->page_num = page_num;
    new_item->pid = pid;

    // now we can insert the new item in the list
    List_insert(&dm.diskFrame_list, &dm.diskFrame_list.last, (ListItem*) new_item);
}

// Given the pid and the page_number finds the corresponding page

DiskItem* Find_DiskItem(int pid, uint32_t page_num) {
    ListItem* aux = (ListItem*) dm.diskFrame_list.first;

    while (aux) {
        DiskItem* item = (DiskItem*) aux;
        if (item->page_num == page_num && item->pid == pid) {
            return item;
        }
        aux = aux->next;
    }

    return 0;
}