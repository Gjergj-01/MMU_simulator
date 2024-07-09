CC = gcc
CCOPTS = --std=gnu99 -g -Wall
AR = ar 

BINS = mmu_test memory_manager_test

OBJS = disk.o frame_item.o linked_list.o memory_manager.o mmu.o process_memory_item.o 

HEADERS = constants.h disk.h frame_item.h linked_list.h memory_manager.h mmu.h process_memory_item.h 

LIBS = lib_memory.a 

%.o:	%.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@ $<

.phony: clean all

all:	$(BINS) $(LIBS)

lib_memory.a: $(OBJS) 
	$(AR) -rcs $@ $^
	$(RM) $(OBJS)

mmu_test: mmu_test.c mmu.c 
	$(CC) $(CCOPTS) -o $@ $^

memory_manager_test: memory_manager_test.c $(OBJS)
	$(CC) $(CCOPTS) -o $@ $^

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)