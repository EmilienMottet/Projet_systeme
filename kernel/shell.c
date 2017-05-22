/*
 * shell.c
 */
#include "shell.h"



void init_shell(){
hash_set(&programmes,"shell",&shell);
}

int shell(){
  int i;
  char  cmd[NBR_MAX_BUFF];
  while (1) {
    assigner_couleur_ecriture_ecrant(VERT,NOIR);
    cons_write("\nshell>> ",9);
    assigner_couleur_ecriture_ecrant(BLANC,NOIR);
    i=cons_read(cmd, NBR_MAX_BUFF);
    cons_write("\n",2);
    cons_write(cmd,i);
    cons_write("\n",2);
    //if (!strcmp(cmd,"ps")) afficher_processus_simple();
    if (!strcmp(cmd,"ps")) shell_ps();

    else if (!strcmp(cmd,"exit")) shell_exit();

    else if (!strcmp(cmd,"echo 1")) shell_echo(1);
    else if (!strcmp(cmd,"echo 0")) shell_echo(0);
    else if (!strcmp(cmd,"clear")) shell_clear();

    else if (!strncmp(cmd,"start ",6)){
      printf("'%s'\n", cmd+6);
      start(cmd+6,4000,2,NULL);
    }

    else cons_write(" commande inconnue",19);
  }

}
