CC=gcc
CFLAGS=-g -ggdb3 -fPIC
MALLOC_VERSION=BF
WDIR=/home/ql143/ECE_650/Malloc/my_malloc

all: mymalloc_test

mymalloc_test: mymalloc_test.c
	$(CC)  $(CFLAGS) -I$(WDIR) -L$(WDIR) -D$(MALLOC_VERSION) -Wl,-rpath=$(WDIR) -o $@ mymalloc_test.c -lmymalloc -lrt

clean:
	rm -f *~ *.o mymalloc_test

clobber:
	rm -f *~ *.o
