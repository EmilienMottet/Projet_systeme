/*
 * Ensimag - Projet système
 * Copyright (C) 2012 - Damien Dejean <dam.dejean@gmail.com>
 */

/*******************************************************************************
 * Division 64 bits
 ******************************************************************************/

 typedef unsigned long long uint_fast64_t;
 typedef unsigned long uint_fast32_t;
 short randShort(void);
 void setSeed(uint_fast64_t _s);
 unsigned long rand();
 unsigned long long div64(unsigned long long num, unsigned long long div, unsigned long long *rem);
