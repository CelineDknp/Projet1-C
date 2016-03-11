#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct block_header {
	unsigned int size : 29, zero : 2, alloc : 1;
} header;

size_t memsize;//Taille memoire a recuperer en ligne de commande
header * base_heap;//La ou on a commence � stocker notre memoire
header * top_heap;
size_t memloc;//La memoire allouee actuelle
header *last;//Le dernier header encode

void* mymalloc(size_t size) {
	if (base_heap == NULL) {//Si base_heap est null, on est au premier appel, initialisons
		base_heap = sbrk(0);
		top_heap = sbrk(memsize);
		last=base_heap;
	}

	if (size == 0)//Si la taille demandee vaut 0, renvoyons NULL
		return NULL;
	size = size + (sizeof(size_t) - 1)/2 & ~(sizeof(size_t) - 1)/2; //Calcule la taille pour l'aligner sur 32 bits.
	if (memloc < size)//Si y'a plus de place, on renvoie NULL
		return NULL;
	header * temp = base_heap;//On part du d�but de la pile
	while (temp->alloc != 0 || temp->size < size) {
		if(memsize==memloc){//On est � la premi�re iteration, creons le header
			header * first = base_heap;
			first->size = size;
			first->alloc = 1;
			memloc-=size;
			return first+4;
		}
		if(temp>=top_heap)//On est alles trop loin, impossible de trouver un espace memoire satisfaisant Probleme sous Mac ?
			return NULL;
		if(temp>=last){//On est au dernier header et on a pas trouve de place
			header * next = temp+4+temp->size;//Cr�ons un nouveau header
			next->size=size; //Et allouons-lui la m�moire
			next->alloc=1;
			last=next; //Mettre � jour la fin des headers
			memloc-=size;
			return next+4;
		}
		temp = temp + 4 + temp->size;//On avance
	}
	temp->size = size;//Si on est sorti de la boucle, c'est qu'on a trouve une place, allouons-la
	temp->alloc = 1;
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
