#include <xc.h>             // Inclui a biblioteca de cabeçalhos para o microcontrolador
#include "kernel.h"         // Inclui o cabeçalho do kernel do sistema operacional de tempo real (RTOS)
#include "user_app.h"       // Inclui o cabeçalho da aplicação do usuário (onde as tarefas são definidas)

// Função principal do programa
int main() {
    // Passo 1: Configura o sistema operacional de tempo real (RTOS) e inicializa o hardware
    setup_os();  // Inicializa o sistema operacional e prepara o ambiente para a execução das tarefas

    // Passo 2: Cria as tarefas definidas pelo usuário
    // A criação das tarefas envolve associar um identificador único, a prioridade e a função que será executada
    create_task(1, 0, task_temperature_read);  // Cria a tarefa de leitura do sensor de temperatura (ID = 1, prioridade = 0)
    create_task(2, 0, task_general_control);  // Cria a tarefa de controle geral (ID = 2, prioridade = 0)
    create_task(3, 0, task_stepper_motor);    // Cria a tarefa de controle do motor de passo (ID = 3, prioridade = 0)
    create_task(4, 0, task_dc_motor);         // Cria a tarefa de controle do motor DC (ID = 4, prioridade = 0)

    // Passo 3: Inicia o sistema operacional (RTOS)
    start_os();  // Inicia o RTOS, que gerenciará a execução das tarefas criadas

    // Laço infinito gerenciado pelo RTOS
    // O RTOS será responsável por gerenciar a execução das tarefas, então o loop principal fica vazio
    while (1) {
        // O RTOS chamará as funções das tarefas conforme necessário
        // Este loop principal não precisa fazer nada, pois o RTOS cuida de tudo
    }

    return 0;  // O programa nunca deve alcançar este ponto, pois o RTOS está em execução
}
