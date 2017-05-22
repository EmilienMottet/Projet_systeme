/*
 * shellcmd.c
 */
#include "shellcmd.h"



void shell_ps(){
  afficher_simple_procesus_existants();
}
void shell_exit(){
  exit(0);
}

void shell_echo(int i){ // bascule echo dans le mode opposé
   cons_echo(i);
}

void shell_clear() {
  efface_ecran();
}
