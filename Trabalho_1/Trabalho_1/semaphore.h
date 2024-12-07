#ifndef SEMAPHORE_H
#define	SEMAPHORE_H

#include "types.h"

// Rotinas para manipulação de semáforo (padrão POSIX)
void sem_init(sem_t *s, uint_t valor);
void sem_wait(sem_t *s);
void sem_post(sem_t *s);



#endif	/* SEMAPHORE_H */

