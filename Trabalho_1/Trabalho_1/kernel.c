#include "kernel.h"
#include "user_app.h"

ready_queue_t Ready_queue;

void __interrupt() isr_timer(void)
{
    if (INTCONbits.TMR0IF == 1) {
        // Verifica se tem tarefas em estado de WAITING
        for (int i = 0; i < Ready_queue.queue_size; i++) {
            if (Ready_queue.tasks[i].task_STATE == WAITING) {
                Ready_queue.tasks[i].task_time_sleeping--;
                if (Ready_queue.tasks[i].task_time_sleeping == 0) {
                    Ready_queue.tasks[i].task_STATE = READY;
                }
            }
        }

        SAVE_CONTEXT(READY);
        // Ready_queue.task_running = round_robin_scheduler();
        Ready_queue.task_running = scheduler();    
        RESTORE_CONTEXT();
        // Zera o flag do timer
        INTCONbits.TMR0IF   = 0;
        TMR0                = 0;
    }

}


void create_task(uint_t id, uint_t prior, t_ptr func)
{
    // Cria��o de um novo bloco de controle de tarefa (TCB)
    tcb_t new_task;

    // Inicializa��o dos campos do TCB
    new_task.task_ID                = id;        // Identificador �nico da tarefa
    new_task.task_PRIOR             = prior;     // Prioridade da tarefa
    new_task.task_PTR               = func;      // Fun��o que ser� executada pela tarefa
    new_task.task_STACK.stack_size  = 0;         // Tamanho da pilha (inicializado como 0)
    new_task.task_STATE             = READY;     // Estado inicial da tarefa (pronta para execu��o)

    // Adiciona a nova tarefa � fila de tarefas prontas (Ready_queue)
    Ready_queue.tasks[Ready_queue.queue_size]   = new_task;
    Ready_queue.queue_size                     += 1; // Incrementa o tamanho da fila
}

void setup_os(void)
{
    // Inicializa a fila de tarefas (Ready_queue)
    Ready_queue.queue_size      = 0; // Nenhuma tarefa na fila inicialmente
    Ready_queue.task_running    = 0; // Primeira tarefa em execu��o ser� na posi��o 0 (idle)

    // Cria a tarefa idle (tarefa padr�o do sistema)
    create_task(0, 0, idle); // ID = 0, Prioridade = 0, Fun��o = idle

    // Declara o s�mbolo _idle como global para o assembler
    asm("GLOBAL _idle");
}

void start_os(void)
{
    // Configura o Timer 0 para gerar interrup��es peri�dicas
    config_timer0();
    
    // Configura os m�dulos ou perif�ricos necess�rios para o aplicativo do usu�rio
    config_app_user();
    
    // Ativa o Timer 0, iniciando o funcionamento do sistema operacional
    T0CONbits.TMR0ON    = 1;
}

void delay_task(uint_t time)
{
    // Desabilita as interrup��es
    di();
    
    Ready_queue.tasks[Ready_queue.task_running].task_time_sleeping = time;
    
    // Salva contexto da tarefa que ficar� aguardando por um tempo
    SAVE_CONTEXT(WAITING);
    // Chama o escalonador para selecionar a pr�xima tarefa apta
    Ready_queue.task_running = scheduler();
    // Restaura o contexto da tarefa que foi selecionada para executar
    RESTORE_CONTEXT();
    
    // Habilita as interrup��es
    ei();
}

void idle(void) 
{
    while (1) {
        // Alterna o estado l�gico do pino RD5 (para depura��o ou controle de hardware)
        LATDbits.LATD5 = ~PORTDbits.RD5;

        // Executa uma instru��o de "n�o opera��o" (para estabilidade ou ajustes de tempo)
        Nop();
    }
}

