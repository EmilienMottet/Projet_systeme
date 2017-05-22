/*
 * syscalls.h
 */

#ifndef SHARED_SYSCALLS_H_
#define SHARED_SYSCALLS_H_

int start(const char *nom,unsigned long ssize, int prio, void *args);
int kill(int pid);
int waitpid(int pid, int *retvalp);
int getpid(void); // La valeur de retour de getpid est le pid du processus appelant cette primitive.
int getprio(int pid);
void exit(int retval);
int chprio(int pid, int newprio);

unsigned long cons_read(char *string, unsigned long length);
int cons_write(const char *str, long size);
void cons_echo(int on);

void halt();

#endif /* SHARED_SYSCALLS_H_ */
