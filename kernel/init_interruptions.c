
#include "init_interruptions.h"

void masque_IRQ(uint32_t num_IRQ/*entre 0 et 7*/, char masque){
	//démasquer l'IRQ
	//lire la valeur actuelle du masque sur le port de données 0x21
	//1 si l’IRQ est masquée, 0 si elle est autorisée
	uint8_t i=inb(0x21);
  printf("\nvaleur du IRQ = {");print_byte(i);printf("} (%d)\n",i);
  printf("\nmise a %d du bit %d\n",(int)masque,num_IRQ);
	if(masque)i|=  1<<num_IRQ; //mettre a 1 le num_IRQ ieme bit
	else 	    i&=~(1<<num_IRQ);//mettre a 0 le num_IRQ ieme bit
  outb(i,0x21);
  printf("\nnouvelle valeur du IRQ = {");print_byte(i);printf("} (%d)\n",inb(0x21));
}

void init_traitant_IT(int32_t num_IT, void (*traitant)(void)){

	//initialiser l'entrée 32 dans la table des vecteurs d'interruptions

	//Chaque entrée de l’IDT occupe 2 mots consécutifs de 4 octets chacun et a le format suivant :
	//— le premier mot de l’entrée est composé de la constante sur 16 bits KERNEL_CS (bits 31 à 16) et des 16 bits de poids faibles de l’adresse du traitant (bits 15 à 0) ;
	//— le deuxième mot est composé des 16 bits de poids forts de l’adresse du traitant (bits 31 à 16) et de la constante 0x8E00 (bits 15 à 0).
	// traitant KERNEL_CS    0x8E00 traitant

  #define I uint16_t

  I * entree = (I*)(0x1000);
  entree+=4*num_IT;//((int32_t)4*num_IT;
  //entree+=num_IT;
  //block unitaite de memoire : 2*(uint16_t=4octets) == uint32_t
  //[traitant,KERNEL_CS,0x8E00,traitant]

  printf("\nentree = [1000]+num_IT = [%08x]\n",(uint32_t)entree);

  printf("\ntraitant = [%08x]\n",(uint32_t)traitant);

  I poid_fort_traitant = (I) (((uint32_t)traitant)>>16);
  I poid_faible_traitant = (uint32_t)traitant & 0xffff;

  printf("\npoid_fort_traitant   = [%.4X]\n",poid_fort_traitant);
  printf("\npoid_faible_traitant = [%.4X]\n",poid_faible_traitant);

  entree[1]=   KERNEL_CS;
  entree[0]=(I)poid_faible_traitant;
  entree[3]=(I)poid_fort_traitant;
  entree[2]=(I)0x8E00;

  printf("\nentree = [%.4X] [%.4X] [%.4X] [%.4X]\n",entree[0],entree[1],entree[2],entree[3]);

}

void initialiser_interruptions(){
	//Initialiser l’IVT avec l’adresse du traitant

	init_traitant_IT(32,traitant_IT_32);

  //Initialiser l’IVT avec l’adresse du traitant
  init_traitant_IT(49,traitant_IT_49);

	//Initialiser l'interruption console
	init_traitant_IT(33, traitant_IT_33);

  //Régler la fréquence du PIT
  regler_la_frequence_de_l_horloge();

  //Démasquer l’IRQ 0 puis démasquer les IT
  masque_IRQ(0,0);


	//Démasquer l'IRQ 1 pour le clavier
	masque_IRQ(1,0);
	//sti();//démasquage de toute les interuptions
}
