#include "page.h"
#include "process.h"

#define K_PD_MSK_BIT_P 0x00000001   // bit de presence
#define K_PD_MSK_BIT_R 0x00000002   // bit R/W   0: read only
#define K_PD_MSK_BIT_U 0x00000004   // bit User/superviser 0: superviser only
#define K_PD_MSK_BIT_W 0x00000008   // bit write-through 1:write-through caching enabled
#define K_PD_MSK_BIT_D 0x00000010   // bit cache disable   0: caching enable
#define K_PD_MSK_BIT_A 0x00000020   // bit acces bit 1: page was read or write
#define K_PD_MSK_BIT_S 0x00000080   // bit page size 0:4KiB 1:4MiB
#define K_PD_MSK_ADD   0xFFFFF000   // Page Table 4KB aligned address

#define K_PT_MSK_BIT_P 0x00000001   // bit de presence
#define K_PT_MSK_BIT_R 0x00000002   // bit R/W   0: read only
#define K_PT_MSK_BIT_U 0x00000004   // bit User/superviser 0: superviser only
#define K_PT_MSK_BIT_W 0x00000008   // bit write-through 1:write-through caching enabled
#define K_PT_MSK_BIT_C 0x00000010   // bit cache disable   0: caching enable
#define K_PT_MSK_BIT_A 0x00000020   // bit acces bit 1: page was read or write
#define K_PT_MSK_BIT_D 0x00000040   // bit dirty flag 1: page was write
#define K_PT_MSK_BIT_G 0x00000100   // bit global flag
#define K_PT_MSK_ADD   0xFFFFF000   // Physical Page 4KB aligned address

// Pb d'include a regler dans ce fichier et dans process

// UTILISER LES FONCTIONS GET POUR AVOIR LES BONS PARAMETRES
// BIT 0 page present
// BIT 1 Permission READ/WRITE

// A FAIRE: UTILISER LES FONCTIONS A LA CREATION DU PROCESSUS, DANS start();?

// Tout l'espace memoire d'un proscess a les memes permissions au meme moment? non

// Retourne true si le bit n est a 1, le bit de poids faible est le bit 0
// Retourne false si n<0 et n>32
bool is_bit_n_true(int n, uint32_t adr){

  uint32_t mask=0b1;

  if(n<0 || n>31){
    return false;
  }

  else{

    mask=mask<<n;

    if((adr & mask)!=0){
      return true;
    }

    else{
      return false;
    }
  }
}


// Set le bit n a 0 si boo est false, sinon set le bit n a 1
// Ne fais rien si n<0 et n>32
void set_bit_n(int n, bool boo, uint32_t * adr){

  uint32_t mask=0b1;

  if(n<0 || n>31){
    return;
  }

  if(boo==false){
    mask=mask<<n;
    mask=~mask;
    *adr=(*adr & mask);
    return;
  }

  else{
    mask=mask<<n;
    *adr|=mask;
    return;
  }
}


// Retourne l'entier represente par les bits n_fort..n_faible de l'adr
// Retourne 0, si n_fort-n_faible < 0 ou si n_fort > 31 ou si n_faible < 0
int get_n_bits(int n_fort, int n_faible, uint32_t adr){

  int num=0;
  int i;
  int cpt=0;

  if(n_fort-n_faible<0 || n_fort>31 || n_faible<0){
    return 0;
  }

  for(i=n_faible; !(i>n_fort); i++){
    if(is_bit_n_true(i, adr)==true){
      num=num+(0b1<<cpt);
    }
    cpt++;
  }
  return num;
}


// Mode READ/WRITE
// La page passé en paramètre sera la page table correspondant
void permision_ecriture(bool boo, uint32_t *page){
  set_bit_n(1, boo, page);
}


// Retourne vrai si le process a la permission WRITE
bool a_permission_ecriture(uint32_t page){
  return is_bit_n_true(1, page);
}


// Met le processus en mode superuser
void put_superuser(bool boo, uint32_t * page){
  set_bit_n(2 ,boo, page);
}


// Regarde si le processus est en mode superuser
bool is_superuser(uint32_t page){
  return is_bit_n_true(2, page);
}


//Creation de la page directory alignes sur 12 bits
// Retourne le pointeur vers le debut du tableau
// Fonction a utiliser a la creation du processus
// BIT Size (7) deja a 0
// ATTENTION PB KILL, et pid peut etre inutile voir comment faire un kill
uint32_t * create_page_directory(){
  uint32_t * page_directory;
  page_directory=memalign(K_4KiB, K_4KiB);

  if(page_directory==NULL){
    return NULL;
  }
  // FIXME Jlc add begin
  uint32_t ii;
  uint32_t* pt=page_directory;
  extern uint32_t pgdir[];
  uint32_t* pt_pg=pgdir;
/* Fill page directory with the first 256MB of memory of kernel*/
  for (ii=0;ii<64;ii++,pt++,pt_pg++)
  {
	  *pt=*pt_pg;
  }
  for (ii=64;ii<(K_4KiB/sizeof(uint32_t));ii++,pt++)
  {
	  *pt=0;
  }
  // FIXME Jlc add end
  return page_directory;
}


