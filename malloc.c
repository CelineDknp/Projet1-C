#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct block_header {
	unsigned int size : 29, zero : 2, alloc : 1;
} header;

size_t memsize;
header * base_heap;
size_t memloc;

void* mymalloc(size_t size) {
	if (base_heap == NULL) {
		base_heap = sbrk(0);
		sbrk(memsize);
		header * base = base_heap;
		base->size = 4;
		base->alloc = 0;
	}
	if (size == 0)
		return NULL;
	size = size + (sizeof(size_t) - 1)/2 & ~(sizeof(size_t) - 1)/2;
	//alligne sur 32 bits. sizeof size_t ?
	if (memloc < size)
		return NULL;
	header * temp = base_heap;
	while (temp->alloc != 0 && temp->size < size) {
		printf("test1\n");
		temp = temp + 4 + temp->size;
	}
	temp->size = size;
	temp->alloc = 1;
	header * next = temp+4+size;
	next->size = 4;
	next->alloc = 0;
	memloc -= size;
	return temp+4;
}

void* mycalloc(size_t size) {
	int* ptr = (int *) mymalloc(size);
	size = size + (sizeof(size_t) - 1)/2 & ~(sizeof(size_t) - 1)/2;
	int i;
	for (i = 0; i < size; i++) {
		*(ptr+i) = 0;
	}
	return (void *) ptr;
}

void myfree(void* ptr) {
	header * head = (header *) ptr-4;
	head->alloc = 0;
	memloc += head->size;
}

int main(int argc, char const *argv[]) {
	memsize = atoi(argv[1]);
	memloc = memsize;
	char * memOne = mymalloc(5);
 	myfree(memOne);
 	char * memTwo = mymalloc(5);
 	char * memThree = mymalloc(1);
 	myfree(memThree);
 	char * memFour = mymalloc(1);
 	memFour[0]='h';
 	char * memFive = mymalloc(1);
 	printf("Fin des appels\n");
 	printf("Pointeur 1 = %p\n", memOne);
 	printf("Pointeur 2 = %p\n", memTwo);
 	printf("Pointeur 3 = %p\n", memThree);
 	printf("Pointeur 4 = %p\n", memFour);
 	printf("Pointeur 5 = %p\n", memFive);
	return 0;
}
