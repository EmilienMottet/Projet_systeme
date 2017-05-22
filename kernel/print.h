
#ifndef LE_PRINT_H
#define LE_PRINT_H

#include <stdint.h> // pour uint32_t
#include <string.h> // pour memmove & memset
#include <stdio.h>
#include "cpu.h" // pour outb

enum COULEUR_CONSOLE{
	NOIR,BLEU_FONCE,VERT_CLAIR,CYAN,ROUGE,VIOLET,MARRON,GRIS_CLAIR,GRIS,
	POURPRE,VERT,CYAN2,ORANGE,ROSE,JAUNE,BLANC
};

void efface_car();

void ecrit_car(uint32_t lig, uint32_t col, char c);

void efface_ecran();

void place_curseur(uint32_t lig, uint32_t col);

void traite_car(char c);

void defilement(void);

void endline();

void affiche_en_haut_a_droite(char*s);

void console_putbytes(const char *chaine, int taille);

void affiche_en_haut_a_droite(char*s);

void print_byte(uint8_t byte);

void affiche_message_ecrant(char*s,int ligne,int colonne);

void assigner_couleur_ecriture_ecrant(enum COULEUR_CONSOLE couleur_caracteres,enum COULEUR_CONSOLE couleur_fond);

uint16_t couleur_ecriture_ecrant;

#endif
