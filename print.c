#include "print.h"

/*
* Fonction qui renvoie un pointeur sur la case mémoire correspondant aux données fournies.
*/
uint16_t *ptr_mem(uint32_t lig, uint32_t col) {
  // L'adresse de début de la mémoire vidéo est 0xB8000
  // Chaque case est composée de 2 octets
  uint16_t *p = (uint16_t*)(0xB8000 + 2*((uint16_t)lig*80+(uint16_t)col));
  return p;
}

/*
* Fonction qui écrit le caractère c aux coordonnées spécifiées
*/
void ecrit_car(uint32_t lig, uint32_t col, char c) {
  uint16_t *p;
  // Récupération de la case mémoire correspondante aux coordonnées spécifiées
  p=ptr_mem(lig,col);
  // Stockage dans cette case de c en valeur basse (le premier octet)
  // et en valeur haute : stockage de la couleur de fond (0 pour noir) et de la couleur d'écriture (F pour blanc)
  *p=c | 0x0F00;
}

void efface_ecran(void) {
  for (uint32_t i = 0; i < 25; i++) {
    for (uint32_t j = 0; j < 80; j++) {
      ecrit_car(i,j,' ');
    }
  }
}

/*
* Fonction qui place le curseur à une position donnée
*/
void place_curseur(uint32_t lig, uint32_t col) {
  // Calcul de la position du curseur (un entier sur 16 bits)
  uint16_t pos = (uint16_t)col + 80*(uint16_t)lig;
  // Envoi de 0x0F sur le port de commande pour indiquer à la carte que l'on va envoyer
  // la partie basse de la position du curseur
  outb(0x0F,0x3D4);
  // Envoi de cette partie basse sur le port de données
  outb((uint8_t)pos,0x3D5);
  // Envoi de 0x0E sur le port de commande pour indiquer à la carte que l'on va envoyer
  // la partie haute de la position du curseur
  outb(0x0E,0x3D4);
  // Envoi de cette partie haute sur le port de données
  outb((uint8_t)(pos >> 8),0x3D5);
}
/*
* Fonction qui retourne la position actuelle du curseur
*/
uint16_t position_curseur() {
  uint8_t partie_basse, partie_haute;
  // Envoi de 0x0F sur le port de commande pour indiquer à la carte que l'on va recevoir
  // la partie basse de la position du curseur
  outb(0x0F,0x3D4);
  // Réception de la partie passe du curseur à partir de la lecture du port de données
  partie_basse = inb(0x3D5);
  // Idem pour la partie haute (avec l'envoi de 0x0E cette fois-ci)
  outb(0x0E,0x3D4);
  partie_haute = inb(0x3D5);

  uint16_t pos = (partie_haute<<8) + partie_basse;
  return pos;
}

/*
* Fonction de traitement d'un caractère (affichage si caractère normal
* et mise en place de l'effet si caractère de contrôle)
*/
void traite_car(char c) {

  uint32_t pos = (uint32_t) position_curseur();

  // pos = col + 80*lig donc :
  uint32_t col = pos%80;
  uint32_t lig = pos/80;

  switch (c) {
    //  Cas où c est un caractère de contrôle
    case '\t':
      place_curseur(lig,col+8);
      break;
    case '\n':
      place_curseur(lig+1,0);
      break;
    case '\f':
      efface_ecran();
      place_curseur(0,0);
      break;
    case '\r':
      place_curseur(lig,0);
      break;
    // Cas où c est un caractère normal
    default:
      ecrit_car(lig,col,c);
      break;
  }
}

void defilement(void) {
  // // Version 1
  // uint32_t lig, col;
  //
  // // // JUSTE ?
  // // partie_haute = ((uint16_t)partie_haute)<<8;
  // // pos = partie_haute+partie_basse;
  //
  // uint32_t pos = (uint32_t) position_curseur();
  //
  // col = pos%80;
  // lig=(pos-col)/80;
  //
  // memmove((void*)0xB8000,(void*)0xB80A0,2000*2-80*2);
  // if (lig!=0) {
  //   place_curseur(lig-1,col);
  // }

  uint16_t *adresse_src;
  uint16_t *adresse_dst;

  for (int32_t i = 0; i < 24; i++) {
    for (int32_t j = 0; j < 80; j++) {
      adresse_src = ptr_mem(i+1,j);//la case du dessous relativement à la case du dessus
      adresse_dst = ptr_mem(i,j);//la case du dessus
      memmove(adresse_dst, adresse_src, 2);//la case du haut est remplacée par celle du bas
    }
  }

  // La dernière ligne n'a donc plus de sens, on l'efface
  for (int32_t j = 0; j < 80; j++) {
    ecrit_car(24,j,' ');
  }
}
/*
* Fonction utilisée par printf
*/
void console_putbytes(char *chaine, int32_t taille) {
  uint16_t pos = position_curseur();
  uint32_t lig, col;

  for (int32_t i = 0; i < taille; i++) {

    if (((pos+i) / 80) > 24) { //si lig est une ligne qui sort de l'écran
      defilement(); //défilement
      pos = pos-80; //recalcul de la position
    }
    // mise à jour de col et lig
    col = (pos+i) % 80;
    lig = (pos+i) / 80;

    ///placement du curseur et traitement du caractère
    place_curseur(lig,col);
    traite_car(chaine[i]);
  }
}

/*
* Fonction pour l'affichage de l'heure en haut à droite
*/
void affiche_haut_droite(char* c) {
  for (int32_t i = 0; i < strlen(c); i++) {
    ecrit_car(0, 80-strlen(c)+i, c[i]);
  }
  // ecrit_car(0, 80-strlen(c), (char) c)
  // place_curseur(0,80-strlen(c));
  // printf(c);
}
