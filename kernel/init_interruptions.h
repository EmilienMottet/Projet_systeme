#ifndef INTER_H
#define INTER_H

#include "segment.h"//pour KERNEL_CS


#include "debugger.h"
#include "print.h"
#include "horloge.h"
#include "console.h"
#include "process.h"

void masque_IRQ(uint32_t num_IRQ/*entre 0 et 7*/, char masque);

void traitant_IT_32(void);//prototype visible

void traitant_IT_49(void);

void traitant_IT_33(void);


void init_traitant_IT(int32_t num_IT, void (*traitant)(void));

void initialiser_interruptions();


#endif
