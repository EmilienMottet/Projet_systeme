/*
 * Ensimag - Projet système
 * Copyright (C) 2013 - Damien Dejean <dam.dejean@gmail.com>
 */
#include "test.h"
#include "init_interruptions.h"
#include "../shared/syscalls.h"
//#define TESTS_NUMBER    23

#define TESTS_NUMBER   17

/* Because malloc.h defines assert as void */
#undef __DEBUG_H__
#undef assert
#include "../shared/debug.h"

const char *tests[TESTS_NUMBER] = {
        "test_wait_clock",
        "test0",
        "test1",
        "test2",
        "test3",
        "test19_clavier",
        "test3_1",
        "test4",
        "test5",
        //"test6",
        //"test7",
        "test8",
        //"test9",
        "test10",
        //"test11",
        "test12",
        //"test13",
        "test14",
        "test15",
        //"test16",
        "test17",
        //"test18",
        "test19_clavier"/*,
        //"test19_lecture_console",
        //"test20", // utilise les semaphores
        "test21",
        "test22",*/
};

int tout_tester(void)
{
        int i;
        int pid;
        int ret;
int j; for(j=0;j<10;j++) {// FIXME Jlc
        for (i = 0; i < TESTS_NUMBER; i++) {
                printf("Test %s : \n", tests[i]);

                pid = start(tests[i], 4000, 128, NULL);
                waitpid(pid, &ret);
                assert(ret == 0);
                printf("fin %s.\n", tests[i]);

        }
}
        return 0;
}

void idle_test(){

  printf("\n");
  start("tout_tester", 4000, 0, NULL);

  printf("attente idle tests\n");

  while(1){
    sti();// demasquage des interruptions
    hlt();// pause du systeme
    cli();// masquage des interruptions
  }
}

/*******************************************************************************
 * test wait clock
 * simple attente de la 100 iéme interruption de l'horloge
 ******************************************************************************/

int test_wait_clock(){
  printf("attente de la 50 ieme interruption de l'horloge ... ");
  wait_clock(50);
  printf("ok.\n");
  return 0;
}

void init_test_clock(){
  hash_set(&programmes,"test_wait_clock",&test_wait_clock);
}

/*******************************************************************************
 * Test 0
 *
 * A simple test that probes a classic system call.
 ******************************************************************************/

int test0(void *arg)
{
        (void)arg;
        register unsigned reg1 = 1u;
        register unsigned reg2 = 0xFFFFFFFFu;
        register unsigned reg3 = 0xBADB00B5u;
        register unsigned reg4 = 0xDEADBEEFu;

        printf("I'm a simple process running ...\n");

        unsigned i;
        for (i = 0; i < 10000000; i++) {
             if (reg1 != 1u || reg2 != 0xFFFFFFFFu || reg3 != 0xBADB00B5u || reg4 != 0xDEADBEEFu) {
                printf(" and I feel bad. Bybye ...\n");
                assert(0);
             }
        }

        printf(" and I'm healthy. Leaving.\n");

        return 0;
}

void init_test0(){
  hash_set(&programmes,"test0",&test0);
}

/*******************************************************************************
 * Test 1
 *
 * Demarrage de processus avec passage de parametre
 * Terminaison normale avec valeur de retour
 * Attente de terminaison (cas fils avant pere et cas pere avant fils)
 ******************************************************************************/

int test1(void *arg)
{
        int pid1;
        int r;
        int rval;

        (void)arg;
        pid1 = start("dummy1", 4000, 192, (void *) DUMMY_VAL);
        assert(pid1 > 0);
        printf(" 2");
        r = waitpid(pid1, &rval);
        assert(r == pid1);
        assert(rval == 3);
        printf(" 3");
        pid1 = start("dummy2", 4000, 100, (void *) (DUMMY_VAL + 1));
        assert(pid1 > 0);
        printf(" 4");
        r = waitpid(pid1, &rval);
        assert(r == pid1);
        assert(rval == 4);
        printf(" 6.\n");
        printf("fin test 1\n");
        return 0;
}

int dummy1(void *arg) {
        printf(" 1");
        assert((int) arg == DUMMY_VAL);
        return 3;
}

int dummy2(void *arg)
{
        printf(" 5");
        assert((int) arg == DUMMY_VAL + 1);
        return 4;
}

void init_test1(){
  hash_set(&programmes,"test1",&test1);
  hash_set(&programmes,"dummy1",&dummy1);
  hash_set(&programmes,"dummy2",&dummy2);
}

/*
 * test2
 */

int procExit(void *args)
{
        printf(" 5");
        exit((int) args);
        assert(0);
        return 0;
}

int procKill(void *args)
{
        printf(" X");
        return (int)args;
}

int test2(void *arg)
{
        int rval;
        int r;
        int pid1;
        int val = 45;

        (void)arg;

        printf("1");
        pid1 = start("procKill", 4000, 100, (void *) val);
        assert(pid1 > 0);
        printf(" 2");
        r = kill(pid1);
        assert(r == 0);
        printf(" 3");
        r = waitpid(pid1, &rval);
        assert(rval == 0);
        assert(r == pid1);
        printf(" 4");
        pid1 = start("procExit", 4000, 192, (void *) val);
        assert(pid1 > 0);
        printf(" 6");
        r = waitpid(pid1, &rval);
        assert(rval == val);
        assert(r == pid1);
        assert(waitpid(getpid(), &rval) < 0);
        printf(" 7.\n");
        printf("fin test2\n");
        return 0;
}

void init_test2(){
  hash_set(&programmes,"test2",&test2);
  hash_set(&programmes,"procKill",&procKill);
  hash_set(&programmes,"procExit",&procExit);
}


/*******************************************************************************
 * Test 3
 *
 ******************************************************************************/


int prio4(void *arg)
{
        /* arg = priority of this proc. */
        int r;

        assert(getprio(getpid()) == (int) arg);
        printf("1");
        r = chprio(getpid(), 64);
        assert(r == (int) arg);
        printf(" 3");
        return 0;
}

int prio5(void *arg)
{
        /* Arg = priority of this proc. */
        int r;

        assert(getprio(getpid()) == (int) arg);
        printf(" 7");
        r = chprio(getpid(), 64);
        assert(r == (int)arg);
        printf("error: I should have been killed\n");
        assert(0);
        return 0;
}

int test3(void *arg)
{
        int pid1;
        int p = 192;
        int r;

        (void)arg;

        assert(getprio(getpid()) == 128);
        printf("prio4\n");
        pid1 = start("prio4", 4000, p, (void *) p);
        assert(pid1 > 0);
        printf(" 2");
        r = chprio(getpid(), 32);
        assert(r == 128);
        printf(" 4");
        r = chprio(getpid(), 128);
        assert(r == 32);
        printf(" 5");
        assert(waitpid(pid1, 0) == pid1);
        printf(" 6");

        assert(getprio(getpid()) == 128);
        printf("prio5\n");
        pid1 = start("prio5", 4000, p, (void *) p);
        assert(pid1 > 0);
        printf(" 8");
        r = kill(pid1);
        assert(r == 0);
        assert(waitpid(pid1, 0) == pid1);
        printf(" 9");
        r = chprio(getpid(), 32);
        assert(r == 128);
        printf(" 10");
        r = chprio(getpid(), 128);
        assert(r == 32);
        printf(" 11.\n");
        return 0;
}

void init_test3(){
  hash_set(&programmes,"test3",&test3);
  hash_set(&programmes,"prio4",&prio4);
  hash_set(&programmes,"prio5",&prio5);
}

/*******************************************************************************
 * Test 3_1
 * test de waitpid avec parametre négatif
 ******************************************************************************/

int proc_test_3_1_1(void*arg){assert((int)arg==1);return 1;}
int proc_test_3_1_2(void*arg){assert((int)arg==2);return 2;}
int proc_test_3_1_3(void*arg){assert((int)arg==3);return 3;}

int test3_1(){
  int r = 0;
  int ok[4] = {0,0,0,0};
  assert(getprio(getpid()) == 128);
  int pid1 = start("proc_test_3_1_1",4000,120,(void*)1);
  int pid2 = start("proc_test_3_1_2",4000,120,(void*)2);
  int pid3 = start("proc_test_3_1_3",4000,120,(void*)3);

  for(int i=0;i<3;i++){
    int pid = waitpid(-1,&r);
    assert(pid == pid1 || pid == pid2 || pid == pid3);
    assert(r>0 && r<4 && !ok[r]);
    ok[r] = 1;
  }
  assert(waitpid(-1,&r) < 0);

  assert(getprio(getpid()) == 128);

  printf("ok.\n");

  return 0;
}

void init_test3_1(){
  hash_set(&programmes,"test3_1",&test3_1);
  hash_set(&programmes,"proc_test_3_1_1",&proc_test_3_1_1);
  hash_set(&programmes,"proc_test_3_1_2",&proc_test_3_1_2);
  hash_set(&programmes,"proc_test_3_1_3",&proc_test_3_1_3);
}

