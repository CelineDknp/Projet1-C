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

/**
 * - Fonction d'aide a mymalloc -
 *@pre :  Prend en argument une taille a allouer dans la memoire
 *@post : Retourne le premier pointeur permettant de contenir cette taille,
 *        separe les blocs s'ils sont trop grands
 */
void* bestAlloc(size_t size){
	header * temp = base_heap;//On part du debut de la pile
	
	while (temp->alloc != 0 || temp->size < size) {
		if(memsize==memloc){//On est a la premiere iteration, creons le header
			header * first = base_heap;
			first->size = size;
			first->alloc = 1;
			memloc-=size+sizeof(header);//On supprime ce qu'on vient d'allouer de ce qu'il reste de m�more
			return first+sizeof(header);
		}
		//if(temp>=sbrk(0))//On est alles trop loin, impossible de trouver un espace memoire satisfaisant Pose toujours probl�me
		//return NULL;
		if(temp>=last){//On est au dernier header et on a pas trouve de place
			header * next = temp+sizeof(header)+temp->size;//Cr�ons un nouveau header
			next->size=size; //Et allouons-lui la m�moire
			next->alloc=1;
			last=next; //Mettre a jour la fin des headers
			memloc-=size+sizeof(header);
			return next+sizeof(header);
		}
		temp = temp + sizeof(header) + temp->size;//On avance
	}

	size_t beforeSplit=temp->size;
	if ((size*2+sizeof(header))<= beforeSplit) {//Si l'espace disponible est au moins deux fois plus grand que celui dont on a besoin
		temp->size = size; //On alloue
		temp->alloc = 1;
		memloc -= size+sizeof(header);
		header* split = temp+sizeof(header)+temp->size;//On split le bloc en deux en cr�ant un nouvel header.
		split->size= beforeSplit-sizeof(header)-size;
		split->alloc=0;
		if(temp==last)//Si on �tait sur le dernier, update
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
 *@post : Renvoie un pointeur vers un espace memoire de la taille demand�e
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
  header * head = (header *) ptr-sizeof(header);
  head->alloc = 0;
  memloc += head->size;
  defragMemory();
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
