    .text
# cette directive sert a rendre l'etiquette publique
    .globl start
# strart systemcall
start:
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %esi
	movl 0x20(%esp),%ebx
	movl 0x1C(%esp),%ecx
	movl 0x18(%esp),%edx
	movl 0x14(%esp),%esi
	mov $0x00000001,%eax
	int $49
	popl %esi
	popl %edx
	popl %ecx
 	popl %ebx
    ret
# cette directive sert a rendre l'etiquette publique
    .globl halt
# halt systemcall
halt:
	mov $0x00000002,%eax
	int $49
    ret

.globl exit
exit:
	pushl %ebx
	movl 0x20(%esp),%ebx
	mov $0x00000003,%eax
	int $49
 	popl %ebx
    ret

.globl kill
kill:
	pushl %ebx
	movl 0x20(%esp),%ebx
	mov $0x00000004,%eax
	int $49
	popl %ebx
    ret

.globl waitpid
waitpid:
	pushl %ebx
	pushl %ecx
	movl 0x20(%esp),%ebx
	movl 0x1C(%esp),%ecx
	mov $0x00000005,%eax
	int $49
	popl %ecx
 	popl %ebx
    ret

.globl getprio
getprio:
	pushl %ebx
	movl 0x20(%esp),%ebx
	mov $0x00000006,%eax
	int $49
 	popl %ebx
    ret

.globl chprio
chprio:
	pushl %ebx
	pushl %ecx
	movl 0x20(%esp),%ebx
	movl 0x1C(%esp),%ecx
	mov $0x00000007,%eax
	int $49
	popl %ecx
 	popl %ebx
    ret

.globl getpid
getpid:
	mov $0x00000008,%eax
	int $49
    ret

.globl cons_read
cons_read:
	pushl %ebx
	pushl %ecx
	movl 0x20(%esp),%ebx
	movl 0x1C(%esp),%ecx
	mov $0x00000009,%eax
	int $49
	popl %ecx
 	popl %ebx
    ret

.globl cons_write
cons_write:
	pushl %ebx
	pushl %ecx
	movl 0x20(%esp),%ebx
	movl 0x1C(%esp),%ecx
	mov $0x0000000a,%eax
	int $49
	popl %ecx
 	popl %ebx
    ret

.globl cons_echo
cons_echo:
	pushl %ebx
	movl 0x20(%esp),%ebx
	movl 0x1C(%esp),%ecx
	mov $0x0000000b,%eax
	int $49
 	popl %ebx
    ret