/*******************************************************************************
 * Test 4
 *
 * Boucles d'attente active (partage de temps)
 * chprio()
 * kill() de processus de faible prio
 * kill() de processus deja mort
 ******************************************************************************/

 /*******************************************************************************
  * Unmask interrupts for those who are working in kernel mode
  ******************************************************************************/
 void test_it(void)
 {
 #ifdef microblaze
         int status, mstatus;
         __asm__ volatile("mfs %0,rmsr; ori %1,%0,2; mts rmsr,%1; nop; nop; mts rmsr,%0":"=r" (status), "=r" (mstatus));
 #else
         __asm__ volatile("pushfl; testl $0x200,(%%esp); jnz 0f; sti; nop; cli; 0: addl $4,%%esp\n":::"memory");
 #endif
 }

int busy1(void *arg)
{
        (void)arg;
        while (1) {
                int i, j;

                printf(" A");
                for (i=0; i<loop_count1; i++) {
                        test_it();
                        for (j=0; j<loop_count0; j++);
                }
        }
        return 0;
}

/* assume the process to suspend has a priority == 64 */
int busy2(void *arg)
{
        int i;

        for (i = 0; i < 3; i++) {
                int k, j;

                printf(" B");
                for (k=0; k<loop_count1; k++) {
                        test_it();
                        for (j=0; j<loop_count0; j++);
                }
        }
        i = chprio((int) arg, 16);
        assert(i == 64);
        return 0;
}

int test4(void *args)
{
        int pid1, pid2;
        int r;
        int arg = 0;

        (void)args;

        assert(getprio(getpid()) == 128);
        pid1 = start("busy1", 4000, 64, (void *) arg);
        assert(pid1 > 0);
        pid2 = start("busy2", 4000, 64, (void *) pid1);
        assert(pid2 > 0);
        printf("1 -");
        r = chprio(getpid(), 32);
        assert(r == 128);
        printf(" - 2");
        r = kill(pid1);
        assert(r == 0);
        assert(waitpid(pid1, 0) == pid1);
        assert(table_des_processus[pid2].etat == ZOMBIE);
        r = kill(pid2);
        assert(r < 0); /* kill d'un processus zombie */
        assert(waitpid(pid2, 0) == pid2);
        printf(" 3");
        r = chprio(getpid(), 128);
        assert(r == 32);
        printf(" 4.\n");

        return 0;
}

void init_test4(){
  hash_set(&programmes,"busy1",&busy1);
  hash_set(&programmes,"busy2",&busy2);
  hash_set(&programmes,"test4",&test4);
}

/*******************************************************************************
 * Test 5
 *
 * Tests de quelques parametres invalides.
 * Certaines interdictions ne sont peut-etre pas dans la spec. Changez les pour
 * faire passer le test correctement.
 ******************************************************************************/

int no_run(void *arg)
{
        (void)arg;
        assert(0);
        return 1;
}

int waiter(void *arg)
{
        int pid = (int)arg;
        assert(kill(pid) == 0);
        assert(waitpid(pid, 0) < 0);
        return 1;
}

int test5(void *arg)
{
        int pid1, pid2;
        int r = -666;

        (void)arg;

        // Le processus 0 et la priorite 0 sont des parametres invalides
        assert(kill(0) < 0);
        assert(chprio(getpid(), 0) < 0);
        assert(getprio(getpid()) == 128);
        pid1 = start("no_run", 4000, 64, 0);
        assert(pid1 > 0);
        printf(" 1");
        assert(kill(pid1) == 0);
        assert(kill(pid1) < 0); //pas de kill de zombie
        printf(" 2");
        assert(chprio(pid1, 128) < 0); //changer la priorite d'un zombie
        assert(chprio(pid1, 64) < 0); //changer la priorite d'un zombie
        printf(" 3");
        assert(waitpid(pid1, 0) == pid1);
        assert(waitpid(pid1, 0) < 0);
        printf(" 4");
        pid1 = start("no_run", 4000, 64, 0);
        assert(pid1 > 0);
        pid2 = start("waiter", 4000, 65, (void *)pid1);
        assert(pid2 > 0);
        printf(" 5");
        assert(waitpid(pid2, &r) == pid2);
        assert(r == 1);
        printf(" 6");
        assert(waitpid(pid1, &r) == pid1);
        assert(r == 0);
        printf(" ok.\n");
        return 0;
}

void init_test5(){
  hash_set(&programmes,"no_run",&no_run);
  hash_set(&programmes,"waiter",&waiter);
  hash_set(&programmes,"test5",&test5);
}

/*******************************************************************************
 * Test 6
 *
 * Waitpid multiple.
 * Creation de processus avec differentes tailles de piles.
 *******************************************************************************/
 #if defined microblaze
 __asm__(
 ".text\n"
 ".globl proc6_1\n"
 "proc6_1:\n"
 "addik r3,r0,3\n"
 "rtsd r15,8\n"
 "nop\n"
 ".previous\n"
 );
 #else
 __asm__(
 ".text\n"
 ".globl proc6_1\n"
 "proc6_1:\n"
 "movl $3,%eax\n"
 "ret\n"
 ".previous\n"
 );
 #endif

 #if defined microblaze
 __asm__(
 ".text\n"
 ".globl proc6_2\n"
 "proc6_2:\n"
 "addk r3,r0,r5\n"
 "swi r3,r1,-4\n"
 "rtsd r15,8\n"
 "nop\n"
 ".previous\n"
 );
 #else
 __asm__(
 ".text\n"
 ".globl proc6_2\n"
 "proc6_2:\n"
 "movl 4(%esp),%eax\n"
 "pushl %eax\n"
 "popl %eax\n"
 "ret\n"
 ".previous\n"
 );
 #endif

 #if defined microblaze
 __asm__(
 ".text\n"
 ".globl proc6_3\n"
 "proc6_3:\n"
 "addk r3,r0,r5\n"
 "swi r3,r1,-4\n"
 "rtsd r15,8\n"
 "nop\n"
 ".previous\n"
 );
 #else
 __asm__(
 ".text\n"
 ".globl proc6_3\n"
 "proc6_3:\n"
 "movl 4(%esp),%eax\n"
 "pushl %eax\n"
 "popl %eax\n"
 "ret\n"
 ".previous\n"
 );
 #endif

int test6(void *arg)
{
        int pid1, pid2, pid3;
        int ret;

        (void)arg;

        assert(getprio(getpid()) == 128);
        pid1 = start("proc6_1", 0, 64, 0);
        assert(pid1 > 0);
        pid2 = start("proc6_2", 4, 66, (void*)4);
        assert(pid2 > 0);
        pid3 = start("proc6_3", 0xffffffff, 65, (void*)5);
        assert(pid3 < 0);
        pid3 = start("proc6_3", 8, 65, (void*)5);
        assert(pid3 > 0);
        assert(waitpid(-1, &ret) == pid2);
        assert(ret == 4);
        assert(waitpid(-1, &ret) == pid3);
        assert(ret == 5);
        assert(waitpid(-1, &ret) == pid1);
        assert(ret == 3);
        assert(waitpid(pid1, 0) < 0);
        assert(waitpid(-1, 0) < 0);
        assert(waitpid(getpid(), 0) < 0);
        printf("ok.\n");
        return 0;
}

int proc6_1(void *arg);
int proc6_2(void *arg);
int proc6_3(void *arg);

void init_test6(){
  hash_set(&programmes,"proc6_1",&proc6_1);
  hash_set(&programmes,"proc6_2",&proc6_2);
  hash_set(&programmes,"proc6_3",&proc6_3);
  hash_set(&programmes,"test6",test6);
}

/*******************************************************************************
 * Test 7
 *
 * Test de l'horloge (ARR et ACE)
 * Tentative de determination de la frequence du processeur et de la
 * periode de scheduling
 ******************************************************************************/
