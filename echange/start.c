
#include <cpu.h>
#include <inttypes.h>

//rien a rendre juste faire la meme chose pendant le partiel

#define ecr(l,c)  ( (uint16_t*)(0xB8000+2*(80*l+c)) )
#define HALT while(1)hlt();
#define ptr_mem(l,c)  ( (uint16_t*)(0xB8000+2*(l*80+c)) )
//make
//make run

//gdb kernel.bin
//target remote :1234
//b kernel_start
//c
//n
//n

//adresse de début de l'écrant est 0xB8000
//uint8_t inb(uint16_t num_port) : renvoie l’octet lu sur le port de numéro num_port
//void outb(uint8_t val, uint16_t port) : envoie la valeur val sur le port num_port

//////////////////////////////////////////////////////////////////////////////////////////////
//1
//uint16_t *ptr_mem(uint32_t lig, uint32_t col){// on peut faire un define pour plus de rapidité
//	return ecr(lig,col);//(uint16_t*)(0xB8000+2*(lig*80+col));// !! metre des parentheses devant le caste
//}

//2
void ecrit_car(uint32_t lig, uint32_t col, char c){*ptr_mem(lig,col)=c|0x0F00;}

//3
void efface_ecran(){int i,j;for(i=0;i<25;i++)for(j=0;j<80;j++)ecrit_car(i,j,' ');}

//4
uint32_t ligne_curseur;// 25 lignes
uint32_t colonne_curseur;// 80 colonnes
void place_curseur(uint32_t lig, uint32_t col){
	uint16_t e = (uint16_t)(lig*80+col);
	outb(0xF,0x3D4);
	outb((uint8_t)(e),0x3D5);
	outb(0xE,0x3D4);
	outb((uint8_t)(e>>8),0x3D5);
	ligne_curseur = lig;
	colonne_curseur = col;
}

//5
void traite_car(char c){
	ecrit_car(ligne_curseur,colonne_curseur,c);
}

//6
#include<string.h>
void defilement(void){
	if(ligne_curseur>0)ligne_curseur--;
	memmove((void*)0xB8000,(void*)(0xB8000+2*80),2*80*25);
	memset((void*)(0xB8000+24*2*80),0,2*80);//nouvelle derniere ligne vide
}

//7
void console_putbytes(char *chaine, int32_t taille){
	int32_t i;
	int32_t lig=ligne_curseur;
	int32_t col=colonne_curseur;
	for(i=0;i<taille;i++){
		switch(*chaine){
			case '\n':col=0;lig++;break;
			case '\t':col+=4;break;
			default:ecrit_car(lig,col,*chaine);col++;
		}
		if(col==80){col=0;lig++;}
		if(lig==25){lig--;defilement();}
		chaine++;
	}
	place_curseur(lig,col);
}

