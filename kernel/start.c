#include "debugger.h"
#include "cpu.h"
#include "print.h"
#include "init_interruptions.h"
#include "test.h"
#include "test_file.h"
#include "console.h"
#include "file.h"
#include "test_file.h"

#include "shell.h"

/*
void * use = 0;

int fonc_test(void*args){
	use = args;
	printf("debut de fonc_test\n");
	while(1);
	printf("fin de fonc_test\n");
	return 0;
}

int test_A(void*args){
	use = args;
	printf("debut test A\n");
	while(1);
	printf("fin test A\n");
	return 0;
}

int test_B(void*args){
	use = args;
	printf("debut test B\n");
	printf("fin test B\n");
	return 0;
}

int test_C(void*args){
	use = args;
	printf("debut test C\n");
	start("test_B", 4000, 0, NULL);
	printf("fin test C\n");
	return 0;
}
*/
void idle_shell(){
  start("shell",4000,1,NULL);
  for(;;){// ne doit jamais se terminer
    sti();// demasquage des interruptions
    hlt();// pause du systeme
    cli();// masquage des interruptions
  }
}
void kernel_start(void)
{
	efface_ecran();
	initialiser_gestion_des_processus();
	initialiser_interruptions();
	//char cmd[100];
	//cons_read(cmd, 99);
	init_tout_les_tests();
	init_tout_les_tests_file();
	init_file_ordonnee_des_processus_en_attente_de_cons_read();

	init_tout_les_tests();
	init_tout_les_tests_file();
	init_shell();
	hash_set(&programmes,"idle_shell",&idle_shell);
	start("idle_shell",4000,0,NULL);


	//start("idle", 4000, 0, NULL);
	//start("idle_test", 4000, 0, NULL);
	//idle_test();
	start("idle_test", 4000, 0, NULL);
	//start("idle_test_file", 4000, 0, NULL); // git add pour ajouter le fichier

	while(1)
	  hlt();

	return;
}