// Creation de la page table alignes sur 12 bits
// Page table creee? 0 dans le bit de poids faible dans la case du page directory correspondant, 1 sinon
// Met a jour l'avant dernier bit de poids faible dans la case du page directory
// Ne retourne rien mais met l'adresse du debut de la page table dans la case du page directory
// ATTENTION PB KILL, et pid peut etre inutile, voir comment faire un kill
int create_page_table(uint32_t * case_page_directory){

  uint32_t * page_table;
  page_table=memalign(K_4KiB, K_4KiB);

  if(page_table==NULL){
	return -1;
  }

  uint32_t ii;
  uint32_t* pt=page_table;
  for (ii=0;ii<(K_4KiB/sizeof(uint32_t));ii++,pt++)
  {
	  *pt=0;
  }

  *case_page_directory=(((uint32_t)page_table)&K_PD_MSK_ADD)|K_PD_MSK_BIT_U;
  set_bit_n(0, true, case_page_directory);
  return 0;
}


// Met l'adresse reelle du processus en fonction de l'adresse virtuelle
// Met le BIT present (0) a 1
// REECRIRE LA FONCTION AVEC LES FONCTIONS GET, OU UTILISER LES FONCTIONS GET AVANT ET METTRE LES BONS PARAMETRES QUAND ON APPELLE LA FONCTION
int put_address_reel(void* adr_reel, void* adr_virt, PERMISSION perm, uint32_t *page_directory){

  int offset;
  uint32_t * case_page_directory;
  uint32_t * case_page_table;
  uint32_t * page_table;

  offset=get_n_bits(31, 22, (uint32_t)adr_virt);
  assert(offset>63);
  case_page_directory=page_directory+offset;
  
  // page_table existe ?
  if (((*case_page_directory) & K_PD_MSK_BIT_P)==0)
  { //Non
	  if (create_page_table(case_page_directory)!=0)
	  {
		  return -1;
	  }
  }
  if (perm == WRITE) *case_page_directory|=K_PD_MSK_BIT_R;
  *case_page_directory|=K_PT_MSK_BIT_U;
  page_table = (uint32_t*)(*case_page_directory & K_PD_MSK_ADD);


  offset=get_n_bits(21, 12, (uint32_t)adr_virt);
  case_page_table=page_table+offset;
  if (perm == READ)
  {
	  *case_page_table=(((uint32_t)adr_reel)&K_PT_MSK_ADD)|K_PT_MSK_BIT_U|K_PT_MSK_BIT_P;
  }
  else
  {
	  *case_page_table=(((uint32_t)adr_reel)&K_PT_MSK_ADD)|K_PT_MSK_BIT_U|K_PT_MSK_BIT_R|K_PT_MSK_BIT_P;
  }
  return 0;

}

// Retourne l'adresse reelle en fonction du processus et de l'adresse virtuelle
uint32_t get_address_reel(uint32_t adr_virt, uint32_t * page_directory){

  int offset;
  uint32_t * case_page_directory;
  uint32_t * case_page_table;

  offset=get_n_bits(31, 22, adr_virt);
  case_page_directory=page_directory+offset;

  offset=get_n_bits(21, 12, adr_virt);
  case_page_table=((uint32_t *)(*case_page_directory))+offset; //comment caster correctement?
  return * case_page_table; // quel est l'offset du programme? offset_virt=offset_reel
}


//Desalloue les pages allouees aux processus. Desalloue toutes les page table, et la page directory
void free_page(uint32_t *page_directory){

	int i;
	uint32_t* case_page_directory=page_directory;
	for(i=64;i<1024;i++,case_page_directory++) // pas de liberation du kernel
	{
		if (((*case_page_directory) & K_PD_MSK_BIT_P)!=0)
		{
			void* pt_page_table = (void*)(*case_page_directory & K_PD_MSK_ADD);
			free(pt_page_table);
		}
	}
	free(page_directory);
}


// Retourne l'adresse de debut du page_directory
uint32_t * get_page_directory(int pid){
  processus * p = table_des_processus+pid;
  return p->page_directory;
}


// Met l'adresse du page_directory du processus du pid dans le registre cr3
void put_cr3(int pid){

  uint32_t * page_directory;

  page_directory=get_page_directory(pid);

  __asm__("movl %0, %%cr3" : : "r" (page_directory)  );

  tss.cr3=(int)page_directory;

  if(is_superuser(* page_directory)==true){
    tss.cs=0x10;
    tss.ds=0x18;
    tss.ss=0x18;
    tss.es=0x18;
    tss.fs=0x18;
    tss.gs=0x18;
  }
  else{
    tss.cs=0x43;
    tss.ds=0x4b;
    tss.ss=0x4b;
    tss.es=0x4b;
    tss.fs=0x4b;
    tss.gs=0x4b;
  }

}

/* Questions:
 - un pointeur est sur 32 bits?  oui
 - une adresse est sur 32 bits?  oui
 - dépend de quoi?  de l'architecture
 - quand mettre quoi ou?  au debut
 - qui appelle la creation de page directory et page table?
 - l'espace memoire d'un processus ne va jamais changer? Une page table ne changera jamais d'adresse reel pointee? l'espace ne va jamais changer mais offset?

*/