void endline(){
	int32_t lig=ligne_curseur;
	int32_t col=colonne_curseur;
	col=0;
	if(++lig==25){lig--;defilement();}
	place_curseur(lig,col);
}
#include <stdio.h>
//////////////////////////////////////////////////////////////////////////////////////////////
//1
void affiche_en_haut_a_droite(char*s){
	int32_t l;for(l=0;s[l];l++);
	if(l>79){printf("erreur affiche_en_haut_a_droit, chaine trop longue\n");return;}
	int32_t lig=ligne_curseur;
	int32_t col=colonne_curseur;
	place_curseur(0,80-l);
	console_putbytes(s,l);
	place_curseur(lig,col);
}
//2
void ordonnance_touniquet();
void tic_PIT(void){
  //signal toutes les 20 ms (50 Hz) si QUARTZ vaut 0x1234DD et CLOCKFREQ vaut 50

	//Pour acquitter une des interruptions qu’on manipule dans ce TP,
  //on doit envoyer la commande sur 8 bits 0x20 sur le port de commande 0x20.
  //Comme on veut le faire en C au tout début de la partie du traitant
  //d’interruption gérant l’affichage de l’horloge,
  //on utilisera le bout de code suivant :
	outb(0x20,0x20);//bon

	static char s[200];

	static int tps = 0;
	tps+=20;
  static int t;t=tps/1000;
  if(t<60)sprintf(s," %d secondes",t);
  else sprintf(s," %d minutes %d secondes",t/60,t%60);

	affiche_en_haut_a_droite(s);

	ordonnance_touniquet();
}
//3
void traitant_IT_32(void);//prototype visible
#include<segment.h>//pour KERNEL_CS

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

  //ORDRE ??????????????????????????????????????????????????????????
  entree[1]=   KERNEL_CS;
  entree[0]=(I)poid_faible_traitant;
  entree[3]=(I)poid_fort_traitant;
  entree[2]=(I)0x8E00;

  printf("\nentree = [%.4X] [%.4X] [%.4X] [%.4X]\n",entree[0],entree[1],entree[2],entree[3]);

}
//4
#define QUARTZ 0x1234DD
#define CLOCKFREQ 50
void regler_la_frequence_de_l_orloge(){
	//régler la fréquence de l'orloge

	//on envoie la commande sur 8 bits 0x34 sur le port de commande 0x43 cette commande indique à l’horloge que l’on va lui envoyer la valeur de réglage de la fréquence sous la forme de deux valeurs de 8 bits chacunes qui seront émises sur le port de données
	outb(0x34,0x43);

  uint16_t e = (uint16_t)( (uint32_t)QUARTZ / (uint32_t)CLOCKFREQ );
  //NE MARCHE PAS SI ON FAIT : e = (uint16_t)QUARTZ / (uint16_t)CLOCKFREQ

	//on envoie les 8 bits de poids faibles de la valeur de réglage de la fréquence sur le port de données 0x40 : cela peut se faire simplement par
	//outb((QUARTZ / CLOCKFREQ) % 256, 0x40);
  outb(e & 0xffff, 0x40);
	//où QUARTZ vaut 0x1234DD et CLOCKFREQ vaut 50

	//on envoie ensuite les 8 bits de poids forts de la valeur de réglage sur le même port 0x40
	//uint16_t e = (uint16_t)QUARTZ / (uint16_t)CLOCKFREQ;
	outb((uint8_t)(e>>8),0x40);
}