/*necessite wait_clock , (test_it), current_clock, shm_release, ...
#ifdef TELECOM_TST
int test7(void *arg)
{
        (void)arg;
        printf("Test desactive pour les TELECOM.\n");
}

void init_test7(){
  hash_set(&programmes,"test7",test7);
}

#else

int sleep_pr1(void *arg)
{
        (void)arg;
        wait_clock(current_clock() + 2);
        printf(" not killed !!!");
        assert(0);
        return 1;
}

int timer(void *arg)
{
        volatile unsigned long *timer = NULL;
        timer = shm_acquire("test7_shm");
        assert(timer != NULL);

        (void)arg;
        while (1) {
                unsigned long t = *timer + 1;
                *timer = t;
                while (*timer == t) test_it();
        }
        while (1);
        return 0;
}

int timer1(void *arg)
{
        (void)arg;

        unsigned long quartz;
        unsigned long ticks;
        unsigned long dur;
        int i;

        clock_settings(&quartz, &ticks);
        dur = (quartz + ticks) / ticks;
        printf(" 2");
        for (i = 4; i < 8; i++) {
                wait_clock(current_clock() + dur);
                printf(" %d", i);
        }
        return 0;
}

int test7(void *arg)
{
        int pid1, pid2, r;
        unsigned long c0, c, quartz, ticks, dur;
        volatile unsigned long *timer = NULL;

        (void)arg;
        timer = shm_create("test7_shm");
        assert(timer != NULL);

        assert(getprio(getpid()) == 128);
        printf("1");
        pid1 = start("timer1", 4000, 129, 0);
        assert(pid1 > 0);
        printf(" 3");
        assert(waitpid(-1, 0) == pid1);
        printf(" 8 : ");

        *timer = 0;
        pid1 = start("timer", 4000, 127, 0);
        pid2 = start("timer", 4000, 127, 0);
        assert(pid1 > 0);
        assert(pid2 > 0);
        clock_settings(&quartz, &ticks);
        dur = 2 * quartz / ticks;
        test_it();
        c0 = current_clock();
        do {
                test_it();
                c = current_clock();
        } while (c == c0);
        wait_clock(c + dur);
        assert(kill(pid1) == 0);
        assert(waitpid(pid1, 0) == pid1);
        assert(kill(pid2) == 0);
        assert(waitpid(pid2, 0) == pid2);
        printf("%lu changements de contexte sur %lu tops d'horloge", *timer, dur);
        pid1 = start("sleep_pr1", 4000, 192, 0);
        assert(pid1 > 0);
        assert(kill(pid1) == 0);
        assert(waitpid(pid1, &r) == pid1);
        assert(r == 0);
        printf(".\n");
        shm_release("test7_shm");
}

void init_test7(){
  hash_set(&programmes,"sleep_pr1",&sleep_pr1);
  hash_set(&programmes,"timer",&timer);
  hash_set(&programmes,"timer1",&timer1);
  hash_set(&programmes,"test7",test7);
}

#endif
*/


/*******************************************************************************
 * Test 8
 *
 * Creation de processus se suicidant en boucle. Test de la vitesse de creation
 * de processus.
 ******************************************************************************/

int suicide_launcher(void *arg)
{
	int pid1;
        (void)arg;
	pid1 = start("suicide", 4000, 192, 0);
	assert(pid1 > 0);
	return pid1;
}

int suicide(void *arg)
{
        (void)arg;
        kill(getpid());
        assert(0);
        return 0;
}

int test8(void *arg)
{
        unsigned long long tsc1;
        unsigned long long tsc2;
        int i, r, pid, count;

        (void)arg;
        assert(getprio(getpid()) == 128);

        /* Le petit-fils va passer zombie avant le fils mais ne pas
           etre attendu par waitpid. Un nettoyage automatique doit etre
           fait. */
        pid = start("suicide_launcher", 4000, 129, 0);
        assert(pid > 0);
        assert(waitpid(pid, &r) == pid);
        assert(chprio(r, 192) < 0);

        count = 0;

        __asm__ __volatile__("rdtsc":"=A"(tsc1));
        do {
                for (i=0; i<10; i++) {
                        pid = start("suicide_launcher", 4000, 200, 0);
                        assert(pid > 0);
                        assert(waitpid(pid, 0) == pid);
                }
                //test_it();
                count += i;
                __asm__ __volatile__("rdtsc":"=A"(tsc2));
        } while ((tsc2 - tsc1) < 1000000000);
        printf("%lu cycles/process.\n", (unsigned long)div64(tsc2 - tsc1, 2 * (unsigned)count, 0));
        return 0;
}

void init_test8(){
  hash_set(&programmes,"suicide_launcher",&suicide_launcher);
  hash_set(&programmes,"suicide",&suicide);
  hash_set(&programmes,"test8",&test8);
}

/*******************************************************************************
 * Test 9
 *
 * Test de la sauvegarde des registres dans les appels systeme et interruptions
 ******************************************************************************/
/*

int nothing(void *arg)
{
        (void)arg;
        return 0;
}

static unsigned eax = 0xBADB00B5;
static unsigned *it_ok = NULL;

void __test_valid_eax(unsigned a1)
{
        __asm__ __volatile__(
        "   pushl %%eax                 \n" // Sauver %eax
        "   movl  %1,       %%eax       \n" // Attendre un scheduling, ie que *it_ok vaille 1
        "0: testl $1,       %2          \n"
        "   jz    0b                    \n"
        "   movl  %%eax,    %0          \n" // Récupérer la valeur d'%eax après interruption
        "   popl  %%eax                 \n" // Restaurer %eax
        : "=m" (eax)
        : "m" (a1), "m" (*it_ok)
        : "%eax", "memory"
        );
        // %eax doit avoir conservé sa valeur avant interruption !
        assert(eax == a1);
}

int test_eax(void *arg)
{
        (void)arg;
        it_ok = (unsigned*) shm_acquire("test9_shm");
        *it_ok = 0x0u;
        __test_valid_eax(rand());
        shm_release("test9_shm");
        return 0;

}

necessite shm_create at shm_release

static unsigned ebx = 1;
static unsigned ecx = 2;
static unsigned edx = 3;
static unsigned edi = 4;
static unsigned esi = 5;
static unsigned ebp_before = 0x12345678u;
static unsigned esp_before = 0x98765432u;
static unsigned ebp_after = 0xCAFE0101u;
static unsigned esp_after = 0x1010CAFEu;

static volatile unsigned *it_ok = NULL;

void __test_valid_regs2(unsigned a1,
                        unsigned a2,
                        unsigned a3,
                        unsigned a4,
                        unsigned a5)
{
        // Initialise les registres avec des valeurs spéciales
        __asm__ __volatile__(
        "movl %2,       %%ebx    \n"
        "movl %3,       %%ecx    \n"
        "movl %4,       %%edx    \n"
        "movl %5,       %%edi    \n"
        "movl %6,       %%esi    \n"
        //Sauve le "stack pointer" et le "frame pointer"
        "movl %%ebp,     %0      \n"
        "movl %%esp,     %1      \n"
        : "=m" (ebp_before), "=m" (esp_before)
        : "m" (a1), "m" (a2), "m" (a3), "m" (a4), "m" (a5)
        : "%ebx","%ecx", "%edx", "%edi", "%esi", "memory"
        );

        // Attendre au moins un scheduling (ie des interruptions)
        while (*it_ok == 0);

        // Sauver les valeurs des registres
        __asm__ __volatile__(
        "movl %%ebp,    %0      \n"
        "movl %%esp,    %1      \n"
        "movl %%ebx,    %2      \n"
        "movl %%ecx,    %3      \n"
        "movl %%edx,    %4      \n"
        "movl %%edi,    %5      \n"
        "movl %%esi,    %6      \n"
        : "=m" (ebp_after), "=m" (esp_after), "=m" (ebx), "=m" (ecx), "=m" (edx), "=m" (edi), "=m" (esi)
        : // No input registers
        : "memory"
        );

        // Controler la validite des registres sauves
        assert(ebp_before == ebp_after);
        assert(esp_before == esp_after);
        assert(ebx == a1);
        assert(ecx == a2);
        assert(edx == a3);
        assert(edi == a4);
        assert(esi == a5);
}

int test_regs2(void *arg)
{
        (void)arg;
        it_ok = (unsigned*) shm_acquire("test9_shm");
        *it_ok = 0x0u;
        __test_valid_regs2(rand(), rand(), rand(), rand(), rand());
        shm_release("test9_shm");
        return 0;
}

static unsigned int ebp_before = 0x12345678u;
static unsigned int esp_before = 0x87654321u;
static unsigned int ebp_after  = 0xDEADBEEFu;
static unsigned int esp_after  = 0xDEADFACEu;
static unsigned int eax        = 0xBADB00B5u;
static unsigned int ebx        = 0xF0F0F0F0u;
static unsigned int edi        = 0x0F0F0F0Fu;
static unsigned int esi        = 0xABCDEFABu;

__asm__(
"       .data                   \n"
"nothing:                       \n"
"       .string \"nothing\"     \n"
"       .previous               \n"
);

void __test_valid_regs1(unsigned a1, unsigned a2, unsigned a3)
{
        __asm__ __volatile__(
        // Assigner des valeurs connues aux registres
        "movl   %8,     %%ebx           \n"
        "movl   %9,     %%edi           \n"
        "movl   %10,    %%esi           \n"
        "movl   %%ebp,  %0              \n"
        "movl   %%esp,  %1              \n"

        // Démarrer le processus "nothing"
        "pushl  $0                      \n"
        "pushl  $192                    \n"
        "pushl  $4000                   \n"
        "pushl  $nothing                \n"
        "call   start                   \n"
        "addl   $16,    %%esp           \n"
        "movl   %%eax,  %2              \n"

        // Sauver les registres
        "movl %%ebx,    %3              \n"
        "movl %%edi,    %4              \n"
        "movl %%esi,    %5              \n"
        "movl %%ebp,    %6              \n"
        "movl %%esp,    %7              \n"

        // Registres de sortie
        : "=m" (ebp_before), "=m" (esp_before),
          "=m" (eax),        "=m" (ebx),
          "=m" (edi),        "=m" (esi),
          "=m" (ebp_after),  "=m" (esp_after)
        // Registres en entrée
        : "m" (a1),
          "m" (a2),
          "m" (a3)
        // Registres utilisés par ce bloc ASM
        : "eax", "ebx", "edi", "esi", "memory"
        );

        // On attend le processus nothing dont le pid est dans eax
        assert(waitpid((int)eax, NULL) == (int)eax);

        // Vérifier les valeurs des registres après l'appel
        assert(ebx == a1);
        assert(edi == a2);
        assert(esi == a3);
        assert(ebp_before == ebp_after);
        assert(esp_before == esp_after);
}

int test9(void *arg)
{
        int i;
        int pid;
        volatile unsigned *it_ok = NULL;

        (void)arg;
        it_ok = (unsigned*) shm_create("test9_shm");
        assert(it_ok != NULL);
        assert(getprio(getpid()) == 128);
        printf("1");

        for (i = 0; i < 1000; i++) {
                __test_valid_regs1(rand(), rand(), rand());
        }
        printf(" 2");

        // Test de la cohérence de tous les registres
        for (i = 0; i < 100; i++) {
                *it_ok = 1;
                pid = start("test_regs2", 4000, 128, 0);
                assert(pid > 0);
                while (*it_ok != 0);
                *it_ok = 1;
                assert(waitpid(pid, 0) == pid);
        }
        printf(" 3");

        // Test de la cohérence de %eax
        for (i = 0; i < 100; i++) {
                *it_ok = 1;
                pid = start("test_eax", 4000, 128, 0);
                assert(pid > 0);
                while (*it_ok != 0);
                *it_ok = 1;
                assert(waitpid(pid, 0) == pid);
        }
        printf(" 4.\n");

        shm_release("test9_shm");
        return 0;
}
*/


