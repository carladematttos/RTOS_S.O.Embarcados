#include "semaphore.h"   // Inclui o cabe�alho para a manipula��o de sem�foros
#include "kernel.h"      // Inclui o cabe�alho do kernel (n�cleo do sistema operacional)

// Rotina para inicializar um sem�foro
// Esta fun��o configura o sem�foro com o valor inicial e outras vari�veis associadas
void sem_init(sem_t *s, uint_t valor)
{
    di();  // Desativa interrup��es para evitar problemas de concorr�ncia durante a configura��o do sem�foro

    s->sem_cont             = valor;              // Define o contador do sem�foro com o valor inicial
    s->sem_initial_value    = valor;              // Armazena o valor inicial do sem�foro (�til para reinicializa��o)
    s->sem_queue_size       = 0;                  // Inicializa a fila de espera como vazia
    s->sem_queue_pos_release= 0;                  // Inicializa a posi��o de libera��o da fila de espera

    ei();  // Reativa as interrup��es ap�s a configura��o do sem�foro
}

// Fun��o para realizar a opera��o "sem_wait" em um sem�foro (sem�foro de bloqueio)
void sem_wait(sem_t *s)
{
    di();  // Desativa as interrup��es para garantir exclus�o m�tua e evitar condi��es de corrida
    
    s->sem_cont--;  // Decrementa o contador do sem�foro

    if (s->sem_cont < 0) {
        // Se o sem�foro n�o estiver dispon�vel (contador negativo), a tarefa ficar� bloqueada
        // A tarefa atual ser� colocada na fila de espera do sem�foro
        s->sem_queue[s->sem_queue_size] = Ready_queue.task_running;
        s->sem_queue_size = (s->sem_queue_size + 1) % SEM_SIZE;  // Atualiza a posi��o da fila de espera, com controle circular

        // Salva o contexto da tarefa que ficar� bloqueada para retomada posterior
        SAVE_CONTEXT(WAITING_SEM);

        // Chama o escalonador para selecionar a pr�xima tarefa que pode ser executada
        Ready_queue.task_running = scheduler();

        // Restaura o contexto da tarefa que foi selecionada para execu��o
        RESTORE_CONTEXT();        
    }

    ei();  // Reativa as interrup��es, permitindo a continuidade das tarefas
}

// Fun��o para realizar a opera��o "sem_post" em um sem�foro (sem�foro de libera��o)
void sem_post(sem_t *s)
{
    di();  // Desativa interrup��es para evitar condi��es de corrida e garantir integridade de dados
    
    s->sem_cont++;  // Incrementa o contador do sem�foro

    if (s->sem_cont <= 0) {
        // Se o contador do sem�foro for menor ou igual a zero, isso significa que h� tarefas na fila de espera

        // A pr�xima tarefa na fila de espera ser� liberada e poder� voltar para o estado READY
        Ready_queue.tasks[s->sem_queue[s->sem_queue_pos_release]].task_STATE = READY;
        s->sem_queue_pos_release = (s->sem_queue_pos_release+1) % s->sem_queue_size;  // Atualiza a posi��o da fila de libera��o, com controle circular

        // Salva o contexto da tarefa que estava bloqueada
        SAVE_CONTEXT(READY);

        // Chama o escalonador para selecionar a pr�xima tarefa a ser executada
        Ready_queue.task_running = scheduler();

        // Restaura o contexto da tarefa que foi selecionada para execu��o
        RESTORE_CONTEXT();                
    }

    ei();  // Reativa as interrup��es, permitindo a continuidade das tarefas    
}