const char *bit_rep[16] = {
    [ 0] = "0000", [ 1] = "0001", [ 2] = "0010", [ 3] = "0011",
    [ 4] = "0100", [ 5] = "0101", [ 6] = "0110", [ 7] = "0111",
    [ 8] = "1000", [ 9] = "1001", [10] = "1010", [11] = "1011",
    [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
};

void print_byte(uint8_t byte){printf("%s%s", bit_rep[byte >> 4], bit_rep[byte & 0x0F]);}

//5
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
//6
//////////////////////////////////////////////////////////////////////////////////////////////

/*
//test
uint16_t *ecrant(uint32_t lig, uint32_t col){
    return (uint16_t*)(0xB8000+2*(lig*80+col));// !! metre des parentheses devant le caste
}
void effacer_ecrant(){int i,j;for(i=0;i<100;i++)for(j=0;j<80;j++)*ecrant(i,j) = ' ' | 0x0F00;}

void placer_curseur(uint32_t lig, uint32_t col){
    uint16_t e = (uint16_t)(lig*80+col);
    outb(0xF,0x3D4);//le port de commande gérant la position du curseur est le 0x3D4
    outb((uint8_t)(e),0x3D5);//le port de données associé est le 0x3D5
    outb(0xE,0x3D4);
    outb((uint8_t)(e>>8),0x3D5);
    ligne_curseur = lig;
    colonne_curseur = col;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////
enum ETAT{ELUT,ACTIVABLE};
#define TAILLE_PILE 512

struct processus{//un processus est en fait un cotexte //processus légers (ou threads)
	int pid;
	char nom[70];
	enum ETAT etat;
	int registres[5];//champ regs pointe sur le sommet de la pile
	// registres contient %esp qui est le pointeur de pile
	int pile[TAILLE_PILE];

};
typedef struct processus processus;

#define NB_MAX_PROCESSUS 4

processus les_processus[NB_MAX_PROCESSUS];

static int pid_actif = 0;
int mon_pid(void){return pid_actif;}
const char *mon_nom(void){return les_processus[pid_actif].nom;}

void ctx_sw(int*,int*);

int32_t cree_processus(void (*code)(void), char *nom){
	static int pid = 1;
	if(pid==NB_MAX_PROCESSUS)return -1;
	processus * pr = les_processus + pid;
	pr->pid = pid;
	strcpy(pr->nom, nom);
	pr->etat = ACTIVABLE;
	pr->registres[1] = (int) (pr->pile + TAILLE_PILE - 1);
  pr->pile[TAILLE_PILE - 1] = (int) code;
  printf("cree_processus [%s] pid = %i\n", les_processus[pid].nom, les_processus[pid].pid);
  return pid++;
}

void ordonnance_touniquet(){//echange sans fin
	if(NB_MAX_PROCESSUS==1)return;
	int i = pid_actif;
	pid_actif = (i + 1)%NB_MAX_PROCESSUS;
	printf("ordonnance_touniquet [%s] pid = %i\n", les_processus[i].nom, les_processus[i].pid);
	printf("ordonnance_touniquet [%s] pid = %i\n", les_processus[pid_actif].nom, les_processus[pid_actif].pid);

	ctx_sw(	les_processus[i].registres,les_processus[pid_actif].registres);
	//ctx_sw(	(int*)les_processus[i].registres,
	//			(int*)les_processus[pid_actif].registres);
	//PAUSE
	printf("est ce ? ordonnance_touniquet [%s] pid = %i\n",
			les_processus[pid_actif].nom,
			les_processus[pid_actif].pid);
	//PAUSE
}

/*
void ordonnance(){//echange sans fin
	static processus * actif = les_processus+0;//inutile pour les deux premiers testes
	processus * avant = actif;

	if(actif==les_processus+0)actif = les_processus+1;
	else actif = les_processus+0;

	if(avant!=actif)
		ctx_sw(	(int)avant->registres,
				(int)actif->registres);

	printf("[%s] pid = %i\n", avant->nom, actif->pid);
}
*/
void idle(void)
{
	/*//premier teste : idle passe la main à proc1 qui hlt
	printf("[idle] je tente de passer la main a proc1...\n");
	ctx_sw(	(int)les_processus[0].registres,
			(int)les_processus[1].registres);//changement de processus
	*/

	/*//second teste : aller-retour 3 fois
	for (int i = 0; i < 3; i++) {
		printf("[idle] je tente de passer la main a proc1...\n");
		ctx_sw(	(int)les_processus[0].registres,
				(int)les_processus[1].registres);
		printf("[idle] proc1 m a redonne la main\n");
	}
	printf("[idle] je bloque le systeme\n");
	hlt();
	*/

	/*
	for (;;) {
		//printf("[%s] pid = %i\n", mon_nom(), mon_pid());
		ordonnance();
	}
	*/

	for (;;) {
		printf("idle [%s] pid = %i\n", mon_nom(), mon_pid());
		//ne mettez pas d’appel à ssttii() dans kernel_start :
		//c’est la fonction idle qui activera les interruptions la première fois
		sti();
		hlt();
		cli();
	}
}
void proc1(void)
{
	/*//premier teste : idle passe la main à proc1 qui hlt
	printf("[proc1] idle m a donne la main\n");
	printf("[proc1] j arrete le systeme : hlt();\n");
	hlt();
	*/

	/*//second teste : aller-retour 3 fois
	for (;;) {
		printf("[proc1] idle m a donne la main\n");
		printf("[proc1] je tente de lui la redonner...\n");
		ctx_sw(	(int)les_processus[1].registres,
				(int)les_processus[0].registres);
	}
	*/

	/*
	for (;;) {
		//printf("[%s] pid = %i\n", mon_nom(), mon_pid());
		ordonnance();
	}
	*/

	for (;;) {
		printf("proc1 [%s] pid = %i\n", mon_nom(), mon_pid());
		sti();
		hlt();
		cli();
	}

}

void initialiser_les_processus(void){
	strcpy(les_processus[0].nom, "idle");
	les_processus[0].pid = 0;
	les_processus[0].etat= ACTIVABLE;
	int k;for(k=0;k<NB_MAX_PROCESSUS;k++)
		cree_processus(proc1,"processus");
}

void kernel_start(void)
{
	efface_ecran();
	place_curseur(0,0);
	printf("la chaine du printf\n");
	//endline();
	printf("la chaine du printf la chaine du printf la chaine du printf la chaine du printf la chaine du printf la chaine du printf la chaine du printf la chaine du printf la chaine du printf la chaine du printf\n");
	affiche_en_haut_a_droite("cette chaine doit s'afficher en haut a droite");

    //Initialiser l’IVT avec l’adresse du traitant
    init_traitant_IT(32,traitant_IT_32);

    //Régler la fréquence du PIT
    regler_la_frequence_de_l_orloge();

    //Démasquer l’IRQ 0 puis démasquer les IT
    masque_IRQ(0,0);

    //le démasquage de toute les interuptions sti(); se fait dans idle
    initialiser_les_processus();
	idle();
	//ssttii();
	//while(1)hlt();
	//idle();
}

void kernel_start_1(void)
{
	efface_ecran();place_curseur(0,0);

	initialiser_les_processus();

	idle();
}


void initialiser_les_processus_1(void){
	//le sujet souhaite que les noms des processus aient une taille maximale
	strcpy(les_processus[0].nom, "idle");
	les_processus[0].pid = 0;
	les_processus[0].etat= ACTIVABLE;

	//idle.registres :
	//il n’est pas nécessaire d’initialiser la zone de sauvegarde
	//de %esp pour idle puisque ce processus sera exécuté
	//directement par la fonction kernel_start
	//les_processus[0].pile[0] = (int)first_stack;//(int)kernel_start;
	//les_processus[0].registres[1] =  (int)les_processus[0].pile;

	//idle.pile :
	//Il n’est pas nécessaire d’initialiser la pile
	//d’exécution du processus idle :
	//en fait, ce processus n’utilisera pas la pile
	//allouée dans sa structure de processus mais
	//directement la pile du noyau
	//(celle qui est utilisée par la fonction kernel_start notamment)

	//en effet kernel_start est de la forme :
	//void kernel_start(void)
	//{
	//	initialiser_les_processus();
	//	idle();
	//}

	strcpy(les_processus[1].nom, "process1");
	les_processus[1].pid = 1;
	les_processus[1].etat= ACTIVABLE;
	/*
	//case en sommet de pile doit contenir
	//l’adresse de la fonction proc1 : c’est
	//la première exécution de proc1
	les_processus[1].pile[0] = (int)proc1;
	// en assayant toutes les entrées du tableau des registres 1 semble etre notre %esp
	//les_processus[1].registres[0] =  0;//(int)les_processus[1].pile;// 0 ou 1 ou 2 ou 3 .. ???
	les_processus[1].registres[1] =  (int)les_processus[1].pile;// 0 ou 1 ou 2 ou 3 .. ???
	//les_processus[1].registres[2] =  0;//(int)les_processus[1].pile;// 0 ou 1 ou 2 ou 3 .. ???
	//les_processus[1].registres[3] =  0;//(int)les_processus[1].pile;// 0 ou 1 ou 2 ou 3 .. ???
	//les_processus[1].registres[4] =  0;//(int)les_processus[1].pile;// 0 ou 1 ou 2 ou 3 .. ???
	*/

	//d'après : https://github.com/sacortesh/system/blob/master/src/processus.c (tag : %esp ctx_sw)
	//on utilise TAILLE_PILE - 3 et (unsigned long) à la place de (int)
	// mais TAILLE_PILE - 2 marche aussi tout comme TAILLE_PILE - 1
	// (le sujet à tenté d'expliquer d'utiliser la pile à partir ... décrementation)
	// on peut aussi utiliser (int)
	// comme avant : en assayant toutes les entrées du tableau des registres 1 semble etre notre %esp
	les_processus[1].registres[1] = (int) &les_processus[1].pile[TAILLE_PILE - 1];
  les_processus[1].pile[TAILLE_PILE - 1] = (int) proc1;
}

void kernel_start_avant(void)
{
	efface_ecran();
	place_curseur(0,0);
	//printf("la chaine du printf\n");
	printf("abcabcdefghijklmopqrstabca bcdefghijklmopqrstabcabcdefghijk lmopqrstabcabcdefghijklmopqrstabcabcdefghijkl mopqrstabcabcdefghijklmopqrstabcabcdefghijkl mopqrstvabcabcdefghijklmopqrstabcabcdefghijklmopqrsta bcabcdefghijklmopqrst\n");
	//int i;for(i=0;i<300;i++)printf(" CONTINUE i=%d",i);
	affiche_en_haut_a_droite("cette chaine doit s'afficher en haut a droite");


    //Initialiser l’IVT avec l’adresse du traitant
    init_traitant_IT(32,traitant_IT_32);

    //Régler la fréquence du PIT
    regler_la_frequence_de_l_orloge();

    //Démasquer l’IRQ 0 puis démasquer les IT
    masque_IRQ(0,0);

    sti();//démasquage de toute les interuptions

    affiche_en_haut_a_droite("cette chaine doit s'afficher en haut a droite");
    while (1) {// on ne doit jamais sortir de kernel_start
        // cette fonction arrete le processeur
        hlt();
    }
}
