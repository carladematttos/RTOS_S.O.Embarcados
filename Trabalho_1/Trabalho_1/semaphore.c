#include "semaphore.h"   // Inclui o cabeçalho para a manipulação de semáforos
#include "kernel.h"      // Inclui o cabeçalho do kernel (núcleo do sistema operacional)

// Rotina para inicializar um semáforo
// Esta função configura o semáforo com o valor inicial e outras variáveis associadas
void sem_init(sem_t *s, uint_t valor)
{
    di();  // Desativa interrupções para evitar problemas de concorrência durante a configuração do semáforo

    s->sem_cont             = valor;              // Define o contador do semáforo com o valor inicial
    s->sem_initial_value    = valor;              // Armazena o valor inicial do semáforo (útil para reinicialização)
    s->sem_queue_size       = 0;                  // Inicializa a fila de espera como vazia
    s->sem_queue_pos_release= 0;                  // Inicializa a posição de liberação da fila de espera

    ei();  // Reativa as interrupções após a configuração do semáforo
}

// Função para realizar a operação "sem_wait" em um semáforo (semáforo de bloqueio)
void sem_wait(sem_t *s)
{
    di();  // Desativa as interrupções para garantir exclusão mútua e evitar condições de corrida
    
    s->sem_cont--;  // Decrementa o contador do semáforo

    if (s->sem_cont < 0) {
        // Se o semáforo não estiver disponível (contador negativo), a tarefa ficará bloqueada
        // A tarefa atual será colocada na fila de espera do semáforo
        s->sem_queue[s->sem_queue_size] = Ready_queue.task_running;
        s->sem_queue_size = (s->sem_queue_size + 1) % SEM_SIZE;  // Atualiza a posição da fila de espera, com controle circular

        // Salva o contexto da tarefa que ficará bloqueada para retomada posterior
        SAVE_CONTEXT(WAITING_SEM);

        // Chama o escalonador para selecionar a próxima tarefa que pode ser executada
        Ready_queue.task_running = scheduler();

        // Restaura o contexto da tarefa que foi selecionada para execução
        RESTORE_CONTEXT();        
    }

    ei();  // Reativa as interrupções, permitindo a continuidade das tarefas
}

// Função para realizar a operação "sem_post" em um semáforo (semáforo de liberação)
void sem_post(sem_t *s)
{
    di();  // Desativa interrupções para evitar condições de corrida e garantir integridade de dados
    
    s->sem_cont++;  // Incrementa o contador do semáforo

    if (s->sem_cont <= 0) {
        // Se o contador do semáforo for menor ou igual a zero, isso significa que há tarefas na fila de espera

        // A próxima tarefa na fila de espera será liberada e poderá voltar para o estado READY
        Ready_queue.tasks[s->sem_queue[s->sem_queue_pos_release]].task_STATE = READY;
        s->sem_queue_pos_release = (s->sem_queue_pos_release+1) % s->sem_queue_size;  // Atualiza a posição da fila de liberação, com controle circular

        // Salva o contexto da tarefa que estava bloqueada
        SAVE_CONTEXT(READY);

        // Chama o escalonador para selecionar a próxima tarefa a ser executada
        Ready_queue.task_running = scheduler();

        // Restaura o contexto da tarefa que foi selecionada para execução
        RESTORE_CONTEXT();                
    }

    ei();  // Reativa as interrupções, permitindo a continuidade das tarefas    
}