/*******************************************************************************
 * Test 10
 * Test l'utilisation des semaphores ou des files de messages selon le sujet.
 *******************************************************************************/

 ///////////////////
 //#define WITH_SEM
 #define WITH_MSG
 ///////////////////

 #if defined WITH_SEM
 // Test d'utilisation d'un semaphore comme simple compteur.
 int test10(void *arg)
 {
         int sem1;
         (void)arg;
         sem1 = screate(2);
         assert(sem1 >= 0);
         assert(scount(sem1) == 2);
         assert(signal(sem1) == 0);
         assert(scount(sem1) == 3);
         assert(signaln(sem1, 2) == 0);
         assert(scount(sem1) == 5);
         assert(wait(sem1) == 0);
         assert(scount(sem1) == 4);
         assert(sreset(sem1, 7) == 0);
         assert(scount(sem1) == 7);
         assert(sdelete(sem1) == 0);
         printf("ok.\n");
         return 0;
 }

 #elif defined WITH_MSG
 // Test d'utilisation d'une file comme espace de stockage temporaire.

 static void write(int fid, const char *buf, unsigned long len)
 {
         unsigned long i;
         for (i=0; i<len; i++) {
                 assert(psend(fid, buf[i]) == 0);
         }
 }

 static void read(int fid, char *buf, unsigned long len)
 {
         unsigned long i;
         for (i=0; i<len; i++) {
                 int msg;
                 assert(preceive(fid, &msg) == 0);
                 buf[i] = (char)msg;
         }
 }
 int test10(void *arg)
 {
         int fid;
         const char *str = "abcde";
         unsigned long len = strlen(str);
         char buf[10];

         (void)arg;

         printf("1");
         assert((fid = pcreate(5)) >= 0);
         write(fid, str, len);
         printf(" 2");
         read(fid, buf, len);
         buf[len] = 0;
         assert(strcmp(str, buf) == 0);
         assert(pdelete(fid) == 0);
         printf(" 3.\n");
         return 0;
 }

 #else
 # error "WITH_SEM" ou "WITH_MSG" doit être définie.
 #endif

void init_test10(){
  hash_set(&programmes,"test10",&test10);
}

/*******************************************************************************
 * Test 11
 *
 * Mutex avec un semaphore, regle de priorite sur le mutex.
 ******************************************************************************/

/*necessite shm_acquire

#include "xlock.h"

 int proc_mutex(void *arg)
 {
         struct test11_shared *shared = NULL;
         int p = -1;
         int msg;

         (void)arg;
         shared = (struct test11_shared*) shm_acquire("test11_shm");
         assert(shared != NULL);
         p = getprio(getpid());
         assert(p > 0);

         switch (p) {
                 case 130:
                         msg = 2;
                         break;
                 case 132:
                         msg = 3;
                         break;
                 case 131:
                         msg = 4;
                         break;
                 case 129:
                         msg = 5;
                         break;
                 default:
                         msg = 15;
         }
         printf(" %d", msg);
         xwait(&shared->sem);
         printf(" %d", 139 - p);
         assert(!(shared->in_mutex++));
         chprio(getpid(), 16);
         chprio(getpid(), p);
         shared->in_mutex--;
         xsignal(&shared->sem);
         return 0;

 }

 struct test11_shared {
         union sem sem;
         int in_mutex;
 };

 int test11(void *arg)
 {
         struct test11_shared *shared = NULL;
         int pid1, pid2, pid3, pid4;

         (void)arg;
         shared = (struct test11_shared*) shm_create("test11_shm");
         assert(shared != NULL);
         assert(getprio(getpid()) == 128);
         xscreate(&shared->sem);
         shared->in_mutex = 0;
         printf("1");

         pid1 = start("proc_mutex", 4000, 130, 0);
         pid2 = start("proc_mutex", 4000, 132, 0);
         pid3 = start("proc_mutex", 4000, 131, 0);
         pid4 = start("proc_mutex", 4000, 129, 0);
         assert(pid1 > 0);
         assert(pid2 > 0);
         assert(pid3 > 0);
         assert(pid4 > 0);
         assert(chprio(getpid(), 160) == 128);
         printf(" 6");
         xsignal(&shared->sem);
         assert(waitpid(-1, 0) == pid2);
         assert(waitpid(-1, 0) == pid3);
         assert(waitpid(-1, 0) == pid1);
         assert(waitpid(-1, 0) == pid4);
         assert(waitpid(-1, 0) < 0);
         assert(chprio(getpid(), 128) == 160);
         xsdelete(&shared->sem);
         printf(" 11.\n");

         return 0;
 }

void init_test11(){
  hash_set(&programmes,"test11",&test11);
}
*/

/*******************************************************************************
 * Test 12
 *
 * Tests de rendez-vous sur une file de taille 1.
 ******************************************************************************/

 int rdv_proc(void *arg)
 {
         int fid = (int) arg;
         int msg;
         int count;

         printf(" 2");
         assert(psend(fid, 3) == 0); /* Depose dans le tampon */
         printf(" 3");
         assert((pcount(fid, &count) == 0) && (count == 1));
         assert(psend(fid, 4) == 0); /* Bloque tampon plein */
         printf(" 5");
         assert((pcount(fid, &count) == 0) && (count == 1));
         assert(preceive(fid, &msg) == 0); /* Retire du tampon */
         assert(msg == 4);
         printf(" 6");
         assert(preceive(fid, &msg) == 0); /* Bloque tampon vide. */
         assert(msg == 5);
         printf(" 8");
         assert((pcount(fid, &count) == 0) && (count == 0));
         return 0;
 }

 int test12(void *arg)
 {
         int fid;
         int pid;
         int msg;
         int count;

         (void)arg;

         assert(getprio(getpid()) == 128);
         assert((fid = pcreate(1)) >= 0);
         printf("1");
         pid = start("rdv_proc", 4000, 130, (void *)fid);
         assert(pid > 0);
         printf(" 4");
         assert((pcount(fid, &count) == 0) && (count == 2));
         assert(preceive(fid, &msg) == 0); /* Retire du tampon et debloque un emetteur. */
         assert(msg == 3);
         printf(" 7");
         assert((pcount(fid, &count) == 0) && (count == -1));
         assert(psend(fid, 5) == 0); /* Pose dans le tampon. */
         printf(" 9");
         assert(psend(fid, 6) == 0); /* Pose dans le tampon. */
         assert(preceive(fid, &msg) == 0); /* Retire du tampon. */
         assert(msg == 6);
         assert(pdelete(fid) == 0);
         assert(psend(fid, 2) < 0);
         assert(preceive(fid, &msg) < 0);
         assert(waitpid(-1, 0) == pid);
         printf(" 10.\n");
         return 0;
 }

void init_test12(){
  hash_set(&programmes,"test12",&test12);
  hash_set(&programmes,"rdv_proc",&rdv_proc);
}

 /*******************************************************************************
  * Test 13
  *
  * Teste l'ordre entre les processus emetteurs et recepteurs sur une file.
  * Teste le changement de priorite d'un processus bloque sur une file.
  ******************************************************************************/
