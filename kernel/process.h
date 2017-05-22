#ifndef _PROCESS_H
#define _PROCESS_H

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h> // pour uint32_t
//#include <inttypes.h>
#include "segment.h"
#include "cpu.h"
#include "start.h"
#include "console.h"
#include "queue.h"
#include "hash.h"
#include "print.h"
#include "page.h"


#define confirme(assertion,...)\
  if(!(assertion)){\
    assigner_couleur_ecriture_ecrant(ROUGE,NOIR);\
    printf("\nechec de confirme dans %s a la ligne %d \n" #__VA_ARGS__ "\n",__FILE__, __LINE__);\
    printf("pid actif : %d\n",pid_actif);\
    wait_clock(current_clock()+40);\
    terminaison_processus(table_des_processus+pid_actif);\
  }

#define confirme_sans_aboder(assertion,...)\
    if(!(assertion)){\
      assigner_couleur_ecriture_ecrant(ROUGE,NOIR);\
      printf("\nechec de confirme dans %s a la ligne %d \n" #__VA_ARGS__ "\n",__FILE__, __LINE__);\
      printf("pid actif : %d\n",pid_actif);\
      wait_clock(current_clock()+40);\
    }

#define NBPROC 15

extern hash_t programmes;//systeme_de_fichiers

extern void ctx_sw(uint32_t*,uint32_t*);

/**
 * enumeration des differents etats d'un processus
 */
 typedef enum ETAT_PROCESSUS{
   ACTIF,
   ACTIVABLE,
   BLOQUE_SUR_SEMAPHORE,// genre un wait
   BLOQUE_SUR_ENTREE_SORTIE,
   BLOQUE_SUR_ATTENTE_D_UN_FILS,
   BLOQUE_SUR_RECEPTION,
   BLOQUE_SUR_ENVOI,
   ENDORMI,
   MORT,
   ZOMBIE
 } ETAT_PROCESSUS;

 #define TAILLE_PILE 512

/**
 * definition de la structure de donnee d'un processus
 */
typedef struct processus {
  link maillon_comme_activable;// maillon potentiel de file_ordonnee_des_processus_activables
  link maillon_comme_fils;// maillon de de la file des fils du pere si ce pere existe
  int pid;
  int valresetfile;
  char nom[70];
  ETAT_PROCESSUS etat;
  uint32_t temps_reveil;
  uint32_t registres[7];//contexte
  uint32_t pile[TAILLE_PILE];
  int prio;
  struct processus * pere;
  link fils;
  void * args;
  int retval; //valeur de retour du processus
  int (*code)(void*);
  int attend_fils;//pid du fils à attendre OU -2 pour attendre n'importe quel fils OU -1 si pas d'attente OU pid du fils qui a désactivé l'attente si attend_fils était égal à -2
  uint32_t * page_directory; // page directory allouee a la creation du process et qui permet de faire la correspondance adresse virtuelle adresse physique
  unsigned long wait_ieme_interruption_clock;
  link maillon_comme_attente_horloge;
  link maillon_comme_attente_cons_read;
} processus;

processus table_des_processus[NBPROC];

link file_ordonnee_des_processus_activables;

link file_processus_endomis_sur_wait_clock;

int32_t pid_actif;

#include "afficher_processus.h"

void definir_etat_processus(processus * pr,ETAT_PROCESSUS nouvel_etat);
void afficher_simple_processus(processus*pr);
void afficher_processus(processus*pr);
void afficher_le_nom_des_processus_activables_et_du_processus_actif();
void initialiser_gestion_des_processus();
void idle(void);
int start(const char *nom,unsigned long ssize, int prio, void *args);
void ordonnancer();
int kill(int pid);
int waitpid(int pid, int *retvalp);
int getpid(void); // La valeur de retour de getpid est le pid du processus appelant cette primitive.
int getprio(int pid);
void exit(int retval);
int getprio(int pid);
int chprio(int pid, int newprio);
int getvalreset(); //permet de connaitre la valeur de resetfile du process actif
void setvalreset();//permet de set la valeur de resetfile du process actif
void terminaison_processus(processus *);

#endif
