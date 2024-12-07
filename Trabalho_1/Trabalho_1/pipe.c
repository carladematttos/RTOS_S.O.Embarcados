#include "pipe.h"          // Inclui o cabe�alho para manipula��o de pipes
#include "semaphore.h"     // Inclui o cabe�alho para manipula��o de sem�foros

// Fun��o para inicializar o pipe
// Inicializa as posi��es de leitura e escrita, al�m dos sem�foros que controlam o acesso ao pipe
void pipe_init(pipe_t *p)
{
    p->pos_read     = 0;  // Inicializa a posi��o de leitura como 0 (in�cio do buffer)
    p->pos_write    = 0;  // Inicializa a posi��o de escrita como 0 (in�cio do buffer)
    
    // Inicializa os sem�foros para controlar o fluxo de dados
    // sem�foro de escrita: inicializado com o tamanho do pipe (permitindo que o pipe esteja cheio at� seu tamanho m�ximo)
    sem_init(&p->pipe_sem_write, PIPE_SIZE);
    
    // sem�foro de leitura: inicializado com 0 (n�o � poss�vel ler at� que haja algo escrito no pipe)
    sem_init(&p->pipe_sem_read, 0);
}

// Fun��o para escrever um byte de dados no pipe
void pipe_write(pipe_t *p, byte data)
{
    // Aguarda at� que haja espa�o dispon�vel para escrever no pipe
    sem_wait(&p->pipe_sem_write);
    
    // Armazena o dado no pipe na posi��o de escrita atual
    p->pipe_data[p->pos_write] = data;
    
    // Atualiza a posi��o de escrita (circular, ap�s o �ltimo �ndice volta ao in�cio)
    p->pos_write = (p->pos_write + 1) % PIPE_SIZE;
    
    // Libera o sem�foro de leitura, indicando que h� dados dispon�veis para leitura
    sem_post(&p->pipe_sem_read);    
}

// Fun��o para ler um byte de dados do pipe
void pipe_read(pipe_t *p, byte *data)
{
    // Aguarda at� que haja dados para ler no pipe
    sem_wait(&p->pipe_sem_read);
    
    // L� o dado da posi��o de leitura atual do pipe
    *data = p->pipe_data[p->pos_read];
    
    // Atualiza a posi��o de leitura (circular, ap�s o �ltimo �ndice volta ao in�cio)
    p->pos_read = (p->pos_read + 1) % PIPE_SIZE;
    
    // Libera o sem�foro de escrita, indicando que h� espa�o para escrever no pipe
    sem_post(&p->pipe_sem_write);    
}
