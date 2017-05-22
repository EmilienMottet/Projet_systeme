#ifndef _PAGE_H
#define _PAGE_H

#include <stdbool.h>
#include <stdint.h>
#include"phy_mem_allocator.h"
#include "boot/processor_structs.h"

bool is_bit_n_true(int n, uint32_t adr);
void set_bit_n(int n, bool boo, uint32_t * adr);
int get_n_bits(int n_fort, int n_faible, uint32_t adr);
void permision_ecriture(bool boo, uint32_t *page);
bool a_permission_ecriture(uint32_t page);
void put_superuser(bool boo, uint32_t * page);
bool is_superuser(uint32_t page);
uint32_t * create_page_directory();
int create_page_table(uint32_t * case_page_directory); 
int put_address_reel(void* adr_reel, void* adr_virt, PERMISSION perm, uint32_t *page_directory);
uint32_t get_address_reel(uint32_t adr_virt, uint32_t * page_directory);
void free_page(uint32_t *page_directory);
uint32_t * get_page_directory(int pid);
void put_cr3(int pid);


#endif
