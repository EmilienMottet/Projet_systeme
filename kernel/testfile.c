/**
* Ensimag - Projet système
* Copyright (C) 2013 - Damien Dejean <dam.dejean@gmail.com>
*/


#include "test_file.h"

//#define TESTS_NUMBER_FILE    23
#define TESTS_NUMBER_FILE   8

const char *test_files[TESTS_NUMBER_FILE] = {
  "test_file0",
  "test_file1",
  "test_file2",
  "test_file3",
  "test_file4",
  "test_file5",
  "test_file6",
  "test_file7",/*
  "test_file8",
  "test_file9",
  "test_file10",
  "test_file11",
  "test_file12",
  "test_file13",
  "test_file14",
  "test_file15",
  "test_file16",
  "test_file17",
  "test_file18",
  "test_file19",
  "test_file20",
  "test_file21",
  "test_file22",*/
};

int tout_tester_file(void)
{
  int i;
  int pid;
  int ret;

  for (i = 0; i < TESTS_NUMBER_FILE; i++) {
    printf("Test %s : \n", test_files[i]);

    pid = start(test_files[i], 4000, 128, NULL);
    waitpid(pid, &ret);
    assert(ret == 0);
    printf("fin %s.\n", test_files[i]);

  }
  assert(NBFILES==0);
  for(i=0;i<NBQUEUE;i++){
    printf("test reinitialisation file %d\n",i);
    assert ( tab_file_message[i].statut==0);
    assert (tab_file_message[i].nbelem==0);
    assert (tab_file_message[i].file.debut==NULL);
    assert (tab_file_message[i].file.fin==NULL);
    assert (tab_file_message[i].size==0);
    assert (tab_file_message[i].nbproc_recep==0);
    assert (tab_file_message[i].nbproc_envoi==0);
    assert (queue_empty((&tab_file_message[i].liste_proc_bloque_envoi))!=0 );
    assert (queue_empty((&tab_file_message[i].liste_proc_bloque_reception))!=0);
  }
  printf("tous les tests de files sont passes \n ");
  return 0;
}

void idle_test_file(){

  start("tout_tester_file", 4000, 0, NULL);

  printf("attente idle test_files\n");

  for(;;){// ne doit jamais se terminer
    sti();// demasquage des interruptions
    hlt();// pause du systeme
    cli();// masquage des interruptions
  }
}


/*******************************************************************************
* Test 0
*
* test d'envoi et de reception de massages dans une file cree au sein d'un processus
******************************************************************************/

int test_file0()
{
  int message_recu ;
  int fid ;
  int val_retour ;
  fid = pcreate(10);
  for(int i=1; i<11 ; i++){
    val_retour = psend(fid, i);

  }
  for(int i=0; i<10 ; i++){
    val_retour = preceive(fid, &message_recu);

  }
  val_retour=pdelete(fid);


  return val_retour;
}

void init_test_file0(){
  hash_set(&programmes,"test_file0",&test_file0);
}

/*******************************************************************************
* Test 1
*
* test file non cree / num de file invalide
******************************************************************************/

int test_file1()
{
  int message_recu=6 ;
  int fid=0;
  int val_retour ;
  val_retour=psend(fid,message_recu);
  if (val_retour>=0) return -1;
  val_retour=preceive(fid,&message_recu);
  if (val_retour>=0) return -1;
  fid=NBFILES;
  val_retour=psend(fid,message_recu);
  if (val_retour>=0) return -1;
  val_retour=preceive(fid,&message_recu);
  if (val_retour>=0) return -1;
  fid=-1;
  val_retour=psend(fid,message_recu);
  if (val_retour>=0) return -1;
  val_retour=preceive(fid,&message_recu);
  if (val_retour>=0) return -1;
  val_retour=pdelete(fid);
  return 0;

}

void init_test_file1(){
  hash_set(&programmes,"test_file1",&test_file1);
}

int test_file2()
{
  int val_retour,message_recu;
  for (int i=0;i<NBQUEUE;i++){
    val_retour=pcreate(5);
    val_retour=psend(i,i);
    val_retour=psend(i,i);
    val_retour=preceive(i,&message_recu);
    if(message_recu!=i) return -1;
  }

  val_retour=pcreate(5);
  if(val_retour>=0) return -1;
  val_retour=pdelete(3);
  val_retour=pcreate(5);
  if(val_retour!=3) return -1;
  for(int i=0;i<NBQUEUE;i++){
    val_retour=pdelete(i);
  }
  return 0;

}

void init_test_file2(){
  hash_set(&programmes,"test_file2",&test_file2);
}

int test_file3()
{
  int pid1,pid2,ret1,ret2;
  int fid=pcreate(5);

  pid1 = start("test_file3_1", 4000, 128, (void*)fid);
  pid2 = start("test_file3_2", 4000, 128, (void*)fid);
  waitpid(pid1,&ret1);
  waitpid(pid2,&ret2);
  if(ret1 ==0 && ret2 ==0) return 0;
  return -1;
}

