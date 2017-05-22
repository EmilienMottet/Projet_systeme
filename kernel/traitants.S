    .text
# cette directive sert a rendre l'etiquette publique
    .globl traitant_IT_32
# debut du traitant
traitant_IT_32:
# sauvegarde des registres importants
    pushl %eax
    pushl %edx
    pushl %ecx
# appel a la fonction C realisant le traitant
    call tic_PIT
# restauration des registres importants
    popl %ecx
    popl %edx
    popl %eax
# fin du traitant
    iret



    .globl traitant_IT_49
    .globl fin_it_49
# debut du traitant
traitant_IT_49:
	mov $0x18, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
# recherche du traitant
  	cmp $0x0000000b,%eax
  	je _cons_echo
  	cmp $0x0000000a,%eax
  	je _cons_write
  	cmp $0x00000009,%eax
  	je _cons_read
  	cmp $0x00000008,%eax
  	je _getpid
  	cmp $0x00000007,%eax
  	je _chprio
  	cmp $0x00000006,%eax
  	je _getprio
  	cmp $0x00000005,%eax
  	je _waitpid
  	cmp $0x00000004,%eax
  	je _kill
  	cmp $0x00000003,%eax
  	je _exit
  	cmp $0x00000002,%eax
  	je _halt
  	cmp $0x00000001,%eax
  	je _start
# fin du traitant
fin_it_49:
	mov $0x4b, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	iret

#
# appel a _cons_echo
_cons_echo:
# sauvegarde des registres importants
# rien
# recuperation des parametres
	pushl %ebx
# appel a la fonction C realisant le traitant
    call cons_echo
# restauration des registres importants
 	popl %ebx
# fin du traitant
	jmp fin_it_49

#
# appel a _cons_write
_cons_write:
# sauvegarde des registres importants
# rien
# recuperation des parametres
	pushl %ebx
	pushl %ecx
# appel a la fonction C realisant le traitant
    call cons_write
# restauration des registres importants
	popl %ecx
 	popl %ebx
# fin du traitant
	jmp fin_it_49

#
# appel a _cons_read
_cons_read:
# sauvegarde des registres importants
# rien
# recuperation des parametres
	pushl %ebx
	pushl %ecx
# appel a la fonction C realisant le traitant
    call cons_read
# restauration des registres importants
	popl %ecx
 	popl %ebx
# fin du traitant
	jmp fin_it_49

#
# appel a _getpid
_getpid:
# sauvegarde des registres importants
# rien
# recuperation des parametres
# appel a la fonction C realisant le traitant
    call getpid
# restauration des registres importants
# fin du traitant
	jmp fin_it_49

#
# appel a _chprio
_chprio:
# sauvegarde des registres importants
# rien
# recuperation des parametres
	pushl %ebx
	pushl %ecx
# appel a la fonction C realisant le traitant
    call chprio
# restauration des registres importants
	popl %ecx
 	popl %ebx
# fin du traitant
	jmp fin_it_49

#
# appel a _getprio
_getprio:
# sauvegarde des registres importants
# rien
# recuperation des parametres
	pushl %ebx
# appel a la fonction C realisant le traitant
    call getprio
# restauration des registres importants
 	popl %ebx
# fin du traitant
	jmp fin_it_49

#
# appel a _waitpid
_waitpid:
# sauvegarde des registres importants
# rien
# recuperation des parametres
	pushl %ebx
	pushl %ecx
# appel a la fonction C realisant le traitant
    call waitpid
# restauration des registres importants
	popl %ecx
 	popl %ebx
# fin du traitant
	jmp fin_it_49

#
# appel k_kill
_kill:
# sauvegarde des registres importants
# recuperation des parametres
	pushl %ebx
# appel a la fonction C realisant le traitant
	call kill
# restauration des registres importants
	popl %ebx
# fin du traitant
	jmp fin_it_49

#
# appel a k_exit
_exit:
# sauvegarde des registres importants
# rien
# recuperation des parametres
	pushl %ebx
# appel a la fonction C realisant le traitant
    call exit
# restauration des registres importants
 	popl %ebx
# fin du traitant
	jmp fin_it_49

#
# appel k_halt
_halt:
# sauvegarde des registres importants
# traitement
	sti
	hlt
	cli
# restauration des registres importants
# fin du traitant
	jmp fin_it_49

#
# appel a k_start
_start:
# sauvegarde des registres importants
# rien
# recuperation des parametres
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esi
# appel a la fonction C realisant le traitant
    call k_start
# restauration des registres importants
	popl %esi
	popl %edx
	popl %ecx
 	popl %ebx
# fin du traitant
	jmp fin_it_49



# cette directive sert a rendre l'etiquette publique
    .globl traitant_IT_33
# debut du traitant
traitant_IT_33:
# sauvegarde des registres importants
    pushl %eax
    pushl %edx
    pushl %ecx
# appel a la fonction C realisant le traitant
    call traitant_clavier
# restauration des registres importants
    popl %ecx
    popl %edx
    popl %eax
# fin du traitant
    iret