/*
necessite shm_create

int psender(void *arg)
{
        struct psender *ps = NULL;
        int ps_index = (int)arg;
        unsigned i;
        unsigned n;

        ps = shm_acquire("test13_shm");
        assert(ps != NULL);
        n = strlen(ps[ps_index].data);

        for(i = 0; i < n; i++) {
                assert(psend(ps[ps_index].fid, ps[ps_index].data[i]) == 0);
        }
        shm_release("test13_shm");
        return 0;
}

int preceiver(void *arg)
{
        struct psender *ps = NULL;
        int ps_index = (int)arg;
        int msg;
        unsigned i;
        unsigned n;

        ps = shm_acquire("test13_shm");
        assert(ps != NULL);
        n = strlen(ps[ps_index].data);

        for(i = 0; i < n; i++) {
                assert(preceive(ps[ps_index].fid, &msg) == 0);
                assert(msg == ps[ps_index].data[i]);
        }

        shm_release("test13_shm");
        return 0;
}

int test13(void *arg)
{
        struct psender *ps = NULL;
        int pid1, pid2, pid3;
        int fid = pcreate(3);
        int i, msg;

        (void)arg;
        ps = (struct psender*) shm_create("test13_shm");
        assert(ps != NULL);

        printf("1");
        assert(getprio(getpid()) == 128);
        assert(fid >= 0);
        ps[1].fid = ps[2].fid = ps[3].fid = fid;
        strncpy(ps[1].data, "abcdehm", 32);
        strncpy(ps[2].data, "il", 32);
        strncpy(ps[3].data, "fgjk", 32);
        pid1 = start("psender", 4000, 131, (void*)1);
        pid2 = start("psender", 4000, 130, (void*)2);
        pid3 = start("psender", 4000, 129, (void*)3);
        for (i=0; i<2; i++) {
                assert(preceive(fid, &msg) == 0);
                assert(msg == 'a' + i);
        }
        chprio(pid1, 129);
        chprio(pid3, 131);
        for (i=0; i<2; i++) {
                assert(preceive(fid, &msg) == 0);
                assert(msg == 'c' + i);
        }
        chprio(pid1, 127);
        chprio(pid2, 126);
        chprio(pid3, 125);
        for (i=0; i<6; i++) {
                assert(preceive(fid, &msg) == 0);
                assert(msg == 'e' + i);
        }
        chprio(pid1, 125);
        chprio(pid3, 127);
        for (i=0; i<3; i++) {
                assert(preceive(fid, &msg) == 0);
                assert(msg == 'k' + i);
        }
        assert(waitpid(pid3, 0) == pid3); //XXX assert(waitpid(-1, 0) == pid3); ???
        assert(waitpid(-1, 0) == pid2);
        assert(waitpid(-1, 0) == pid1);
        printf(" 2");

        strncpy(ps[1].data, "abej", 32);
        strncpy(ps[2].data, "fi", 32);
        strncpy(ps[3].data, "cdgh", 32);
        pid1 = start("preceiver", 4000, 131, (void*)1);
        pid2 = start("preceiver", 4000, 130, (void*)2);
        pid3 = start("preceiver", 4000, 129, (void*)3);
        for (i='a'; i<='b'; i++) {
                assert(psend(fid, i) == 0);
        }
        chprio(pid1, 129);
        chprio(pid3, 131);
        for (i='c'; i<='d'; i++) {
                assert(psend(fid, i) == 0);
        }
        chprio(pid1, 127);
        chprio(pid2, 126);
        chprio(pid3, 125);
        for (i='e'; i<='j'; i++) {
                assert(psend(fid, i) == 0);
        }
        chprio(pid1, 125);
        chprio(pid3, 127);
        assert(waitpid(-1, 0) == pid3);
        assert(waitpid(-1, 0) == pid2);
        assert(waitpid(-1, 0) == pid1);
        assert(pdelete(fid) == 0);
        printf(" 3.\n");
        shm_release("test13_shm");
        return 0;
}

void init_test13(){
  hash_set(&programmes,"psender",&psender);
  hash_set(&programmes,"preceiver",&preceiver);
  hash_set(&programmes,"test13",&test13);
}
*/

/*******************************************************************************
 * Test 14
 *
 * Tests de preset et pdelete
 ******************************************************************************/

 int psender1(void *arg)
 {
         int fid1 = (int)arg;
         int fid2;
         int msg;

         printf(" 2");
         assert(preceive(fid1, &fid2) == 0);
         assert(psend(fid1, fid2) == 0);
         fid2 -= 42;
         assert(psend(fid1, 1) == 0);
         assert(psend(fid1, 2) == 0);
         assert(psend(fid1, 3) == 0);
         assert(psend(fid1, 4) == 0);
         assert(psend(fid1, 5) < 0);

         //printf("\n valeur reset : %d pid du processus actif : %d \n",getvalreset(),getpid());
         printf(" 6");
         assert(psend(fid1, 12) < 0);
         //printf("\n valeur reset : %d pid du processus actif : %d \n",getvalreset(),getpid());
         printf(" 9");
         assert(psend(fid1, 14) < 0);
         assert(preceive(fid2, &msg) < 0);
         printf(" 12");
         assert(preceive(fid2, &msg) < 0);
         assert(preceive(fid2, &msg) < 0);
         return 0;
 }

 int psender2(void *arg)
 {
         int fid1 = (int)arg;
         int fid2;
         int msg;

         printf(" 3");
         assert(preceive(fid1, &fid2) == 0);
         fid2 -= 42;
         assert(psend(fid1, 6) < 0);
         printf(" 5");
         assert(psend(fid1, 7) == 0);
         assert(psend(fid1, 8) == 0);
         assert(psend(fid1, 9) == 0);
         assert(psend(fid1, 10) == 0);
         assert(psend(fid1, 11) < 0);
         //printf("\n  2 valeur reset : %d pid du processus actif : %d \n",getvalreset(),getpid());
         printf(" 8");
         assert(psend(fid1, 13) < 0);
         //printf("\n 2 valeur reset : %d pid du processus actif : %d \n",getvalreset(),getpid());
         assert((preceive(fid2, &msg) == 0) && (msg == 15));
         assert(preceive(fid2, &msg) < 0);
         printf(" 11");
         assert(preceive(fid2, &msg) < 0);
         assert(preceive(fid2, &msg) < 0);
         return 0;
 }

int test14(void *arg)
{
        int pid1, pid2;
        int fid1 = pcreate(3);
        int fid2 = pcreate(3);
        int msg;

        (void)arg;

        /* Bravo si vous n'etes pas tombe dans le piege. */
        assert(pcreate(1073741827) < 0);

        printf("1");
        assert(getprio(getpid()) == 128);
        assert(fid1 >= 0);
        assert(psend(fid1, fid2 + 42) == 0);
        pid1 = start("psender1", 4000, 131, (void *)fid1);
        pid2 = start("psender2", 4000, 130, (void *)fid1);
        assert((preceive(fid1, &msg) == 0) && (msg == 1));
        assert(chprio(pid2, 132) == 130);
        printf(" 4");
        assert(preset(fid1) == 0);
        assert((preceive(fid1, &msg) == 0) && (msg == 7));
        printf(" 7");
        assert(pdelete(fid1) == 0);
        printf(" 10");
        assert(psend(fid2, 15) == 0);
        assert(preset(fid2) == 0);
        printf(" 13");
        assert(pdelete(fid2) == 0);
        assert(pdelete(fid2) < 0);
        assert(waitpid(pid2, 0) == pid2); //XXX assert(waitpid(-1, 0) == pid2); ???
        assert(waitpid(-1, 0) == pid1);
        printf(".\n");
        return 0;
}

void init_test14(){
  hash_set(&programmes,"psender1",&psender1);
  hash_set(&programmes,"psender2",&psender2);
  hash_set(&programmes,"test14",&test14);
}

/*******************************************************************************
 * Test 15
 *
 * Tuer des processus en attente sur file
 ******************************************************************************/

 int pmsg1(void *arg)
 {
         int fid1 = (int)arg;

         printf(" 2");
         assert(psend(fid1, 1) == 0);
         assert(psend(fid1, 2) == 0);
         assert(psend(fid1, 3) == 0);
         assert(psend(fid1, 4) == 0);
         assert(psend(fid1, 5) == 457);
         return 1;
 }

 int pmsg2(void *arg)
 {
         int fid1 = (int)arg;

         printf(" 3");
         assert(psend(fid1, 6) == 0);
         assert(psend(fid1, 7) == 457);
         return 1;
 }


 int test15(void *arg)
 {
         int pid1, pid2, fid1;
         int msg;
         int count = 1;
         int r = 1;

         (void)arg;

         assert((fid1 = pcreate(3)) >= 0);
         printf("1");
         assert(getprio(getpid()) == 128);
         pid1 = start("pmsg1", 4000, 131, (void *)fid1);
         assert(pid1 > 0);
         pid2 = start("pmsg2", 4000, 130, (void *)fid1);
         assert(pid2 > 0);

         assert((preceive(fid1, &msg) == 0) && (msg == 1));
         assert(kill(pid1) == 0);
         assert(kill(pid1) < 0);
         assert((preceive(fid1, &msg) == 0) && (msg == 2));
         assert(kill(pid2) == 0);
         assert(kill(pid2) < 0);
         assert(preceive(fid1, &msg) == 0);
         assert(msg == 3);
         assert(preceive(fid1, &msg) == 0);
         assert(msg == 4);
         assert(preceive(fid1, &msg) == 0);
         assert(msg == 6);
         assert(pcount(fid1, &count) == 0);
         assert(count == 0);
         assert(waitpid(pid1, &r) == pid1);
         assert(r == 0);
         r = 1;
         assert(waitpid(-1, &r) == pid2);
         assert(r == 0);
         assert(pdelete(fid1) == 0);
         assert(pdelete(fid1) < 0);
         printf(" 4.\n");
         return 0;
 }

 void init_test15(){
   hash_set(&programmes,"pmsg1",&pmsg1);
   hash_set(&programmes,"pmsg2",&pmsg2);
   hash_set(&programmes,"test15",&test15);
 }

 /*******************************************************************************
  * Test 16
  *
  * Test sur des files de diverses tailles et test d'endurance
  ******************************************************************************/
