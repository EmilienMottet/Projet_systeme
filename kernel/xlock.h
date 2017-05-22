/* Wrapper sur les verrous basés sur les sémaphores ou les files de messages */
union sem {
    int fid;
    int sem;
};
void xwait(union sem *s);
void xsignal(union sem *s);
void xsdelete(union sem *s);
void xscreate(union sem *s);

#ifndef TELECOM_TST
void clock_settings(unsigned long *quartz, unsigned long *ticks);
unsigned long current_clock(void);
void wait_clock(unsigned long wakeup);
#endif
int start(const char *process_name, unsigned long ssize, int prio, void *arg);
int waitpid(int pid, int *retval);

#if defined WITH_SEM
/*
 * Pour la soutenance, devrait afficher la liste des processus actifs, des
 * semaphores utilises et toute autre info utile sur le noyau.
 */
#elif defined WITH_MSG
/*
 * Pour la soutenance, devrait afficher la liste des processus actifs, des
 * files de messages utilisees et toute autre info utile sur le noyau.
 */
#endif
