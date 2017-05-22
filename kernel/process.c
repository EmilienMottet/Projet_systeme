#include "process.h"
//#include "../shared/page.h"
#include "phy_mem_allocator.h"

hash_t programmes;//systeme_de_fichiers

void idle(){
  start("fonc_test", 4000, 0, NULL);
  start("test_C", 4000, 0, NULL);
  start("test_A", 4000, 0, NULL);
  start("fonc_test", 4000, 0, NULL);
  printf("attente idle %d\n",pid_actif);
  afficher_processus(table_des_processus+pid_actif);
  for(;;){// ne doit jamais se terminer
    sti();// demasquage des interruptions
    hlt();// pause du systeme
    cli();// masquage des interruptions
  }
}

void initialiser_gestion_des_processus(){
  pid_actif = -1;
  for(int i=0;i<NBPROC;i++)// tout les pid sont initialement libres
    table_des_processus[i] = (processus){.pid = -1,.etat = MORT};
  INIT_LIST_HEAD(&file_ordonnee_des_processus_activables);
  hash_init_string(&programmes);
  hash_set(&programmes,"idle",&idle);
  INIT_LIST_HEAD(&file_processus_endomis_sur_wait_clock);

  create_list_pages_vides();
}

// " Quand un processus est terminé, quelle que soit la manière :
// il passe dans l'état zombie si son père existe toujours sinon il est immédiatement détruit (le pid est libéré) ;
// chaque fils du processus terminé est marqué comme n'ayant plus de père et les fils zombies sont détruits.
// Tout processus détruit doit être enlevé de la liste des fils de son père, mais les zombies y restent. "

// terminaison_processus : termine le processus
void terminaison_processus(processus * pr){// passe à mort ou zombie

  confirme(pr>=table_des_processus&&pr<table_des_processus+NBPROC);
  confirme(pr->pid!=-1);
  confirme(pr->pid!=0,"ne pas terminer idle !");

  if(!pr->pere)definir_etat_processus(pr,MORT);
  else{
    if(pr->pere->etat == BLOQUE_SUR_ATTENTE_D_UN_FILS
      && (pr->pere->attend_fils == -2 || pr->pere->attend_fils == pr->pid) ){// -2 : attend n'importe quel fils
      confirme(pr->pere->etat == BLOQUE_SUR_ATTENTE_D_UN_FILS);
      if(pr->pere->attend_fils == -2)pr->pere->attend_fils = pr->pid;// indique quel fils desactive l'attente
      definir_etat_processus(pr->pere,ACTIVABLE);
    }

    definir_etat_processus(pr,ZOMBIE);
  }

  desalloue_pages_process(pr->pid);
  free_page(pr->page_directory);

  ordonnancer();
}

void definir_etat_processus(processus * pr,ETAT_PROCESSUS nouvel_etat){

  confirme(pr>=table_des_processus&&pr<table_des_processus+NBPROC);
  confirme(pr->pid!=-1);

  if(pr->etat == nouvel_etat)return;// pas de changement d'état

  if(pr->etat == ACTIVABLE && nouvel_etat != ACTIF)
    queue_del(pr, maillon_comme_activable);

  if(pr->etat == BLOQUE_SUR_SEMAPHORE)
    queue_del(pr, maillon_comme_attente_horloge);

  processus * fils,*fils_suiv;

  switch (nouvel_etat){
    case ACTIVABLE:
      queue_add(pr, &file_ordonnee_des_processus_activables, processus, maillon_comme_activable, prio);
      break;

    case ACTIF:
      confirme(pr->etat == ACTIVABLE);
      pid_actif = pr->pid;
      break;

    case MORT:{
        //confirme(!pr->pere);

        // "chaque fils du processus terminé est marqué comme n'ayant plus de père et les fils zombies sont détruits."
        queue_for_each(fils,&pr->fils,processus,maillon_comme_fils){
          debut_boucle:
          fils->pere = 0;
          if(fils->etat == ZOMBIE){// suppression d'un noeud de pr->fils sans fausser l'iteration
            fils_suiv = queue_entry(fils->maillon_comme_fils.next,processus,maillon_comme_fils);
            definir_etat_processus(fils,MORT);// supprime de la liste des fils le fils
            if( &fils_suiv->maillon_comme_fils == &pr->fils)break;
            else goto debut_boucle;
          }
        }
        pr->pid = -1;
        // "Tout processus détruit doit être enlevé de la liste des fils de son père, mais les zombies y restent."
        if(pr->pere&&pr->pere->etat!=MORT)
          queue_del(pr,maillon_comme_fils);
      }
      break;

    case ZOMBIE:{
        queue_for_each(fils,&pr->fils,processus,maillon_comme_fils){
          debut_boucle_zombie:
          if(fils->etat == ZOMBIE){// suppression d'un noeud de pr->fils sans fausser l'iteration
            fils_suiv = queue_entry(fils->maillon_comme_fils.next,processus,maillon_comme_fils);
            definir_etat_processus(fils,MORT);// supprime de la liste des fils le fils
            if( &fils_suiv->maillon_comme_fils == &pr->fils)break;
            else goto debut_boucle_zombie;
          }
        }
      }
      break;

    case BLOQUE_SUR_SEMAPHORE:
      queue_add(pr, &file_processus_endomis_sur_wait_clock, processus, maillon_comme_attente_horloge, prio);
    break;

    default:;
  }

  pr->etat = nouvel_etat;
}

