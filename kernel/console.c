#include "console.h"
#include "cpu.h"


//tampon = (char *)malloc(NBR_MAX_BUFF*sizeof(char));
int echo = 1;
int index = 0;
unsigned long INDEX_FIN_LIGNE;

link file_ordonnee_des_processus_en_attente_de_cons_read;

void init_file_ordonnee_des_processus_en_attente_de_cons_read(){
  INIT_LIST_HEAD(&file_ordonnee_des_processus_en_attente_de_cons_read);
}



void traitant_clavier(void) {
  outb(0x20,0x20); // acquittement de l'interruption
  int scancode = inb(0x60);
  //peut-être intégrer les touches particulières de raccourci
  do_scancode(scancode);

}

void keyboard_data(char *str) {
  //la frappe de backspace provoque la suppression du tampon
  //du dernier caractère entré, sauf si ligne vide

  //if(*str == '\r') *str = '\n';
  //TODO: peut-etre gerer echo
  if(*str ==13){
    if(!queue_empty(&file_ordonnee_des_processus_en_attente_de_cons_read)){
      processus * p = queue_top(&file_ordonnee_des_processus_en_attente_de_cons_read,processus,maillon_comme_attente_cons_read);
      queue_out(&file_ordonnee_des_processus_en_attente_de_cons_read,processus,maillon_comme_attente_cons_read);
      definir_etat_processus(p,ACTIVABLE);
    }
    INDEX_FIN_LIGNE=(unsigned long)index;
    index=0;
  } else{
      if(*str == 127) {
        if(index!=0){
        if(echo==1){

          efface_car();
          console_putbytes("\b",1);}
        /*backspace :--> on supprime le dernier element du tampon*/
        index -= 1; //maj de l'index
      }

      } else if(index < NBR_MAX_BUFF) {
        if(echo == 1) {
          if(*str == 13) {
            printf("\n");
          }
          if(*str == 9 || (*str >= 32 && *str <= 126)) {
            printf("%s",str);
          }
          if(*str == 8) {
            printf("8");
            efface_car();

          }
          if(*str < 32) {//On transforme en majuscule
            *str = *str + 64;
            printf("^%s",str);
          }
          //printf("%s", str);
        }
          /*si on peut toujours mettre des éléments dans le buffer clavier*/
		      tampon[index] = *str;
		      index += 1;
	    }
  }
}

/*
*Gestion des leds
*/
static int console_leds=0;

void console_leds_update(void)
{
    outb(0xed, 0x60);
    outb(console_leds, 0x60);
}

void kbd_leds(unsigned char leds){
    console_leds = leds;
    console_leds_update();
}

/*
*Lecture sur le terminal
*/
unsigned long cons_read(char *string, unsigned long length) {

    if(length>0){
      unsigned long i;
    INDEX_FIN_LIGNE=0;

    /*while(INDEX_FIN_LIGNE==0){
      ssttii();// demasquage des interruptions
      hlt();// pause du systeme
      cli();// masquage des interruptions
    }*/

    queue_add(table_des_processus+pid_actif,&file_ordonnee_des_processus_en_attente_de_cons_read,processus,maillon_comme_attente_cons_read,prio);
    definir_etat_processus(table_des_processus+pid_actif,BLOQUE_SUR_ENTREE_SORTIE);
    ordonnancer();
    confirme( (table_des_processus+pid_actif)->etat != BLOQUE_SUR_ENTREE_SORTIE);

    if(INDEX_FIN_LIGNE>length) INDEX_FIN_LIGNE=length;
    for(i=0; i < INDEX_FIN_LIGNE; i++) {
        string[i] = tampon[i];
    }
    string[i] = '\0';
    return INDEX_FIN_LIGNE;
  }
  return -1;
}

/*
*Affichage sur le terminal
*/
int cons_write(const char *str, long size) {
  console_putbytes(str,size);
  //voir si meme traitement en mode user
  return 0;
}

void cons_echo(int on) {
  echo = on;
}
