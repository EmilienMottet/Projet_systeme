#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <inttypes.h>
#include "kbd.h"
#include "keyboard-glue.h"
#include "init_interruptions.h"
#include "print.h"


#ifndef CONSOLE_H
#define CONSOLE_H

#define NBR_MAX_BUFF 512    //128,256,512,1024

//Lecture sur le terminal
unsigned long cons_read(char *string, unsigned long length);
//Affichage sur le terminal
int cons_write(const char *str, long size);
//gestion du mode echo
void cons_echo(int on);
//traitant de l'IT clavier
void traitant_clavier(void);
char tampon[NBR_MAX_BUFF];

void init_file_ordonnee_des_processus_en_attente_de_cons_read();

#endif
