#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct block_header {
	unsigned int size : 29, zero : 2, alloc : 1;
} header;

size_t memsize;//Taille memoire a recuperer en ligne de commande
header * base_heap;//La ou on a commence à stocker notre memoire
header * top_heap;
size_t memloc;//La memoire allouee actuelle
header *last;//Le dernier header encode

/**
* - Fonction d'aide a mymalloc -
*@pre :  Prend en argument une taille a allouer dans la memoire
*@post : Retourne le premier pointeur permettant de contenir cette taille,
*        separe les blocs s'ils sont trop grands
*/
void* bestAlloc(size_t size){
	header * temp = base_heap;//On part du debut de la pile
	while (temp->alloc != 0 || temp->size < size) {
		if (memsize==memloc) {//On est a la premiere iteration, creons le header
			header * first = base_heap;
			first->size = size;
			first->alloc = 1;
			memloc-=size+sizeof(header);//On supprime ce qu'on vient d'allouer de ce qu'il reste de mémore
			return first+sizeof(header);
		}
		//if(temp>=sbrk(0))//On est alles trop loin, impossible de trouver un espace memoire satisfaisant Pose toujours problème
		//return NULL;
		if (temp>=last) {//On est au dernier header et on a pas trouve de place
			header * next = temp+sizeof(header)+temp->size;//Créons un nouveau header
			next->size=size; //Et allouons-lui la mémoire
			next->alloc=1;
			last=next; //Mettre a jour la fin des headers
			memloc-=size+sizeof(header);
			return next+sizeof(header);
		}
		temp = temp + sizeof(header) + temp->size;//On avance
	}
	size_t beforeSplit=temp->size;
	if ((size*2+sizeof(header)) <= beforeSplit) {//Si l'espace disponible est au moins deux fois plus grand que celui dont on a besoin
		temp->size = size; //On alloue
		temp->alloc = 1;
		memloc -= size+sizeof(header);
		header* split = temp+sizeof(header)+temp->size;//On split le bloc en deux en créant un nouvel header.
		split->size= beforeSplit-sizeof(header)-size;
		split->alloc=0;
		if(temp==last)//Si on était sur le dernier, update
			last=split;
		return temp+sizeof(header);
	} else {
		temp->alloc=1;
		memloc -= size+sizeof(header);
		return temp+sizeof(header);
	}
}

/**
*@pre :  -
*@post : Renvoie un pointeur vers un espace memoire de la taille demandée
*/
void* mymalloc(size_t size) {
	if (base_heap == NULL) {//Si base_heap est null, on est au premier appel, initialisons
		base_heap = sbrk(0);
		top_heap = sbrk(memsize);
		last=base_heap;
	}
	if (size == 0)//Si la taille demandee vaut 0, renvoyons NULL
		return NULL;
	size_t aligned_size = size - (size % 4);
	if (aligned_size < size) {
		size = aligned_size + 4;
	} else {
		size = aligned_size;
	} // Version pas plateforme dépendant et sans divisions

	if (memloc < size)//Si y'a plus de place, on renvoie NULL
		return NULL;
	return bestAlloc(size);//Cherchons la meilleure allocation
}

/**
*@pre :  -
*@post : Meme chose que mymalloc, mais initialise toute la memoire a 0
*/
void* mycalloc(size_t size) {
	char* start = (char *) mymalloc(size);
	//size = size + (sizeof(size_t) - 1)/2 & ~(sizeof(size_t) - 1)/2;
	size_t aligned_size = size - (size % 4);
	if (aligned_size < size) {
		size = aligned_size + 4;
	} else {
		size = aligned_size;
	} // Version pas plateforme dépendant et sans divisions
	char* ptr = start;
	char* end = ptr+size;
	for (; ptr < end; ptr++) {
		*ptr = 0;
	}
	return (void *) start;
}

/**
* - Fonction d'aide a myfree -
*@pre :  -
*@post : La memoire est defragmentee au maximum
*/
void defragMemory(){
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
*@post : Le pointeur ptr est desalloue,
*        la memoire est defragmentee au maximim
*/
void myfree(void* ptr) {
	if(ptr==NULL){
		fprintf(stderr, "Il n'y a rien dans le pointeur que vous voulez liberer !\n");
		return;
	}
	header * head = (header *) ptr-sizeof(header);
	if(head->alloc==0 && head->size==0) {
		fprintf(stderr, "Je ne sais pas quel pointeur vous voulez liberer, mais il n'a pas ete cree par mymalloc et vous ne devez donc pas utiliser myfree !\n");
		return;
	}
	head->alloc = 0;
	memloc += head->size;
	defragMemory();
}
