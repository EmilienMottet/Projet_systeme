//#include "sysapi.h"
#include "process.h"
#include "file.h"

#define DUMMY_VAL 78

/////////////////////
//#define WITH_MSG
//#define WITH_SEM
//#undef NULL
//#include "sysapi.h"
/////////////////////
#include "math.h"

void init_tout_les_tests();
int tout_tester(void);
void idle_test();
/*******************************************************************************
 * Test 4
 *
 * Boucles d'attente active (partage de temps)
 * chprio()
 * kill() de processus de faible prio
 * kill() de processus deja mort
 ******************************************************************************/

#ifdef microblaze
static const int loop_count0 = 500;
static const int loop_count1 = 1000;
#else
static const int loop_count0 = 5000;
static const int loop_count1 = 10000;
#endif

//test13.h
struct psender {
    int fid;
    char data[32];
};

//teste16.h
struct tst16 {
    int count;
    int fid;
};

/*
utilise les semaphores
//test20.h
#define NR_PHILO 5
struct philo {
        char f[NR_PHILO]; // tableau des fourchettes, contient soit 1 soit 0 selon si elle
                          //   est utilisee ou non
        char bloque[NR_PHILO]; // memorise l'etat du philosophe, contient 1 ou 0 selon que le philosophe
                              //  est en attente d'une fourchette ou non
        // Padding pour satisfaire un compilateur strict.
        char padding[sizeof(int) - (NR_PHILO * 2) % sizeof(int)];
        union sem mutex_philo; // exclusion mutuelle
        union sem s[NR_PHILO]; // un semaphore par philosophe
        int etat[NR_PHILO];
};
*/

// test21.h
#define FILL_VALUE          0xCA
#define CHECKER_SUCCESS     0x12345678
