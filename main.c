#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "malloc.h"

extern size_t memsize;
extern size_t memloc;
time_t start, stop;

int main(int argc, char const *argv[]) {
	memsize = atoll(argv[1]);
	memloc = memsize;
	printf("Début des tests de performance\n");
	time(&start);
	printf("Taille allouable : %zu\n", memsize);
	size_t temp_size;
	size_t all_size;
	void * last_pointer;
	do {
		temp_size = rand() % 10 + 1;
		size_t aligned_size = temp_size - (temp_size % 4);
		if (aligned_size < temp_size) {
			temp_size = aligned_size + 4;
		} else {
			temp_size = aligned_size;
		}
		all_size = all_size + temp_size + sizeof(header);
		if (all_size+temp_size >= memsize) {
			printf("Fin des allocations\n");
			break;
		}
		void * ptr = mymalloc(temp_size);
		size_t new_rand = rand() % 10 +1;
		if (new_rand < 5) {
			myfree(last_pointer);
		}
		last_pointer = ptr;
		printf("taille alouée : %zu, taille restante : %zu\n", all_size, memsize-all_size);
		printf("taille logique restante : %zu\n", memloc);
	} while(all_size < memsize);
	time(&stop);
	double time_of_test = stop-start;
	printf("Temps du test : %d\n", (int) time_of_test);
	return 0;
}