void ordonnancer(){

  afficher_le_nom_des_processus_activables_et_du_processus_actif();

  if(queue_empty(&file_ordonnee_des_processus_activables))return;

  // observer le processus le plus prioritaire activable
  processus * pr = queue_top(&file_ordonnee_des_processus_activables, processus, maillon_comme_activable);

  processus * actif = pid_actif==-1 ? 0 : table_des_processus + pid_actif;

  if( actif && actif->etat == ACTIF && pr->prio < actif->prio )return;// meme actif

  // retirer le prochain processus actif de la file des processus activables
  queue_out(&file_ordonnee_des_processus_activables, processus, maillon_comme_activable);

  definir_etat_processus(pr,ACTIF);

  confirme(pr->pid == pid_actif);

  //TODO utiliser les fonctions pour changer de contexte, dont put_cr3
  if(actif && actif->etat== ACTIF)
    definir_etat_processus(actif,ACTIVABLE);

  confirme(pr!=actif);

  if(actif)ctx_sw(actif->registres,pr->registres);
  else{
    uint32_t n_importe[7];
    ctx_sw(n_importe,pr->registres);
  }
}

void vie_processus(){
  confirme(pid_actif>-1);
  processus * pr = table_des_processus + pid_actif;
  confirme(pr->pid>-1);
  pr->retval = pr->code(pr->args);
  terminaison_processus(pr);
}


int k_start(const char *nom,unsigned long ssize, int prio, void *args){
	return start(nom,ssize,prio, args);
}


int start(const char *nom,unsigned long ssize, int prio, void *args){
  int pid_libre;
  int nb_page=0;
  int i;

  //trouver un pid libre
  for(int i=0;i<NBPROC;i++){
    if(table_des_processus[i].pid==-1){
      pid_libre = i;
      goto pid_libre_trouve;
    }
  }

  printf("trop de processus crees : nombre max = %d\n",NBPROC);

  return -1;// trop de processus ! seul NBPROC peuvent etre cree

  pid_libre_trouve:

  confirme(pid_libre>=0&&pid_libre<NBPROC);

  // à faire : il faut allouer une mémoire de taille ssize pour le processus
  //  ssize++;// à retirer : permet de compiler
  //ssize = (int)args;// à retirer : permet de compiler
  nb_page=(int)(ssize/4096)+1;

  int existe = hash_isset(&programmes,(char*)nom);
  confirme_sans_aboder(existe,"faire hash_set(&programmes,\"nom\",&prg)");// le programme doit exister
  if(!existe)return -1;

  int (*code)(void*) = (int (*)(void*))hash_get(&programmes,(char*)nom,0);

  assert(code);

	processus * pr = table_des_processus + pid_libre;

  *pr = (processus){
    .pid = pid_libre,
    .etat = ENDORMI,
    .valresetfile=0,
    .temps_reveil = 0,
    .prio = prio,
    .pere = pid_actif!=-1 ? table_des_processus + pid_actif : 0,
    .args = args,
    .retval = 0,
    .code = code,
    .attend_fils = -1
  };

  pr->page_directory=create_page_directory();
  assert(pr->page_directory != NULL);
  alloue_pages_process(pid_libre, nb_page, READ);
  assert(get_nb_pages(pid_libre) == nb_page);

  for(i=0; i<nb_page; i++){
	  adresse* adr = get_adr(pid_libre, i);
    if ((adr == NULL) || (put_address_reel(adr->adresse_phy, adr->adresse_virt, adr->perm ,pr->page_directory)!=0))
    {
    	assert(true);// erreur creation process
    }
  }

	strcpy(pr->nom, nom); // TODO verifier la taille max pour eviter les debordements

  pr->registres[1] = (int) (pr->pile + TAILLE_PILE - 1); // vers le code !
  pr->pile[TAILLE_PILE - 1] = (int) vie_processus;

  INIT_LINK(&pr->maillon_comme_activable);
  INIT_LINK(&pr->maillon_comme_fils);
  INIT_LINK(&pr->maillon_comme_attente_horloge);
  INIT_LINK(&pr->maillon_comme_attente_cons_read);
  INIT_LIST_HEAD(&pr->fils);

  if(pid_actif!=-1)// ajouter le nouveau processus pr à la liste des fils du pid actif
    queue_add(pr, &table_des_processus[pid_actif].fils, processus, maillon_comme_fils, prio);

  definir_etat_processus(pr,ACTIVABLE);

  ordonnancer();

  return pid_libre;// plus libre maintenant
}

