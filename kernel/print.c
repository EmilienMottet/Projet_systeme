#include "print.h"

#define ptr_mem(l,c)  ( (uint16_t*)(0xB8000+2*(l*80+c)) )

uint16_t couleur_ecriture_ecrant = 0x0F00;

uint8_t couleur_ecrant(enum COULEUR_CONSOLE c){
	switch (c) {
		case NOIR:return 0x00;break;
		case BLEU_FONCE:return 0x01;break;
		case VERT_CLAIR:return 0x02;break;
		case CYAN:return 0x03;break;
		case ROUGE:return 0x04;break;
		case VIOLET:return 0x05;break;
		case MARRON:return 0x06;break;
		case GRIS_CLAIR:return 0x07;break;
		case GRIS:return 0x08;break;
		case POURPRE:return 0x09;break;
		case VERT:return 0x0A;break;
		case CYAN2:return 0x0B;break;
		case ORANGE:return 0x0C;break;
		case ROSE:return 0x0D;break;
		case JAUNE:return 0x0E;break;
		case BLANC:return 0x0F;break;
		default:assert(0);
	}
}

void assigner_couleur_ecriture_ecrant(enum COULEUR_CONSOLE couleur_caracteres,enum COULEUR_CONSOLE couleur_fond){
	couleur_ecriture_ecrant = 0xFF00 & ( couleur_ecrant(couleur_fond)<<12 | couleur_caracteres<<8 );
}

void ecrit_car(uint32_t lig, uint32_t col, char c){
		*ptr_mem(lig,col)=c|couleur_ecriture_ecrant;
}

void efface_ecran(){
	int i,j;
	for(i=0;i<25;i++) {
		for(j=0;j<80;j++) {
			ecrit_car(i,j,' ');
		}
	}
	place_curseur(0,0);
}

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

void traite_car(char c){
	ecrit_car(ligne_curseur,colonne_curseur,c);
}

void efface_car() {
	ecrit_car(ligne_curseur, colonne_curseur - 1, ' ');
}
typedef struct message_ecrant{
	int32_t lig,col;
	char s[200];
}message_ecrant;

void affiche_message_ecrant(char*s,int ligne,int colonne){
	int l;for(l=0;s[l];l++);
	int32_t lig=ligne_curseur;
	int32_t col=colonne_curseur;
	place_curseur(ligne,colonne);
	console_putbytes(s,l);
	place_curseur(lig,col);
}

void defilement(void){
	if(ligne_curseur>0)ligne_curseur--;
	memmove((void*)0xB8000,(void*)(0xB8000+2*80),2*80*25);
	memset((void*)(0xB8000+24*2*80),0,2*80);//nouvelle derniere ligne vide
}

void console_putbytes(const char *chaine, int taille){
	int32_t i;
	int32_t lig=ligne_curseur;
	int32_t col=colonne_curseur;
	for(i=0;i<taille;i++){
		switch(*chaine){
			case '\r':col=0;ecrit_car(lig,col,' ');break;
			case '\n':col=0;lig++;break;
			case '\t':col+=8;break;
			case '\b':if(col)col--;ecrit_car(lig,col,' ');break;
			default:ecrit_car(lig,col,*chaine);col++;
		}
		if(col>=80){col=0;lig++;}
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

void affiche_en_haut_a_droite(char*s){
	int32_t l;for(l=0;s[l];l++);
	if(l>79){printf("erreur affiche_en_haut_a_droit, chaine trop longue\n");return;}
	int32_t lig=ligne_curseur;
	int32_t col=colonne_curseur;
	place_curseur(0,80-l);
	console_putbytes(s,l);
	place_curseur(lig,col);
}

const char *bit_rep[16] = {
    [ 0] = "0000", [ 1] = "0001", [ 2] = "0010", [ 3] = "0011",
    [ 4] = "0100", [ 5] = "0101", [ 6] = "0110", [ 7] = "0111",
    [ 8] = "1000", [ 9] = "1001", [10] = "1010", [11] = "1011",
    [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
};

void print_byte(uint8_t byte){printf("%s%s", bit_rep[byte >> 4], bit_rep[byte & 0x0F]);}
