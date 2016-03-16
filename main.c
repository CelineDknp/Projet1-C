#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "malloc.h"

extern size_t memsize;
extern size_t memloc;

int main(int argc, char const *argv[]) {
	memsize = atoi(argv[1]);
	memloc = memsize;
	void* ptr = mymalloc(4);
	myfree(ptr);
	myfree(ptr);
	void* ptr2 = mymalloc(4);
	myfree(ptr2);
	return 0;
}
