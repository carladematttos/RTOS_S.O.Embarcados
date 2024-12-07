#ifndef TYPES_H
#define	TYPES_H

#include <xc.h>
#include "config.h"

#define TRUE            1
#define FALSE           0
typedef unsigned char bool;  // Define bool type
typedef unsigned int uint_t;
typedef char byte;
typedef void (*t_ptr)(void);
typedef enum {READY = 0, RUNNING, WAITING, WAITING_SEM} state_t;

typedef struct stack {
    uint24_t stack_data[STACK_SIZE];
    uint_t stack_size;
} stack_t;

typedef struct tcb {
    uint_t task_ID;
    uint_t task_PRIOR;
    stack_t task_STACK;
    t_ptr task_PTR;
    byte task_WREG;
    byte task_BSRREG;
    byte task_STATUSREG;
    state_t task_STATE;
    uint_t task_time_sleeping;
} tcb_t;

typedef struct ready_queue {
    tcb_t tasks[MAX_TASKS+1];
    uint_t queue_size;
    uint_t task_running;
} ready_queue_t;


// Estrutura de dados de controle do semáforo
typedef struct sem {
    int sem_cont;
    uint_t sem_initial_value;
    uint_t sem_queue[SEM_SIZE];
    uint_t sem_queue_size;
    uint_t sem_queue_pos_release;
} sem_t;

typedef struct pipe {
    uint_t pos_read;
    uint_t pos_write;
    byte pipe_data[PIPE_SIZE];
    sem_t pipe_sem_write;
    sem_t pipe_sem_read;
} pipe_t;

#endif	/* TYPES_H */