int test_file3_1(int fid){
  int message_recu;

  int value=preceive(fid,&message_recu);

  if(message_recu!=5) return -1;
  for(int i=0;i<7;i++){

    value=psend(fid,i);
  }

  return value;
}

  int test_file3_2(int fid){
    int message_recu;

    int value=psend(fid,5);

    for(int i=0;i<6;i++){

      value=preceive(fid,&message_recu);

      //if (message_recu!=i) return -1;
    }
    value=pdelete(fid);

    return value;

}

void init_test_file3(){
  hash_set(&programmes,"test_file3",&test_file3);
  hash_set(&programmes,"test_file3_1",&test_file3_1);
  hash_set(&programmes,"test_file3_2",&test_file3_2);


}


int test_file4()
{
  int pid1,pid2,pid3,pid4,ret1,ret2,ret3,ret4;
  int fid=pcreate(5);

  pid1 = start("test_file4_1", 4000, 128, (void*)fid);
  pid2 = start("test_file4_2", 4000, 128, (void*)fid);
  pid3 = start("test_file4_3", 4000, 128, (void*)fid);
  pid4 = start("test_file4_4", 4000, 128, (void*)fid);
  waitpid(pid1,&ret1);
  waitpid(pid2,&ret2);
  waitpid(pid3,&ret3);
  waitpid(pid4,&ret4);
  pdelete(fid);
  if(ret1 ==0 && ret2 ==0 && ret3==0 && ret4==0) return 0;
  return -1;
}

int test_file4_1(int fid){


  int value;
  for(int i=0;i<6;i++){

    value=psend(fid,i);
  }

  if(value>=0) return -1;
  value=psend(fid,1);


  return value;
}

  int test_file4_2(int fid){

    int i = preset(fid);

    return i ;


pdelete(fid);
}

int test_file4_3(int fid){

  int message_recu,value;
    value=preceive(fid,&message_recu);
    value=preceive(fid,&message_recu);
    if(value<0)
    return 0;
    else return -1;

}

int test_file4_4(int fid){

  return preset(fid);

}

void init_test_file4(){
  hash_set(&programmes,"test_file4",&test_file4);
  hash_set(&programmes,"test_file4_1",&test_file4_1);
  hash_set(&programmes,"test_file4_2",&test_file4_2);
  hash_set(&programmes,"test_file4_3",&test_file4_3);
  hash_set(&programmes,"test_file4_4",&test_file4_4);
}



int test_file5()
{
  int pid1,pid2,pid3,pid4,ret1,ret2,ret3,ret4;
  int fid=pcreate(5);
  int fid2=pcreate(5);

  pid1 = start("test_file5_1", 4000, 128, (void*)fid);
  pid2 = start("test_file5_2", 4000, 128, (void*)fid);
  pid3 = start("test_file5_3", 4000, 128, (void*)fid2);
  pid4 = start("test_file5_4", 4000, 128, (void*)fid2);
  waitpid(pid1,&ret1);
  waitpid(pid2,&ret2);
  waitpid(pid3,&ret3);
  waitpid(pid4,&ret4);
  if(ret1 ==0 && ret2 ==0 && ret3==0 && ret4==0 && tab_file_message[fid].statut==0 && tab_file_message[fid2].statut==0) return 0;
  return -1;
}

int test_file5_1(int fid){


  int value;
  for(int i=0;i<6;i++){

    value=psend(fid,i);
  }
  if(value<0)
  return 0;
  return -1;
}

  int test_file5_2(int fid){


    int i = pdelete(fid);

    return i ;


pdelete(fid);
}

int test_file5_3(int fid){

  int message_recu,value;
    value=preceive(fid,&message_recu);
    if(value<0)
    return 0;
    else return -1;

}

int test_file5_4(int fid){

  return pdelete(fid);

}

void init_test_file5(){
  hash_set(&programmes,"test_file5",&test_file5);
  hash_set(&programmes,"test_file5_1",&test_file5_1);
  hash_set(&programmes,"test_file5_2",&test_file5_2);
  hash_set(&programmes,"test_file5_3",&test_file5_3);
  hash_set(&programmes,"test_file5_4",&test_file5_4);
}

int test_file6()
{int fid;
  for(int i=0;i<NBQUEUE;i++){
    fid=pcreate(5);
  }
  if(fid>=0) return 0;
  return -1;
}
void init_test_file6(){
  hash_set(&programmes,"test_file6",&test_file6);

}

int test_file7()
{

    int fid =pcreate(5);

    if(fid>=0) return -1;

    pdelete(0);
    fid=pcreate(5);

    if(fid!=0) return -1;
    for(int i=0;i<NBQUEUE;i++){
      fid=pdelete(i);

      if(fid!=0) return fid;

    }
    fid=pdelete(3);
    if(fid<0) return 0;
    return -1;
}

void init_test_file7(){
  hash_set(&programmes,"test_file7",&test_file7);

}

///////////////////////////////////////////////////////////////////////////////

void init_tout_les_tests_file(){
  hash_set(&programmes,"idle_test_file",&idle_test_file);
  hash_set(&programmes,"tout_tester_file",&tout_tester_file);
  init_test_file0();
  init_test_file1();
  init_test_file2();
  init_test_file3();
  init_test_file4();
  init_test_file5();
  init_test_file6();
  init_test_file7();

}
