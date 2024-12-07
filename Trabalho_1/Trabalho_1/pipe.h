#ifndef PIPE_H
#define	PIPE_H

#include "types.h"

// Rotinas para manipulação do pipe
void pipe_init(pipe_t *p);
void pipe_write(pipe_t *p, byte data);
void pipe_read(pipe_t *p, byte *data);



#endif	/* PIPE_H */

