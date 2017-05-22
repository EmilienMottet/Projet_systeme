#ifndef __PHY_MEM_ALLOC_H__
#define __PHY_MEM_ALLOC_H__

#include "../shared/malloc.h"
#include "queue.h"



#define K_BASE_VIRT_ADR 0x40000000 // adresse virtuelle en 1GiB
#define K_4KiB 0x1000


typedef struct page_vide {
  //cf queue.h
	link liens;
  int priorite;

  void* adr;
} page_vide;

typedef enum PERMISSION {
  READ,
  WRITE
} PERMISSION;

typedef struct adresse {
	//cf queue.h
	link liens;
  int priorite;

	void* adresse_phy;
	void* adresse_virt;
	PERMISSION perm;
} adresse;

typedef struct page_allouee {
  //cf queue.h
	link liens;
  int priorite;

  PERMISSION perm;
  int pid;
  link adresses;
} page_allouee;

void create_list_pages_vides();
void desalloue_pages_process(int pid);
void alloue_pages_process(int pid, int nb_pages, PERMISSION perm);
int get_nb_pages(int pid);
void* get_adr_phy(int pid, int no_page);
void* get_adr_virt(int pid, int no_page);
adresse* get_adr(int pid, int no_page);


#endif