//Pour gérer correctement la terminaison des processus, il est nécessaire que chacun connaisse son père --- celui qui l'a créé --- et ses fils --- ceux qu'il a créés.
//De plus, on définit un état spécial zombie, où le processus est considéré comme inexistant pour tous les appels systèmes sauf pour waitpid
//qui permet au père de récupérer la valeur de retour du processus.
//Quand un processus est terminé, quelle que soit la manière :
//il passe dans l'état zombie si son père existe toujours sinon il est immédiatement détruit (le pid est libéré) ;
//chaque fils du processus terminé est marqué comme n'ayant plus de père et les fils zombies sont détruits.
//Tout processus détruit doit être enlevé de la liste des fils de son père, mais les zombies y restent. (Pas compris ? si leur pere existe ils sont en ZOMBIE donc ce cas n'existe pas ?)

int kill(int pid){
  //La primitive kill termine le processus identifié par la valeur pid.
  //Si ce processus était bloqué dans une file d'attente pour un quelconque élément système, alors il en est retiré.
  // "Si la valeur de pid est invalide, la valeur de retour est strictement négative. En cas de succès, la valeur de retour est nulle."
  processus * pr = table_des_processus + pid;
  if( pid==0 || pid < 0 || pid >= NBPROC || pr->pid==-1 || pr->etat == ZOMBIE)return -1;
  terminaison_processus(pr);
  return 0;
}