void task_yield(void)
{
    // Desabilita as interrup��es
    di();
 
    // Salva contexto da tarefa que est� entregando o processador
    SAVE_CONTEXT(READY);
    // Chama o escalonador para selecionar a pr�xima tarefa apta
    Ready_queue.task_running = scheduler();
    // Restaura o contexto da tarefa que foi selecionada para executar
    RESTORE_CONTEXT();
    
    // Habilita as interrup��es
    ei();    
}

uint_t priority_scheduler(void)
{
    // Inicializa a vari�vel que armazenar� o �ndice da tarefa com maior prioridade
    uint_t maior_prioridade = 0, i;
    
    // Busca pela primeira tarefa pronta (READY) na fila de tarefas
    for (i = 1; i < Ready_queue.queue_size; i++) {
        if (Ready_queue.tasks[i].task_STATE == READY) {
            maior_prioridade = i;  // Atualiza o �ndice da tarefa com maior prioridade
            break;  // Interrompe o loop ao encontrar a primeira tarefa pronta
        }
    }
    
    // Se nenhuma tarefa pronta foi encontrada, retorna o �ndice 0 (tarefa idle)
    if (maior_prioridade == 0) return 0;
    
    // Continua a busca pela tarefa com maior prioridade
    for (i = maior_prioridade; i < Ready_queue.queue_size - 1; i++) {
        // Se encontrar uma tarefa com maior prioridade, atualiza o �ndice
        if (Ready_queue.tasks[i].task_PRIOR < Ready_queue.tasks[i + 1].task_PRIOR &&
            Ready_queue.tasks[i + 1].task_STATE == READY &&
            Ready_queue.tasks[i + 1].task_PTR != idle) {
            maior_prioridade = i + 1;
        }
    }
    
    // Retorna o �ndice da tarefa com maior prioridade
    return maior_prioridade;
}

uint_t __reentrant round_robin_scheduler(void)
{
    uint_t next_task = Ready_queue.task_running, vez = 0;
    
    do {
        // Incrementa o �ndice da tarefa a ser executada e faz o ciclo na fila
        next_task = (next_task + 1) % Ready_queue.queue_size;

        // Se voltou ao in�cio da fila, incrementa o contador de ciclos
        if (next_task == 0) {
            vez++;  // Um ciclo completo foi realizado

            // Ap�s dois ciclos completos, retorna a tarefa idle
            if (vez == 2) return 0;
        }

    // Continua at� encontrar uma tarefa com estado READY e n�o seja a tarefa idle
    } while (Ready_queue.tasks[next_task].task_STATE != READY ||
             Ready_queue.tasks[next_task].task_PTR == idle);

    return next_task;  // Retorna o �ndice da tarefa pronta para ser executada
}


uint_t __reentrant scheduler(void)
{
    // Seleciona o tipo de escalonador com base na configura��o do pr�-processador
#if RR_SCHEDULER
    return round_robin_scheduler();  // Chama o escalonador round-robin
#elif PRIOR_SCHEDULER
    return priority_scheduler();     // Chama o escalonador de prioridade
#endif    
}

void config_timer0(void)
{
    // Configura��es de interrup��o
    INTCONbits.PEIE     = 1;   // Habilita interrup��es de perif�ricos
    INTCONbits.TMR0IE   = 1;   // Habilita interrup��o do Timer 0
    INTCONbits.TMR0IF   = 0;   // Limpa o flag de interrup��o do Timer 0
    INTCONbits.GIE      = 1;   // Habilita interrup��es globais

    // Configura��es do Timer 0
    T0CONbits.T08BIT    = 1;   // Configura o Timer 0 para 8 bits
    T0CONbits.T0CS      = 0;   // Seleciona o clock interno como fonte de clock
    T0CONbits.PSA       = 0;   // Atribui o prescaler ao Timer 0
    T0CONbits.T0PS      = 0b111; // Configura o prescaler para 1:32
    TMR0                = 0;   // Inicializa o Timer 0 com valor 0
}

void teste(void)
{
    Nop();
}