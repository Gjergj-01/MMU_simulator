CC = gcc
CCOPTS = --std=gnu99 -g -Wall
AR = ar 

BINS = mmu_test

OBJS = mmu.o 

HEADERS = constants.h mmu.h 

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

clean:
	rm -rf *.o *~ $(LIBS) $(BINS)