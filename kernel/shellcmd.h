/*
 * shellcmd.h
 */
#include "syscalls.h"
#include "afficher_processus.h"
#include "print.h"

#ifndef SHELLCMD_H
#define SHELLCMD_H

void shell_ps();
void shell_exit();
void shell_echo(int i);
void shell_clear();

#endif
