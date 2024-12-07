#include "pipe.h"          // Inclui o cabeçalho para manipulação de pipes
#include "semaphore.h"     // Inclui o cabeçalho para manipulação de semáforos

// Função para inicializar o pipe
// Inicializa as posições de leitura e escrita, além dos semáforos que controlam o acesso ao pipe
void pipe_init(pipe_t *p)
{
    p->pos_read     = 0;  // Inicializa a posição de leitura como 0 (início do buffer)
    p->pos_write    = 0;  // Inicializa a posição de escrita como 0 (início do buffer)
    
    // Inicializa os semáforos para controlar o fluxo de dados
    // semáforo de escrita: inicializado com o tamanho do pipe (permitindo que o pipe esteja cheio até seu tamanho máximo)
    sem_init(&p->pipe_sem_write, PIPE_SIZE);
    
    // semáforo de leitura: inicializado com 0 (não é possível ler até que haja algo escrito no pipe)
    sem_init(&p->pipe_sem_read, 0);
}

// Função para escrever um byte de dados no pipe
void pipe_write(pipe_t *p, byte data)
{
    // Aguarda até que haja espaço disponível para escrever no pipe
    sem_wait(&p->pipe_sem_write);
    
    // Armazena o dado no pipe na posição de escrita atual
    p->pipe_data[p->pos_write] = data;
    
    // Atualiza a posição de escrita (circular, após o último índice volta ao início)
    p->pos_write = (p->pos_write + 1) % PIPE_SIZE;
    
    // Libera o semáforo de leitura, indicando que há dados disponíveis para leitura
    sem_post(&p->pipe_sem_read);    
}

// Função para ler um byte de dados do pipe
void pipe_read(pipe_t *p, byte *data)
{
    // Aguarda até que haja dados para ler no pipe
    sem_wait(&p->pipe_sem_read);
    
    // Lê o dado da posição de leitura atual do pipe
    *data = p->pipe_data[p->pos_read];
    
    // Atualiza a posição de leitura (circular, após o último índice volta ao início)
    p->pos_read = (p->pos_read + 1) % PIPE_SIZE;
    
    // Libera o semáforo de escrita, indicando que há espaço para escrever no pipe
    sem_post(&p->pipe_sem_write);    
}
