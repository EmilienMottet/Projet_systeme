#ifndef HORLOGE_H
#define HORLOGE_H

void regler_la_frequence_de_l_horloge();

unsigned long current_clock();

void clock_settings(unsigned long *quartz, unsigned long *ticks);

void wait_clock(unsigned long clock);

#endif
