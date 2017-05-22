#include <cpu.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

uint16_t *ptr_mem(uint32_t lig, uint32_t col);
void ecrit_car(uint32_t lig, uint32_t col, char c);
void efface_ecran(void);
void place_curseur(uint32_t lig, uint32_t col);
void traite_car(char c);
void defilement(void);
void console_putbytes(char *chaine, int32_t taille);
void affiche_haut_droite(char* c);
