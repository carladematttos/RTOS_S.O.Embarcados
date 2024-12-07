#include <xc.h>             // Inclui a biblioteca de cabe�alhos para o microcontrolador
#include "kernel.h"         // Inclui o cabe�alho do kernel do sistema operacional de tempo real (RTOS)
#include "user_app.h"       // Inclui o cabe�alho da aplica��o do usu�rio (onde as tarefas s�o definidas)

// Fun��o principal do programa
int main() {
    // Passo 1: Configura o sistema operacional de tempo real (RTOS) e inicializa o hardware
    setup_os();  // Inicializa o sistema operacional e prepara o ambiente para a execu��o das tarefas

    // Passo 2: Cria as tarefas definidas pelo usu�rio
    // A cria��o das tarefas envolve associar um identificador �nico, a prioridade e a fun��o que ser� executada
    create_task(1, 0, task_temperature_read);  // Cria a tarefa de leitura do sensor de temperatura (ID = 1, prioridade = 0)
    create_task(2, 0, task_general_control);  // Cria a tarefa de controle geral (ID = 2, prioridade = 0)
    create_task(3, 0, task_stepper_motor);    // Cria a tarefa de controle do motor de passo (ID = 3, prioridade = 0)
    create_task(4, 0, task_dc_motor);         // Cria a tarefa de controle do motor DC (ID = 4, prioridade = 0)

    // Passo 3: Inicia o sistema operacional (RTOS)
    start_os();  // Inicia o RTOS, que gerenciar� a execu��o das tarefas criadas

    // La�o infinito gerenciado pelo RTOS
    // O RTOS ser� respons�vel por gerenciar a execu��o das tarefas, ent�o o loop principal fica vazio
    while (1) {
        // O RTOS chamar� as fun��es das tarefas conforme necess�rio
        // Este loop principal n�o precisa fazer nada, pois o RTOS cuida de tudo
    }

    return 0;  // O programa nunca deve alcan�ar este ponto, pois o RTOS est� em execu��o
}
