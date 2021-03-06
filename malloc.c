#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "malloc.h"

size_t memsize = 0; //Taille mémoire à récupérer en ligne de commande
header * base_heap = NULL; //Là où on a commencé à stocker notre memoire
size_t memloc = 0; //La mémoire allouée actuelle
header * last = NULL; //Le dernier header encodé

/**
* - Fonction d'aide a mymalloc -
*@pre :  Prend en argument une taille à allouer dans la mémoire
*@post : Retourne le premier pointeur permettant de contenir cette taille,
*        sépare les blocs s'ils sont trop grands
*/
void* bestAlloc(size_t size) {
	header * temp = base_heap; //On part du debut de la pile
	while (temp->alloc != 0 || temp->size < size) {
		if (memsize == memloc) { //On est a la première iteration, créons le header
			header * first = base_heap;
			first->size = size;
			first->alloc = 1;
			memloc -= size + sizeof(header); //On supprime ce qu'on vient d'allouer de ce qu'il reste de mémoire
			return first + sizeof(header);
		}
		if (temp >= last) { //On est au dernier header et on a pas trouvé de place
			if (last + size > sbrk(0)) //Si y'a plus de place, on renvoie NULL
				return NULL;
			header * next = temp + sizeof(header) + temp->size;//Créons un nouveau header
			next->size = size; //Et allouons-lui la mémoire
			next->alloc = 1;
			last = next; //Mettre a jour la fin des headers
			memloc -= size + sizeof(header);
			return next + sizeof(header);
		}
		temp = temp + sizeof(header) + temp->size; //On avance
	}
	size_t beforeSplit = temp->size;
	if ((size * 2 + sizeof(header)) <= beforeSplit) { //Si l'espace disponible est au moins deux fois plus grand que celui dont on a besoin
		temp->size = size; //On alloue
		temp->alloc = 1;
		memloc -= size + sizeof(header);
		header* split = temp + sizeof(header) + temp->size; //On split le bloc en deux en créant un nouvel header.
		split->size = beforeSplit - sizeof(header) - size;
		split->alloc = 0;
		if (temp >= last) //Si on était sur le dernier, update
			last = split;
		return temp + sizeof(header);
	} else {
		temp->alloc = 1;
		memloc -= size + sizeof(header);
		return temp + sizeof(header);
	}
}

/**
*@pre :  -
*@post : Renvoie un pointeur vers un espace memoire de la taille demandée
*/
void* mymalloc(size_t size) {
	if (base_heap == NULL) { //Si base_heap est null, on est au premier appel, initialisons
		base_heap = sbrk(0);
		void * err = sbrk(memsize);
		if (err == -1 && errno == ENOMEM) {
			 fprintf(stderr, "Pas assez de mémoire !\n");
			 exit(EXIT_FAILURE);
		}
		last = base_heap;
	}
	if (size == 0) //Si la taille demandée vaut 0, renvoyons NULL
		return NULL;
	size_t aligned_size = size - (size % 4);
	if (aligned_size < size) {
		size = aligned_size + 4;
	} else {
		size = aligned_size;
	} //Alignement sur 4 bytes
	return bestAlloc(size); //Cherchons la meilleure allocation
}

/**
*@pre :  -
*@post : Même chose que mymalloc, mais initialise toute la memoire à 0
*/
void* mycalloc(size_t size) {
	char * start = (char *) mymalloc(size);
	size_t aligned_size = size - (size % 4);
	if (aligned_size < size) {
		size = aligned_size + 4;
	} else {
		size = aligned_size;
	} //Alignement sur 4 bytes
	char * ptr = start;
	char * end = ptr + size;
	for (; ptr < end; ptr++) {
		*ptr = 0;
	}
	return (void *) start;
}

/**
* - Fonction d'aide a myfree -
*@pre :  -
*@post : La mémoire est defragmentée au maximum
*/
void defragMemory() {
	header* temp=base_heap;
	while(temp<=last){
		if(temp->alloc == 0){
			header* next=temp+sizeof(header)+temp->size;
			if(next->alloc==0 && next->size !=0){//Si on trouve deux blocs desalloues de suite
				temp->size+=next->size+sizeof(header); //On ecrase le deuxieme avec le premier
			}
		}
		temp+=temp->size+sizeof(header);
	}
}

/**
*@pre :  -
*@post : Le pointeur ptr est desalloué,
*        la mémoire est defragmentée au maximim
*/
void myfree(void* ptr) {
	if(ptr == NULL){
		fprintf(stderr, "Il n'y a rien dans le pointeur que vous voulez libérer !\n");
		return;
	}
	header * head = (header *) ptr - sizeof(header);
	if (head->alloc == 0 && head->size != 0) {
		fprintf(stderr, "Ce pointeur a déjà été libéré, inutile de le free !\n");
		return;
	}
	if(head->alloc == 0 && head->size == 0) {
		fprintf(stderr, "Je ne sais pas quel pointeur vous voulez liberer, mais il n'a pas été créé par mymalloc et vous ne devez donc pas utiliser myfree !\n");
		return;
	}
	head->alloc = 0;
	memloc += head->size;
	defragMemory();
}
