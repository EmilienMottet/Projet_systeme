#include "mem.h"
#include "file.h"
int NBFILES; //initialisée à  par défault




//Fonciton pour initialiser la table des files de message à NULL
void initialiser_tableau_file() {
  for(int i = 0; i < NBQUEUE; i++) {
    tab_file_message[i].statut = 0;

  }
}
void add_message(int val,File *  L){
  message p = mem_alloc(sizeof(*p));
  p->val=val;
  if(L->debut==NULL){
    L->debut=p;
    L->fin=p;
  }

  else{
    L->fin->next=p;
    L->fin=p;
    p->next=NULL;
  }
}

int read_message(File * L){
  message p;
  p=L->debut;
  L->debut=L->debut->next;
  int i = p->val;
  	mem_free(p, sizeof(*p));
  return i;
}

void reset_file(File * L){
  message p=L->debut;
  message q;
  if(L->debut!=NULL){
    while(p!=L->fin && p->next!=NULL){

      q=p->next;

      mem_free(p, sizeof(*p));
      p=q;
    }
    mem_free(p, sizeof(*p));
  }
  L->debut=L->fin=NULL;
}
//fonction de création d'une file de messages
int pcreate(int count){
  if(NBFILES>=NBQUEUE || count<=0 || count >TAILLEMAXFILE) {return -1;}
  int i=0;
  while(tab_file_message[i].statut!=0 && i!=NBQUEUE) i+=1;
  if (i==NBQUEUE){ return -1;}
  tab_file_message[i].statut=1;
  tab_file_message[i].nbelem=0;
  tab_file_message[i].nbproc_recep=0;
  tab_file_message[i].nbproc_envoi=0;
  tab_file_message[i].size=count;
  (tab_file_message[i].liste_proc_bloque_reception).prev=&tab_file_message[i].liste_proc_bloque_reception;
  (tab_file_message[i].liste_proc_bloque_reception).next=&tab_file_message[i].liste_proc_bloque_reception;
  (tab_file_message[i].liste_proc_bloque_envoi).prev=&tab_file_message[i].liste_proc_bloque_envoi;
  (tab_file_message[i].liste_proc_bloque_envoi).next=&tab_file_message[i].liste_proc_bloque_envoi;

  NBFILES+=1;
  return i;
}

int preceive(int fid,int *message){


  if(fid<0 || fid>=NBQUEUE ||tab_file_message[fid].statut==0)return -1;

  while(tab_file_message[fid].nbelem==0 && getvalreset()==0){
    process_bloque  * p = mem_alloc(sizeof(*p));
    p->prio=getprio(getpid());
    p->pid=getpid();
    queue_add(p,&(tab_file_message[fid].liste_proc_bloque_reception),process_bloque,lien,prio);
    definir_etat_processus(table_des_processus+getpid(),BLOQUE_SUR_RECEPTION);
    tab_file_message[fid].nbproc_recep++;
    ordonnancer();
  }
  if(getvalreset()==0){
  *message=read_message(&(tab_file_message[fid].file));

  tab_file_message[fid].nbelem-=1;
  if(tab_file_message[fid].nbelem==tab_file_message[fid].size-1){
    if(queue_empty((&tab_file_message[fid].liste_proc_bloque_envoi))==0){
      process_bloque * q;
      q=queue_out(&(tab_file_message[fid].liste_proc_bloque_envoi),process_bloque,lien);
      definir_etat_processus(table_des_processus+q->pid,ACTIVABLE);
      tab_file_message[fid].nbproc_envoi--;
        mem_free(q, sizeof(*q));
       ordonnancer();

    }
  }
  setvalreset();
  return 0;
}setvalreset();return -1;
}


