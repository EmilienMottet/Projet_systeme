#include "process.h"
#include "phy_mem_allocator.h"
// prend en apramètre pid_libre
void put_stack(int pid) {
  // tables_des_processus + pid = le processus que je traite

  int last_page = get_nb_pages(pid) - 1;
  void* addr_stack = get_adr_virt(pid, last_page);

  __asm__("movl %0, %%esp" : : "r" (addr_stack) );
}