/*
necessite shm_create

#ifdef NB_PROCS
#error ne pas utiliser NB_PROCS
#endif

#define NB_PROCS NBPROC - 5

 int test16(void *arg)
 {

         int i, count, fid, pid;
         struct tst16 *p = NULL;
         int pids[2 * NB_PROCS];

         (void)arg;
         p = (struct tst16*) shm_create("test16_shm");
         assert(p != NULL);

         assert(getprio(getpid()) == 128);
         for (count = 1; count <= 100; count++) {
                 fid = pcreate(count);
                 assert(fid >= 0);
                 p->count = count;
                 p->fid = fid;
                 pid = start("proc16_1", 2000, 128, 0);
                 assert(pid > 0);
                 for (i=0; i<=count; i++) {
                         assert(psend(fid, i) == 0);
                         test_it();
                 }
                 assert(waitpid(pid, 0) == pid);
                 assert(pdelete(fid) == 0);
         }

         p->count = 20000;
         fid = pcreate(50);
         assert(fid >= 0);
         p->fid = fid;
         for (i = 0; i< NB_PROCS; i++) {
                 pid = start("proc16_2", 2000, 127, 0);
                 assert(pid > 0);
                 pids[i] = pid;
         }
         for (i=0; i < NB_PROCS; i++) {
                 pid = start("proc16_3", 2000, 127, 0);
                 assert(pid > 0);
                 pids[NB_PROCS + i] = pid;
         }
         for (i=0; i < 2 * NB_PROCS; i++) {
                 assert(waitpid(pids[i], 0) == pids[i]);
         }
         assert(pcount(fid, &count) == 0);
         assert(count == 0);
         assert(pdelete(fid) == 0);

         shm_release("test16_shm");
         printf("ok.\n");
         return 0;
 }

 #undef NB_PROCS
*/

/*******************************************************************************
 * Test 17
 *
 * On teste des limites de capacite
 ******************************************************************************/

 int proc_return(void *arg)
 {
         return (int)arg;
 }

 static int ids[1200];

 static const int heap_len = 64 << 20;

 int test17(void *arg)
 {
         int i, n, nx;
         int l = sizeof(ids) / sizeof(int);
         int count;
         int prio;

         (void)arg;

         n = 0;
         while (1) {
                 int fid = pcreate(1);
                 if (fid < 0) break;
                 ids[n++] = fid;
                 if (n == l) {
                         assert(!"Maximum number of queues too high !");
                 }
                 test_it();
         }
         for (i=0; i<n; i++) {
                 assert(pdelete(ids[i]) == 0);
                 test_it();
         }
         for (i=0; i<n; i++) {
                 int fid = pcreate(1);
                 assert(fid >= 0);
                 ids[i] = fid;
                 test_it();
         }
         assert(pcreate(1) < 0);
         for (i=0; i<n; i++) {
                 assert(pdelete(ids[i]) == 0);
                 test_it();
         }
         printf("%d", n);

         for (i=0; i<n; i++) {
                 int fid = pcreate(1);
                 assert(fid >= 0);
                 assert(psend(fid, i) == 0);
                 ids[i] = fid;
                 test_it();
         }
         assert(pcreate(1) < 0);
         for (i=0; i<n; i++) {
                 int msg;
                 assert(preceive(ids[i], &msg) == 0);
                 assert(msg == i);
                 assert(pdelete(ids[i]) == 0);
                 test_it();
         }

         count = heap_len / (int)sizeof(int);
         count /= n - 1;
         nx = 0;
         while (nx < n) {
                 int fid = pcreate(count);
                 if (fid < 0) break;
                 ids[nx++] = fid;
                 test_it();
         }
         assert(nx < n);
         for (i=0; i<nx; i++) {
                 assert(pdelete(ids[i]) == 0);
                 test_it();
         }
         printf(" > %d", nx);

         prio = getprio(getpid());
         assert(prio == 128);
         n = 0;
         while (1) {
                 int pid = start("no_run", 2000, 127, 0);
                 if (pid < 0) break;
                 ids[n++] = pid;
                 if (n == l) {
                         assert(!"Maximum number of processes too high !");
                 }
                 test_it();
         }
         for (i=0; i<n; i++) {
                 assert(kill(ids[i]) == 0);
                 assert(waitpid(ids[i], 0) == ids[i]);
                 test_it();
         }
         for (i=0; i<n; i++) {
                 int pid = start("proc_return", 2000, 129, (void *)i);
                 assert(pid > 0);
                 ids[i] = pid;
                 test_it();
         }
         for (i=0; i<n; i++) {
                 int retval;
                 assert(waitpid(ids[i], &retval) == ids[i]);
                 assert(retval == i);
                 test_it();
         }
         printf(", %d.\n", n);
         return 0;
 }

void init_test17(){
  hash_set(&programmes,"test17",&test17);
  hash_set(&programmes,"proc_return",&proc_return);
}

/*******************************************************************************
 * Test 18
 *
 * Amusement : piratage !
 ******************************************************************************/
/*

redefinition de outb

 int proc18_1(void *arg)
 {
         printf("1 ");
         return (int)arg;
 }

 int proc18_2(void *arg)
 {
         printf("2 ");
         return (int)arg;
 }

static char callhack[] = { (char)0xcd, (char)0x32, (char)0xc3 };

__asm__(
                ".text\n"
                ".globl __hacking\n"
                "__hacking:\n"

                "pushal\n"
                "pushl %ds\n"
                "pushl %es\n"
                "movl $0x18,%eax\n"
                "movl %eax,%ds\n"
                "movl %eax,%es\n"
                "cld\n"
                "call __hacking_c\n"
                "popl %es\n"
                "popl %ds\n"
                "popal\n"
                "iret\n"
);

extern void __hacking(void);

__inline__ static void outb(unsigned char value, unsigned short port)
{
        __asm__ __volatile__("outb %0, %1" : : "a" (value), "Nd" (port));
}

__inline__ static unsigned char inb(unsigned short port)
{
        unsigned char rega;
        __asm__ __volatile__("inb %1,%0" : "=a" (rega) : "Nd" (port));
        return rega;
}

static int getpos(void)
{
        int pos;
        outb(0x0f, 0x3d4);
        pos = inb(0x3d4 + 1);
        outb(0x0e, 0x3d4);
        pos += inb(0x3d4 + 1) << 8;
        return pos;
}

static int firsttime = 1;

void __hacking_c(void)
{
        static int pos;
        if (firsttime) {
                firsttime = 0;
                pos = getpos();
        } else {
                int pos2 = getpos();
                const char *str = "          Kernel hacked ! :P          ";
                short *ptr = (short *)0xb8000;
                int p = pos;
                while (p > pos2) p-= 80;
                if ((p < 0) || (p >= 80 * 24)) p = 80 * 23;
                ptr += p;
                while (*str) {
                        *ptr++ = (short)(((128 + 4 * 16 + 15) << 8) + *str++);
                }
        }
}

static void do_hack(void)
{
        firsttime = 1;
        ((void (*)(void))callhack)();
        printf("nok.\n");
        ((void (*)(void))callhack)();
}

int test18(void *arg)
{
        unsigned long a = (unsigned long)__hacking;
        unsigned long a1 = 0x100000 + (a & 0xffff);
        unsigned long a2 = (a & 0xffff0000) + 0xee00;
        int pid1, pid2;
        int cs;

        (void)arg;

        __asm__ volatile ("movl %%cs,%%eax":"=a" (cs));
        if ((cs & 3) == 0) {
                printf("This test can not work at kernel level.\n");
                return -1;
        }
        pid1 = start("proc18_1", 4000, 127, (void *)a1);
        pid2 = start("proc18_2", 4000, 126, (void *)a2);
        assert(pid1 > 0);
        assert(pid2 > 0);
        if ((waitpid(pid1, (int *)0x1190) == pid1) &&
                        (waitpid(pid2, (int *)0x1194) == pid2)) {
                do_hack();
                return 0;
        }
        waitpid(-1, 0);
        waitpid(-1, 0);
        cons_write((char *)0x100000, 50);
        printf("3.\n");
}

void init_test18(){
  hash_set(&programmes,"proc18_1",&proc18_1);
  hash_set(&programmes,"proc18_2",&proc18_2);
  hash_set(&programmes,"test18",&test18);
}
*/

/*******************************************************************************
 * Test 19 clavier
 *
 * Test du clavier.
 ******************************************************************************/

 int cons_rd0(void *arg)
 {
         unsigned long i;
         char buf[101];

         (void)arg;

         i = cons_read(buf, 100);
         buf[i] = 0;
         printf(". : %s\n", buf);
         return 0;
 }

 int cons_rdN(void *arg)
 {
         unsigned long i;
         char buf[101];

         (void)arg;

         i = cons_read(buf, 100);
         buf[i] = 0;
         printf("%d : %s\n", 133 - getprio(getpid()), buf);
         return 0;
 }