int psend(int fid, int message){
  

  if(fid<0 || fid>=NBQUEUE ||tab_file_message[fid].statut==0)return -1;
  while(tab_file_message[fid].nbelem==tab_file_message[fid].size && getvalreset()==0){
    process_bloque  * p = mem_alloc(sizeof(*p));
    p->prio=getprio(getpid());
    p->pid=getpid();
    queue_add(p,&(tab_file_message[fid].liste_proc_bloque_envoi),process_bloque,lien,prio);
    definir_etat_processus(table_des_processus+getpid(),BLOQUE_SUR_ENVOI);
    tab_file_message[fid].nbproc_envoi++;
    ordonnancer();
    if(tab_file_message[fid].nbelem==tab_file_message[fid].size) return -1;
  }
  if(getvalreset()==0){
  add_message(message,&(tab_file_message[fid].file));
  tab_file_message[fid].nbelem+=1;
  //printf("%d\n",tab_file_message[fid].file->debut->val );
  if(tab_file_message[fid].nbelem==1){

    if(queue_empty(&(tab_file_message[fid].liste_proc_bloque_reception))==0){
      process_bloque * q;
      q=queue_out(&(tab_file_message[fid].liste_proc_bloque_reception),process_bloque,lien);

      definir_etat_processus(table_des_processus+q->pid,ACTIVABLE);
      tab_file_message[fid].nbproc_recep--;
      mem_free(q, sizeof(*q));
      //ordonnancer();
    }
  }
  setvalreset();
  return 0;
}setvalreset();return-1;
}


int preset(int fid) {
  //TODO: FAIRE PASSER DANS L'ETAT ACTIF OU ACTIVABLE
  //TOUS LES PROCESSUS SE TROUVANT DANS L'ETAT BLOQUE
  //SUR FILE PLEINE OU FILE VIDE

  //process_bloque *pt = NULL;

  if (fid < 0 || fid > NBQUEUE || tab_file_message[fid].statut==0) {
    return -1;
  }
  else {
    reset_file(&(tab_file_message[fid].file));
    tab_file_message[fid].nbelem=0;

    while(queue_empty(&(tab_file_message[fid].liste_proc_bloque_reception))==0) {//condition de fin de liste pour SD liste doublement chainee ?
      process_bloque *proc = queue_top(&(tab_file_message[fid].liste_proc_bloque_reception),process_bloque,lien);
      if (table_des_processus[proc->pid].etat == BLOQUE_SUR_RECEPTION) {
        definir_etat_processus(table_des_processus+proc->pid,ACTIVABLE); //liberer le processus
        table_des_processus[proc->pid].valresetfile=1;
        queue_del(proc,lien);
        mem_free(proc, sizeof(*proc));
      }
    }
    while(queue_empty(&(tab_file_message[fid].liste_proc_bloque_envoi))==0){//condition de fin de liste pour SD liste doublement chainee ?
      process_bloque *proc = queue_top(&(tab_file_message[fid].liste_proc_bloque_envoi),process_bloque,lien);
      if (table_des_processus[proc->pid].etat == BLOQUE_SUR_ENVOI) {
        definir_etat_processus(table_des_processus+proc->pid,ACTIVABLE);
         //liberer le processus
         table_des_processus[proc->pid].valresetfile=1;
        queue_del(proc,lien);
        mem_free(proc, sizeof(*proc));
      }
    }
    tab_file_message[fid].nbproc_envoi=0;
    tab_file_message[fid].nbproc_recep=0;
    // mettre nombre message à 0
    //vider la file de message message par message
    //attention garder les deux tetes de listes
    return 0;
  }
}
//fonction pour supprimer une file de messages
int pdelete(int fid) {
  int i = preset(fid); //on peut utiliser preset pur vider les queues et files
  if (i==-1) return i;
  tab_file_message[fid].size=0;
  tab_file_message[fid].statut=0; // il reste juste à liberer l'espace de la file de message
  NBFILES--;

  return 0;

}

//fonction pour réinitialiser une file de message

//Obtention d'information sur une file de messages
int pcount(int fid, int *count) {

  if (fid < 0 || fid > NBQUEUE) {
    return -1;
  }
  else {

    //le nombre de messages est dans le champ nbelem
    //on compte le nombre de processus en attente sur la file


  //si count n'est pas null on y place la valeur de compt
  //choix : nombre de messages + nombre de proc bloqué
  if(count != NULL) {

    *count =-tab_file_message[fid].nbproc_recep + tab_file_message[fid].nbelem+tab_file_message[fid].nbproc_envoi;
    printf("count %d \n",*count);
  }
  return 0;

}
}
