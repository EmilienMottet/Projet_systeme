#ifndef FILE_H
#define FILE_H

#include "process.h"


#define NBQUEUE 5
#define TAILLEMAXFILE 512
typedef struct process_bloque {
  int pid;
  int prio;
  link lien;
} process_bloque;

typedef struct tab_file {
  int nbelem;
  link * elem;
} tab_file;

typedef struct message_interne{
  int val;
  struct message_interne * next;
} message_interne;

typedef message_interne * message;

typedef struct File_interne{
  message debut;
  message fin;
}File_interne;

typedef File_interne File;

typedef struct file_message{
  int statut;
  int size;
  int nbelem;
  int nbproc_envoi;
  int nbproc_recep;
  File file;
  link liste_proc_bloque_reception;
  link liste_proc_bloque_envoi; // listes des proc bloqués sur la file d'attente
  //nbr process_bloque a rajouter si besoin
} file_message;




/*
*Tableau des structures de file de message
*/
extern int NBFILES;
file_message tab_file_message[NBQUEUE];
/*Création d'une file de message*/
int pcreate(int count);
/*Destruction d'une file de message */
int pdelete(int fid);
/*Récupération d'un message d'une file*/
int preceive(int fid,int *message);
/*Réinitialisation d'une file de message*/
int preset(int fid);
/*Envoi d'un message dans une file*/
int psend(int fid, int message);
/*Obtention d'information sur une file de message*/
int pcount(int fid, int *count);

void reset_file(File * L);
int read_message(File * L);
void add_message(int val,File * L);
void initialiser_tableau_file();


#endif