static void mega_cycles(int n)
{
        unsigned long long t1, t2;
        int i = 0;

        __asm__ __volatile__("rdtsc":"=A"(t1));
        for (i=0; i<n; i++) {
                do {
                        test_it();
                        __asm__ __volatile__("rdtsc":"=A"(t2));
                } while ((t2 - t1) < 1000000);
                t1 += 1000000;
        }
}

int test19_clavier(void *arg)
{
        char ch[101];
        int i;
        unsigned long j;
        int pid1, pid2, pid3, pid4;

        (void)arg;

        printf("cons_read bloquant, entrez des caracteres : ");
        j = cons_read(ch, 100); ch[j] = 0;
        printf("%lu chars : %s\n", j, ch);
        printf("Frappez une ligne de 5 caracteres : ");
        j = cons_read(ch,5); ch[5] = 0;
        printf("%lu chars : %s\n", j, ch);
        j = cons_read(ch,5);
        if (j != 0) printf("!!! j aurait du etre nul\n");
        assert(cons_read(ch, 0) == 0);

        printf("Les tests suivants supposent un tampon clavier de l'ordre  de 20 caracteres.\n"
               "Entrez des caracteres tant qu'il y a echo, frappez quelques touches\n"
               "supplementaires puis fin de ligne : ");
        j = cons_read(ch, 100); ch[j] = 0;
        printf("%lu chars : %s\n", j, ch);

        printf("Entrees sorties par anticipation. Frappez 4 lignes en veillant a ne pas\ndepasser la taille du tampon clavier : ");
        for (i=0; i<80; i++) {
                mega_cycles(200);
                printf(".");
        }
        printf("Fini.\n");
        pid1 = start("cons_rd0", 4000, 129, 0);
        pid2 = start("cons_rd0", 4000, 129, 0);
        pid3 = start("cons_rd0", 4000, 129, 0);
        pid4 = start("cons_rd0", 4000, 129, 0);
        waitpid(pid2, 0);
        waitpid(pid3, 0);
        waitpid(pid1, 0);
        waitpid(pid4, 0);

        printf("Entrees sorties par anticipation. Depassez maintenant la taille du tampon\n"
               "clavier : ");
        for (i=0; i<80; i++) {
                mega_cycles(200);
                printf(".");
        }
        printf("Fini.\n");
        pid1 = start("cons_rd0", 4000, 129, 0);
        pid2 = start("cons_rd0", 4000, 129, 0);
        pid3 = start("cons_rd0", 4000, 129, 0);
        pid4 = start("cons_rd0", 4000, 129, 0);
        waitpid(pid2, 0);
        waitpid(pid3, 0);
        waitpid(pid1, 0);
        waitpid(pid4, 0);

        printf("Enfin on teste que dans le cas d'entrees bloquantes, les processus sont servis\n"
               "dans l'ordre de leurs priorites. Entrez quatre lignes : ");
        pid1 = start("cons_rdN", 4000, 130, 0);
        pid2 = start("cons_rdN", 4000, 132, 0);
        pid3 = start("cons_rdN", 4000, 131, 0);
        pid4 = start("cons_rdN", 4000, 129, 0);
        waitpid(pid2, 0);
        waitpid(pid3, 0);
        waitpid(pid1, 0);
        waitpid(pid4, 0);

        return 0;
}

void init_test19_clavier(){
  hash_set(&programmes,"cons_rd0",&cons_rd0);
  hash_set(&programmes,"cons_rdN",&cons_rdN);
  hash_set(&programmes,"test19_clavier",&test19_clavier);
}

/*******************************************************************************
 * Test 19 lecture console
 *
 * Quelques processus lisent sur la console et transmettent leur terminaison
 * via une file.
 ******************************************************************************/
/*
cons_read prend des arguments
  int cons_reader(void *arg)
  {
          int fid = (int)arg;
          int c = cons_read();
          assert(psend(fid, 1) == 0);
          printf(" %d (%c)", 134 - getprio(getpid()), c);
          return 0;
  }

 int test19_lecture_console(void *arg)
 {
         int fid = pcreate(10);
         int pid1, pid2, pid3, pid4;
         int param;
         int count;

         (void)arg;

         assert(getprio(getpid()) == 128);
         assert(fid >= 0);
         cons_echo(0);
         pid1 = start("cons_reader", 4000, 130, (void *)fid);
         assert(pid1 > 0);
         pid2 = start("cons_reader", 4000, 132, (void *)fid);
         assert(pid2 > 0);
         pid3 = start("cons_reader", 4000, 131, (void *)fid);
         assert(pid3 > 0);
         pid4 = start("cons_reader", 4000, 129, (void *)fid);
         assert(pid4 > 0);
         printf("1");
         param = 4;
         while (param > 0) {
                 unsigned long long t1, t2;
                 int msg = 0;
                 printf(".");
                 __asm__ __volatile__("rdtsc":"=A"(t1));
                 do {
                         test_it();
                         __asm__ __volatile__("rdtsc":"=A"(t2));
                 } while ((t2 - t1) < 200000000);
                 assert(psend(fid, 0) == 0);
                 param++;
                 do {
                         assert(preceive(fid, &msg) == 0);
                         param--;
                 } while (msg);
         }
         assert(waitpid(pid2, 0) == pid2);
         assert(waitpid(pid3, 0) == pid3);
         assert(waitpid(pid1, 0) == pid1);
         assert(waitpid(pid4, 0) == pid4);
         cons_echo(1);
         assert(pcount(fid, &count) == 0);
         assert(count == 0);
         assert(pdelete(fid) == 0);
         printf(" 6.\n");
         return 0;
 }

 void init_test19_lecture_console(){
   hash_set(&programmes,"cons_reader",cons_reader);
   hash_set(&programmes,"test19_lecture_console",&test19_lecture_console);
 }
 */


/*
 * philosophes
 */

/*

utilise les semaphores

 int launch_philo(void *arg)
 {
         int i, pid;

         (void)arg;

         for (i = 0; i < NR_PHILO; i++) {
                 pid = start("philosophe", 4000, 192, (void *) i);
                 assert(pid > 0);
         }
         return 0;
 }

 static void affiche_etat(struct philo *p)
 {
         int i;
         printf("%c", 13);
         for (i=0; i<NR_PHILO; i++) {
                 unsigned long c;
                 switch (p->etat[i]) {
                         case 'm':
                                 c = 2;
                                 break;
                         default:
                                 c = 4;
                 }
                 (void)c;
                 printf("%c", p->etat[i]);
         }
 }

 static void waitloop(void)
 {
         int j;
         for (j = 0; j < 5000; j++) {
                 int l;
                 test_it();
                 for (l = 0; l < 5000; l++);
         }
 }

 static void penser(struct philo *p, long i)
 {
         xwait(&p->mutex_philo); // DEBUT SC
         p->etat[i] = 'p';
         affiche_etat(p);
         xsignal(&p->mutex_philo); // Fin SC
         waitloop();
         xwait(&p->mutex_philo); // DEBUT SC
         p->etat[i] = '-';
         affiche_etat(p);
         xsignal(&p->mutex_philo); // Fin SC
 }

 static void manger(struct philo *p, long i)
 {
         xwait(&p->mutex_philo); // DEBUT SC
         p->etat[i] = 'm';
         affiche_etat(p);
         xsignal(&p->mutex_philo); // Fin SC
         waitloop();
         xwait(&p->mutex_philo); // DEBUT SC
         p->etat[i] = '-';
         affiche_etat(p);
         xsignal(&p->mutex_philo); // Fin SC
 }

 static int test(struct philo *p, int i)
 {
         // les fourchettes du philosophe i sont elles libres ?
         return ((!p->f[i] && (!p->f[(i + 1) % NR_PHILO])));
 }

 static void prendre_fourchettes(struct philo *p, int i)
 {
         // le philosophe i prend des fourchettes

         xwait(&p->mutex_philo); // Debut SC

         if (test(p, i)) {  // on tente de prendre 2 fourchette
                 p->f[i] = 1;
                 p->f[(i + 1) % NR_PHILO] = 1;
                 xsignal(&p->s[i]);
         } else
                 p->bloque[i] = 1;

         xsignal(&p->mutex_philo); // FIN SC
         xwait(&p->s[i]); // on attend au cas o on ne puisse pas prendre 2 fourchettes
 }

 static void poser_fourchettes(struct philo *p, int i)
 {

         xwait(&p->mutex_philo); // DEBUT SC

         if ((p->bloque[(i + NR_PHILO - 1) % NR_PHILO]) && (!p->f[(i + NR_PHILO - 1) % NR_PHILO])) {
                 p->f[(i + NR_PHILO - 1) % NR_PHILO] = 1;
                 p->bloque[(i + NR_PHILO - 1) % NR_PHILO] = 0;
                 xsignal(&p->s[(i + NR_PHILO - 1) % NR_PHILO]);
         } else
                 p->f[i] = 0;

         if ((p->bloque[(i + 1) % NR_PHILO]) && (!p->f[(i + 2) % NR_PHILO])) {
                 p->f[(i + 2) % NR_PHILO] = 1;
                 p->bloque[(i + 1) % NR_PHILO] = 0;
                 xsignal(&p->s[(i + 1) % NR_PHILO]);
         } else
                 p->f[(i + 1) % NR_PHILO] = 0;

         xsignal(&p->mutex_philo); // Fin SC
 }


 int philosophe(void *arg)
 {
         // comportement d'un seul philosophe
         int i = (int) arg;
         int k;
         struct philo *p;

         p = shm_acquire("shm_philo");
         assert(p != (void*)0);

         for (k = 0; k < 6; k++) {
                 prendre_fourchettes(p, i); // prend 2 fourchettes ou se bloque
                 manger(p, i); // le philosophe mange
                 poser_fourchettes(p, i); // pose 2 fourchettes
                 penser(p, i); // le philosophe pense
         }
         xwait(&p->mutex_philo); // DEBUT SC
         p->etat[i] = '-';
         affiche_etat(p);
         xsignal(&p->mutex_philo); // Fin SC
         shm_release("shm_philo");
         return 0;
 }

int test20(void *arg){
  launch_philo(arg);
}

void init_test20(){
  hash_set(&programmes,"test20",&test20);
}
*/