int waitpid(int pid, int *retvalp){ //comprendre comment on accéde au fils, liste et recup leurs infos
  /*
  Si le paramètre pid est négatif, le processus appelant attend qu'un de ses fils, n'importe lequel,
  soit terminé et récupère (le cas échéant) sa valeur de retour dans *retvalp, à moins que retvalp soit nul.
  Cette fonction renvoie une valeur strictement négative si aucun fils n'existe ou sinon le pid de celui dont elle aura récupéré la valeur de retour.

  Pour un fils qui a été terminé par kill, la valeur de retour est nulle.

  Si le paramètre pid est positif,
  le processus appelant attend que son fils ayant ce pid soit terminé ou tué et récupère sa valeur de retour dans *retvalp,
  à moins que retvalp soit nul.
  Cette fonction échoue et renvoie une valeur strictement négative s'il n'existe pas de processus avec ce pid ou si ce n'est pas un fils du processus appelant. En cas de succès, elle retourne la valeur pid.

  Lorsque la valeur de retour d'un fils est récupérée, celui-ci est détruit, et enlevé de la liste des fils.
  */

  processus * pr = table_des_processus + pid_actif, * fils;
  if(queue_empty(&pr->fils)||pid_actif==pid)return -1;

  if(pid<0){//attente de n'importe quel fils
    int pid_fils = -1;

    queue_for_each(fils,&pr->fils,processus,maillon_comme_fils)
      if(fils->etat == ZOMBIE)// si un fils a été terminé (donc à l'état zombie), on le choisi
        { pid_fils = fils->pid; break; }
      else confirme(fils->etat != MORT );// le fils ne peut pas mourrir sans son pere

    if(pid_fils==-1){// pas de fils terminé trouvé : on attend que le premier fils qui se termine

      pr->attend_fils = -2;// -2 pour attendre n'importe quel fils
      definir_etat_processus(pr,BLOQUE_SUR_ATTENTE_D_UN_FILS);
      ordonnancer();
      confirme(pr->etat != BLOQUE_SUR_ATTENTE_D_UN_FILS);
      pid_fils = pr->attend_fils;// recupere le pid du fils qui désactive l'attente
      pr->attend_fils = -1;// plus d'attente de fils

      confirme( pid_fils >=0 && pid_fils < NBPROC );
      fils = table_des_processus + pid_fils;
    }
    confirme( fils->etat == ZOMBIE );
    if(retvalp)*retvalp = fils->retval;
    definir_etat_processus(fils,MORT);
    ordonnancer();
    return pid_fils;

  }else{// attente du fils pid
    if(pid>=NBPROC || (fils = table_des_processus + pid) -> pere != pr)return -1;
    confirme(fils->etat!=MORT);// le fils ne peut pas mourrir sans son pere
    if(fils->etat != ZOMBIE){
      pr->attend_fils = fils->pid;
      definir_etat_processus(pr,BLOQUE_SUR_ATTENTE_D_UN_FILS);
      ordonnancer();
      confirme(pr->etat != BLOQUE_SUR_ATTENTE_D_UN_FILS);
      pr->attend_fils = -1;
      confirme(fils->etat==ZOMBIE);
    }
    if(retvalp)*retvalp = fils->retval;
    definir_etat_processus(fils,MORT);
    ordonnancer();
    return pid;
  }
}


//Si la valeur de pid est invalide, la valeur de retour est strictement négative,
//sinon elle est égale à la priorité du processus identifié par la valeur pid.
int getprio(int pid){
  return ( pid < 0 || pid >= NBPROC || table_des_processus[pid].pid==-1) ? -1 : table_des_processus[pid].prio;
}

//Sors d'un processus normalement et le met dans un etat zombie si le père existe toujours
void exit(int retval) /*__attribute__ ((noreturn)) */{
  processus * pr = table_des_processus + pid_actif;
  pr->retval=retval;
  terminaison_processus(pr);
  confirme(0,"un processus ne doit pas s'executer apres un exit()");
  while(1);
}

// La primitive chprio donne la priorité newprio au processus identifié par la valeur de pid.
//Si la priorité du processus change et qu'il était en attente dans une file, il doit y être replacé selon sa nouvelle priorité.
//Si la valeur de newprio ou de pid est invalide, la valeur de retour de chprio est strictement négative,
//sinon elle est égale à l'ancienne priorité du processus pid.
int chprio(int pid, int newprio){

  processus * pr = table_des_processus + pid;

  if(newprio <= 0 || pid < 0 || pid >= NBPROC || pr->etat == MORT || pr->etat == ZOMBIE )return -1;


  if(pr->prio == newprio)return newprio;

  int oldprio = pr->prio;
  pr->prio = newprio;

  if(pr->etat == ACTIVABLE){// s'il est dans la file des activables il faut mette à jour sa position dans cette file
    queue_del(pr, maillon_comme_activable);
    queue_add(pr, &file_ordonnee_des_processus_activables, processus, maillon_comme_activable, prio);
  }

  if(!queue_empty(&file_ordonnee_des_processus_activables)&&((processus*)queue_top(&file_ordonnee_des_processus_activables, processus, maillon_comme_activable))->prio > table_des_processus[pid_actif].prio)
    ordonnancer();// si un plus prioritaire va prendre la main

  return oldprio;
}

int getvalreset(){
  return table_des_processus[getpid()].valresetfile;

}
void setvalreset(){
  //printf("\n je reset pid : %d \n",getpid());
  table_des_processus[getpid()].valresetfile=0;
}

int getpid(){ // La valeur de retour de getpid est le pid du processus appelant cette primitive.
  return pid_actif;
}
