#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

typedef struct block_header {
	unsigned int size : 29, zero : 2, alloc : 1;
} header;

size_t memsize;//Taille memoire a recuperer en ligne de commande
header * base_heap;//La ou on a commence a stocker notre memoire
header * top_heap;//Le maximum de la heap (probleme sous mac, inutilise pour l'instant
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
   printf("Avant while : taille=%i et alloc=%i\n", temp->size, temp->alloc);

  while (temp->alloc != 0 || temp->size < size) {
    if(memsize==memloc){//On est a la premiere iteration, creons le header
      printf("Premier !\n");
      header * first = base_heap;
      first->size = size;
      first->alloc = 1;
      memloc-=size+sizeof(header);//On supprime ce qu'on vient d'allouer de ce qu'il reste de m�more
      return first+sizeof(header);
    }
    //if(temp>=sbrk(0))//On est alles trop loin, impossible de trouver un espace memoire satisfaisant Pose toujours probleme
	//return NULL;
    if(temp>=last){//On est au dernier header et on a pas trouve de place
      printf("Fin des headers, creation d'un nouveau\n");
      header * next = temp+sizeof(header)+temp->size;//Creons un nouveau header
      next->size=size; //Et allouons-lui la memoire
      next->alloc=1;
      last=next; //Mettre a jour la fin des headers
      memloc-=size+sizeof(header);
      return next+sizeof(header);
    }
    temp = temp + sizeof(header) + temp->size;//On avance
     printf("Tour de while : taille=%i et alloc=%i\n", temp->size, temp->alloc);
  }

  size_t beforeSplit=temp->size;
  printf("Before : %zu, asked : %zu\n", beforeSplit, size);
  if((size*2+sizeof(header))<= beforeSplit){//Si l'espace disponible est au moins deux fois plus grand que celui dont on a besoin
    printf("Bloc split !\n");
    temp->size = size; //On alloue
    temp->alloc = 1;
    memloc -= size+sizeof(header);
    header* split = temp+sizeof(header)+temp->size;//On split le bloc en deux en cr�ant un nouvel header.
    split->size= beforeSplit-sizeof(header)-size;
    printf("Adresse nouveau header : %p, taille : %i\n", split, split->size);
    split->alloc=0;
    if(temp==last)//Si on etait sur le dernier, update
      last=split;
    return temp+sizeof(header);
  }
  else{
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
    printf("Initialisation de la heap\n");
    base_heap = sbrk(0);
    sbrk(memsize);
    last=base_heap;
  }

  if (size == 0)//Si la taille demand�e vaut 0, renvoyons NULL
    return NULL;

  size_t aligned_size = size - (size % 4);
  if (aligned_size < size) {
    size = aligned_size + 4;
  } else {
	size = aligned_size;
  } // Version pas plateforme dépendant et sans divisions
  printf("Taille demandee : %zu\n", size);

  if (memloc < size)//Si y'a plus de place, on renvoie NULL
    return NULL;

  return bestAlloc(size);
}

/**
 *@pre :  -
 *@post : Meme chose que mymalloc, mais initialise toute la memoire a 0
 */
void* mycalloc(size_t size) {
  int* ptr = (int *) mymalloc(size);
  size_t aligned_size = size - (size % 4);
  if (aligned_size < size) {
    size = aligned_size + 4;
  } else {
	size = aligned_size;
  } // Version pas plateforme dépendant et sans divisions
  int i;
  for (i = 0; i < size; i++) {
    *(ptr+i) = 0;
  }
  return (void *) ptr;
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
      printf("Premier if desalloc !\n");
      header* next=temp+sizeof(header)+temp->size;
      if(next->alloc==0 && next->size !=0){//Si on trouve deux blocs desalloues de suite
	temp->size+=next->size+sizeof(header); //On ecrase le deuxieme avec le premier
	printf("Bloc ecrase ! Adresse : %p Taille : %i\n", temp, temp->size);
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
  printf("Premier malloc de 30 :\n");
  char * memOne = mymalloc(30);
  printf("Free du premier malloc\n");
  myfree(memOne);
  printf("Deuxieme malloc de 5 :\n");
  char * memTwo = mymalloc(5);
  printf("Troisieme malloc de 1 :\n");
  char * memThree = mymalloc(1);
  printf("Free du troisième malloc :\n");
  myfree(memThree);
  printf("Quatrieme malloc de 1 :\n");
  char * memFour = mymalloc(1);
  memFour[0]='h';
  printf("Cinquieme malloc de 1 :\n");
  char * memFive = mymalloc(1);
  printf("Fin des appels\n");
  printf("Pointeur 1 = %p\n", memOne);
  printf("Pointeur 2 = %p\n", memTwo);
  printf("Pointeur 3 = %p\n", memThree);
  printf("Pointeur 4 = %p\n", memFour);
  printf("Pointeur 5 = %p\n", memFive);
  return 0;
}