/*******************************************************************************
 * Ensimag - Projet Systeme
 * Copyright 2013 - Damien Dejean <dam.dejean@gmail.com>
 * Test 21
 *
 * Checks the state of CPU's Translation Lookaside Buffer when the kernel does
 * dynamic memory mapping for a process.
 ******************************************************************************/
/*
utilise shm_create
 int shm_checker(void *arg)
 {
         (void)arg;
         char *shared_area = NULL;

         shared_area = shm_acquire("test21-shm");
         assert(shared_area != NULL);

         // Check we get the memory filled by the main process
         for (int i = 0; i < 4096; i++) {
                 if (shared_area[i] != (char)FILL_VALUE) {
                         return -1;
                 }
         }

        //
        // Fill it with something else to let the main process check we success
        // to access it.
        //
         memset(shared_area, 0, 4096);

         return (int)CHECKER_SUCCESS;
 }

int test21(void *arg)
{
        (void)arg;
        char *shared_area = NULL;
        int checker_pid = -1;
        int checker_ret = -1;

        printf("\n%s\n", "Test 21: checking shared memory space ...");

        shared_area = shm_create("test21-shm");
        assert(shared_area != NULL);

        // We have to be able to fill at least 1 page
        memset(shared_area, FILL_VALUE, 4096);

        // Let the check do its job
        checker_pid = start("shm_checker", 4000, getprio(getpid()) - 1, NULL);
        assert(checker_pid > 0);

        waitpid(checker_pid, &checker_ret);

        switch (checker_ret) {
                case CHECKER_SUCCESS:
                        printf(" -> %s\n -> %s\n", "\"shm_checker\" ends correctly.", "TEST PASSED");
                        break;
                case 0:
                        printf(" -> %s\n -> %s\n", "\"shm_checker\" killed.", "TEST FAILED");
                        break;
                default:
                        printf(" -> %s\n -> %s\n", "\"shm_checker\" returned inconsistent value. Check waitpid implementation.", "TEST FAILED");
        }

        int shm_valid = 1;
        for (int i = 0; i < 4096; i++) {
                if (shared_area[i] != 0) {
                        shm_valid = 0;
                }
        }

        if (shm_valid) {
                printf(" -> %s\n -> %s\n", "shm area content is correct.", "TEST PASSED");
        } else {
                printf(" -> %s\n -> %s\n", "shm area content is invalid.", "TEST FAILED");
        }

        shm_release("test21-shm");
        return 0;
}

void init_test21(){
  hash_set(&programmes,"shm_checker",&shm_checker);
  hash_set(&programmes,"test21",&test21);
}*/


/*******************************************************************************
 * Ensimag - Projet Systeme
 * Copyright 2013 - Damien Dejean <dam.dejean@gmail.com>
 * Test 22
 *
 * Checks the state of CPU's Translation Lookaside Buffer when the kernel does
 * dynamic memory mapping for a process.
 ******************************************************************************/
/*
necessite shm_create

 #define MAGIC_COOKIE        0xDEADBEEFu
 #define MALICIOUS_SUCCESS   0xBAAAAAAD

 int pagefault(void *arg)
 {
         (void)arg;
         // Try to fault
         *((int*)0) = 0;
         // We should have been killed ...
         return (int)MALICIOUS_SUCCESS;
 }

 int malicious(void *arg)
 {
         unsigned *shared_area = NULL;

         assert(arg == NULL);

         shared_area = shm_acquire("test22-shm");
         assert(shared_area != NULL);

         // The parent process should have filled the shared area
         assert(*shared_area == MAGIC_COOKIE);

         // Write in the memory to force w flag in the TLB
         *shared_area = MAGIC_COOKIE;

         //
          // Try to fool the kernel: shm memory is usually done by dynamic memory
          // mapping, if TLB is not cleaned up after unmap, we should keep the
          // access to shared memory even if the entry is no more valid in page
          // directory/table !
          //

         // Unmap
         printf("%s", "  Unmapping shared area ... ");
         shm_release("test22-shm");
         printf("%s\n", "OK");

         // Try a read
         printf("%s", "  Try a read ... ");
         assert(*shared_area == MAGIC_COOKIE);
         printf("%s\n", " done, this is wrong, I should have been killed ! ");

         // Try a write
         printf("%s", "  Try a write ... ");
         *shared_area = 0xDEADB00B;
         printf("%s\n", " done, this is wrong, I should have been killed ! ");

         //
          // Page is not really unmapped until the TLB is cleaned up ! Fix your
          // kernel !
          //

         return (int)MALICIOUS_SUCCESS;
 }

int test22(void *arg)
{
        (void)arg;
        int pagefault_pid = -1;
        int pagefault_ret = -1;
        unsigned *shared_area = NULL;
        int malicious_pid = -1;
        int malicious_ret = -1;

        //
         // Check page fault handling.
         //
        printf("\n%s\n", "Test 22: checking page fault handling...");
        pagefault_pid = start("pagefault", 4000, getprio(getpid()) - 1, NULL);

        waitpid(pagefault_pid, &pagefault_ret);

        // "pagefault" should have been killed
        switch (pagefault_ret) {
                case 0:
                        printf("-> %s\n-> %s\n", "\"pagefault\" process killed.", "TEST PASSED");
                        break;

                case (int)MALICIOUS_SUCCESS:
                        printf("-> %s\n-> %s\n", "\"pagefault\" process should not ends correctly.", "TEST FAILED");
                        break;
                default:
                        printf("-> %s\n-> %s\n", "unexpected return value for \"pagefault\" process. Check waitpid and kill behaviors", "TEST FAILED");
        }


        //
         // Check virtual memory mappings consistency.
         //
        shared_area = shm_create("test22-shm");
        assert(shared_area != NULL);

        // The malicious process will check this value
        *shared_area = MAGIC_COOKIE;

        printf("%s\n", "Test 22: checking shared memory mappings ...");
        malicious_pid = start("malicious", 4000, getprio(getpid()) - 1, NULL);

        waitpid(malicious_pid, &malicious_ret);

        // "malicious" should have been killed
        switch (malicious_ret) {
                case 0:
                        printf("-> %s\n-> %s\n", "\"malicious\" process killed.", "TEST PASSED");
                        break;

                case (int)MALICIOUS_SUCCESS:
                        printf("-> %s\n-> %s\n", "\"malicious\" process should not ends correctly.", "TEST FAILED");
                        break;
                default:
                        printf("-> %s\n-> %s\n", "unexpected return value for \"malicious\" process. Check waitpid and kill behaviors", "TEST FAILED");
        }

        // and should not have been able to modify shared memory
        if (*shared_area != MAGIC_COOKIE) {
                printf("-> %s\n-> %s\n", "\"malicious\" have been able to write unmapped memory.", "TEST FAILED");
        }

        shm_release("test22-shm");
        while(1);
        return 0;
}

void init_test22(){
  hash_set(&programmes,"pagefault",&pagefault);
  hash_set(&programmes,"malicious",&malicious);
  hash_set(&programmes,"test22",&test22);
}
*/
///////////////////////////////////////////////////////////////////////////////


void init_tout_les_tests(){
  hash_set(&programmes,"idle_test",&idle_test);
  hash_set(&programmes,"tout_tester",&tout_tester);
  init_test_clock();
  init_test0();
  init_test1();
  init_test2();
  init_test3();
  init_test3_1();
  init_test4();
  init_test5();
  init_test6();
  //init_test7();
  init_test8();
  //init_test9();
  init_test10();
  //init_test11();
  init_test12();
  //init_test13();
  init_test14();
  init_test15();
  //init_test16();
  init_test17();
  //init_test18();
  init_test19_clavier();
  //init_test19_lecture_console();
  //init_test20();// utilise les semaphores
  //init_test21();
  //init_test22();
}
