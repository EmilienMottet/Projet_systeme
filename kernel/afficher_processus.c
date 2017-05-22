#include "afficher_processus.h"

void afficher_simple_processus(processus*pr){
  confirme(pr>=table_des_processus&&pr<table_des_processus+NBPROC);
  afficher_le_nom_des_processus_activables_et_du_processus_actif();
  printf("\nprocessus '%s' : pid = %d   priorite = %d   etat = ",pr->nom,pr->pid,pr->prio);
  switch (pr->etat) {
    case ACTIF:printf("actif");break;
    case ACTIVABLE:printf("activable");break;
    case BLOQUE_SUR_SEMAPHORE:printf("bloque sur semaphore");break;
    case BLOQUE_SUR_RECEPTION:printf("bloque sur reception");break;
    case BLOQUE_SUR_ENVOI:printf("bloque sur envoi");break;
    case BLOQUE_SUR_ENTREE_SORTIE:printf("bloque sur entre/sortie");break;
    case BLOQUE_SUR_ATTENTE_D_UN_FILS:printf("bloque sur attente d'un fils");break;
    case ENDORMI:printf("emdormi");break;
    case ZOMBIE:printf("zombie");break;
    case MORT:printf("mort");break;
  }
  printf("\n");
}

void afficher_processus(processus*pr){
  confirme(pr>=table_des_processus&&pr<table_des_processus+NBPROC);
  afficher_le_nom_des_processus_activables_et_du_processus_actif();
  afficher_simple_processus(pr);
  int t = pr->temps_reveil;
  printf("\ttemps de reveil : %d secondes %d millisecondes\n",t/1000,t-(t/1000)*1000);
  printf("\tpere : ");if(!pr->pere)printf("aucun\n");else afficher_simple_processus(pr->pere);
  if(queue_empty(&pr->fils)) printf("\tsans fils\n");
  else{
    processus * fils;
    printf("\tfils :\n");
    queue_for_each(fils,&pr->fils,processus,maillon_comme_fils)
      afficher_simple_processus(fils);
    printf("\tfin fils\n");
  }
}

void afficher_le_nom_des_processus_activables_et_du_processus_actif(){
	int ligne=0; char s[300];
  int colonne = 47;
  assert(colonne<48);
  static int ligne_affichage_precedant = 0;
  uint16_t font_precedant = couleur_ecriture_ecrant;
  assigner_couleur_ecriture_ecrant(ORANGE,NOIR);
	affiche_message_ecrant(" ############################### ",ligne++,colonne);
	affiche_message_ecrant(" #>  activables :  <#> prio : <# ",ligne++,colonne);
	processus * pr;
	queue_for_each(pr,&file_ordonnee_des_processus_activables,processus,maillon_comme_activable){
		sprintf(s," #                  #          # ");affiche_message_ecrant(s,ligne,colonne);
    confirme(pr>=table_des_processus&&pr<table_des_processus+NBPROC);// etre sur que tout les processus on été crée
    sprintf(s,"%d",pr->prio);affiche_message_ecrant(s,ligne,colonne+23);
		sprintf(s," #  %s",pr->nom);affiche_message_ecrant(s,ligne++,colonne);
	}
	affiche_message_ecrant(  " ############################### ",ligne++,colonne);
	if(pid_actif==-1){
    affiche_message_ecrant(" #> pid actif = -1 <#    //    # ",ligne++,colonne);
  }else{
		affiche_message_ecrant(" #>     actif :    <#          # ",ligne++,colonne);
		sprintf(s," #                  #          # ");affiche_message_ecrant(s,ligne,colonne);
    sprintf(s,"%d",table_des_processus[pid_actif].prio);affiche_message_ecrant(s,ligne,colonne+23);
		sprintf(s,"%s",table_des_processus[pid_actif].nom);affiche_message_ecrant(s,ligne++,colonne+4);
	}
  affiche_message_ecrant(" ############################### ",ligne++,colonne);
  affiche_message_ecrant(" #>  attend fils   <#          # ",ligne++,colonne);
  for(pr=table_des_processus;pr<table_des_processus+NBPROC;pr++){
    if( pr->pid!=-1 && pr->etat==BLOQUE_SUR_ATTENTE_D_UN_FILS ){
      sprintf(s," #                  #          # ");affiche_message_ecrant(s,ligne,colonne);
      sprintf(s,"%d",pr->prio);affiche_message_ecrant(s,ligne,colonne + 23);
  		sprintf(s," #  %s",pr->nom);affiche_message_ecrant(s,ligne++,colonne);
    }
  }
  affiche_message_ecrant(" ############################### ",ligne++,colonne);
  affiche_message_ecrant(" #>    zombies     <#          # ",ligne++,colonne);
  for(pr=table_des_processus;pr<table_des_processus+NBPROC;pr++){
    if( pr->pid!=-1 && pr->etat==ZOMBIE ){
      sprintf(s," #                  #          # ");affiche_message_ecrant(s,ligne,colonne);
      sprintf(s,"%d",pr->prio);affiche_message_ecrant(s,ligne,colonne + 23);
  		sprintf(s," #  %s",pr->nom);affiche_message_ecrant(s,ligne++,colonne);
    }
  }
  affiche_message_ecrant(" ############################### ",ligne++,colonne);
  couleur_ecriture_ecrant = font_precedant;
  affiche_message_ecrant("                                 ",ligne++,colonne);
  for(int i=ligne;i<ligne_affichage_precedant;i++)
    affiche_message_ecrant("                                 ",i,colonne);
  ligne_affichage_precedant = ligne;
}

void afficher_simple_procesus_existants(){
	// affiche les informations sur les processus existants:
	// pid, état, nom du programme initial lors du lancement du processus ;
	for(int i=0;i<NBPROC;i++){
		processus * p = table_des_processus+i;
		if(p->etat!=MORT){
			confirme(p->pid != -1);
			afficher_simple_processus(p);
		}
	}
}
