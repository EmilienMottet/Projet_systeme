
#include "print.h"
#include "cpu.h"
#include "horloge.h"
#include "process.h"
#include "init_interruptions.h"

#define QUARTZ 0x1234DD
#define CLOCKFREQ 50 // toutes les 20 ms

static unsigned long nombre_d_interruptions_horloge_depuis_le_demarrage_du_noyau = 0;

void tic_PIT(void){

	nombre_d_interruptions_horloge_depuis_le_demarrage_du_noyau++;
  //signal toutes les 20 ms (50 Hz) si QUARTZ vaut 0x1234DD et CLOCKFREQ vaut 50

	//Pour acquitter une des interruptions qu’on manipule dans ce TP,
  //on doit envoyer la commande sur 8 bits 0x20 sur le port de commande 0x20.
  //Comme on veut le faire en C au tout début de la partie du traitant
  //d’interruption gérant l’affichage de l’horloge,
  //on utilisera le bout de code suivant :



	outb(0x20,0x20);//bon

	static char s[200];

	static int tps = 0;
	tps+=20; // pas exactement
  static int t;t=tps/1000;







	processus * attend;
	parcour_endormis:
	queue_for_each(attend,&file_processus_endomis_sur_wait_clock,processus,maillon_comme_attente_horloge)
		if(attend->wait_ieme_interruption_clock<=nombre_d_interruptions_horloge_depuis_le_demarrage_du_noyau){
			definir_etat_processus(attend,ACTIVABLE);
			goto parcour_endormis;// reparcour car on a supprimé attend de la file que l'on est en train de parcourrir
		}




	if(pid_actif!=-1)
		table_des_processus[pid_actif].temps_reveil += 20;

	ordonnancer();

	//affiche_en_haut_a_droite(s);

	{
		if(t<60)sprintf(s," [ temps systeme : %d secondes ] ",t);
	  else sprintf(s," [ temps systeme : %d minutes %d secondes ] ",t/60,t%60);
		uint16_t font_precedant = couleur_ecriture_ecrant;
		assigner_couleur_ecriture_ecrant(JAUNE,NOIR);

		affiche_en_haut_a_droite(s);

		couleur_ecriture_ecrant = font_precedant;
	}

}

unsigned long current_clock(){
	return nombre_d_interruptions_horloge_depuis_le_demarrage_du_noyau;
}

void clock_settings(unsigned long *quartz, unsigned long *ticks){
	*quartz = QUARTZ;
	*ticks = (unsigned long)( (uint32_t)QUARTZ / (uint32_t)CLOCKFREQ );
}

void wait_clock(unsigned long clock){
	confirme(pid_actif>0);
	processus * actif = table_des_processus + pid_actif;
	actif->wait_ieme_interruption_clock = clock;
	definir_etat_processus(actif,BLOQUE_SUR_SEMAPHORE);
	ordonnancer();
	//while(nombre_d_interruptions_horloge_depuis_le_demarrage_du_noyau<clock)
		//hlt();
	return;
}


void regler_la_frequence_de_l_horloge(){
	//régler la fréquence de l'orloge

	//on envoie la commande sur 8 bits 0x34 sur le port de commande 0x43 cette commande indique à l’horloge que l’on va lui envoyer la valeur de réglage de la fréquence sous la forme de deux valeurs de 8 bits chacunes qui seront émises sur le port de données
	outb(0x34,0x43);

  uint16_t e = (uint16_t)( (uint32_t)QUARTZ / (uint32_t)CLOCKFREQ );
  //NE MARCHE PAS SI ON FAIT : e = (uint16_t)QUARTZ / (uint16_t)CLOCKFREQ

	//on envoie les 8 bits de poids faibles de la valeur de réglage de la fréquence sur le port de données 0x40 : cela peut se faire simplement par
	//outb((QUARTZ / CLOCKFREQ) % 256, 0x40);
  outb(e & 0xffff, 0x40);
	//où QUARTZ vaut 0x1234DD et CLOCKFREQ vaut 50

	//on envoie ensuite les 8 bits de poids forts de la valeur de réglage sur le même port 0x40
	//uint16_t e = (uint16_t)QUARTZ / (uint16_t)CLOCKFREQ;
	outb((uint8_t)(e>>8),0x40);
}
