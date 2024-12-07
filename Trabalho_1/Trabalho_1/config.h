#ifndef CONFIG_H
#define	CONFIG_H

#define STACK_SIZE      10
#define MAX_TASKS       4
#define PIPE_SIZE       2
#define SEM_SIZE        3

// Macros para a definição do escalonador
// 1 ativado
// 0 desativado
#define RR_SCHEDULER        1
#define PRIOR_SCHEDULER     0


#endif	/* CONFIG_H */

