#include "user_app.h"      // Inclui o cabeçalho da aplicação do usuário
#include "pipe.h"          // Inclui o cabeçalho para a manipulação de pipe (comunicação entre tarefas)
#include "semaphore.h"     // Inclui o cabeçalho para a manipulação de semáforos
#include "kernel.h"        // Inclui o cabeçalho do kernel (núcleo do sistema operacional)
#include "hardware_io.h"   // Inclui o cabeçalho para a manipulação de hardware (periféricos)

// Definições de constantes
#define SEVERE 'S'   // Nível de temperatura 'Severe' (severo)
#define MODERATE 'M' // Nível de temperatura 'Moderate' (moderado)
#define NORMAL 'N'   // Nível de temperatura 'Normal' (normal)

#define STEP_180 '1'  // Comando para 180 graus no motor de passo
#define STEP_90 '0'   // Comando para 90 graus no motor de passo
#define STEP_0 '2'    // Comando para 0 graus no motor de passo

// Objetos globais para comunicação entre tarefas
pipe_t comunicacao;         // Pipe usado para comunicação entre tarefas
sem_t stepper_sem, dc_motor_sem;  // Semáforos para controle de motor de passo e motor DC
byte stepper_buffer, dc_motor_buffer; // Buffers para armazenar os dados dos motores

// Função para configurar a aplicação do usuário
void config_app_user() {
    config_leds();           // Configura os LEDs
    configura_motor_passo(); // Configura o motor de passo
    config_dc_motor();       // Configura o motor DC
    config_adc();            // Configura o ADC para leitura de temperatura

    pipe_init(&comunicacao);    // Inicializa o pipe para comunicação entre tarefas
    sem_init(&stepper_sem, 0);  // Inicializa o semáforo para o motor de passo
    sem_init(&dc_motor_sem, 0); // Inicializa o semáforo para o motor DC

    asm("GLOBAL _task_temperature_read, _task_general_control, _task_stepper_motor, _task_dc_motor"); // Declaração global das tarefas
}

// Tarefa 1: Leitura do Sensor de Temperatura
void task_temperature_read() {
    while (1) {
        float temperature = read_temperature();  // Lê a temperatura do sensor
        byte level;

        // Define o nível de temperatura com base no valor lido
        if (temperature < 23) {
            level = NORMAL;    // Temperatura normal
        } else if (temperature < 30) {
            level = MODERATE;  // Temperatura moderada
        } else {
            level = SEVERE;    // Temperatura severa
        }

        // Escreve o nível de temperatura no pipe para ser lido por outras tarefas
        pipe_write(&comunicacao, level);
        delay_task(10);  // Atraso para não sobrecarregar o processador
    }
}

// Tarefa 2: Controle Geral
void task_general_control() {
    byte temperature_level;

    while (1) {
        // Lê o nível de temperatura do pipe
        pipe_read(&comunicacao, &temperature_level);

        // Configura os LEDs e motores com base no nível de temperatura
        if (temperature_level == SEVERE) {
            LATBbits.LATB6 = 1;  // Acende o LED 6 (severo)
            LATBbits.LATB7 = 1;  // Acende o LED 7 (severo)
            stepper_buffer = STEP_180;  // Comando para motor de passo de 180 graus
            dc_motor_buffer = 1;  // Liga o motor DC
        } else if (temperature_level == MODERATE) {
            LATBbits.LATB6 = 0;  // Apaga o LED 6 (moderado)
            LATBbits.LATB7 = 1;  // Acende o LED 7 (moderado)
            stepper_buffer = STEP_90;   // Comando para motor de passo de 90 graus
            dc_motor_buffer = 1;  // Liga o motor DC
        } else { // NORMAL
            LATBbits.LATB6 = 1;  // Acende o LED 6 (normal)
            LATBbits.LATB7 = 0;  // Apaga o LED 7 (normal)
            stepper_buffer = STEP_0;    // Comando para motor de passo de 0 graus
            dc_motor_buffer = 0;  // Desliga o motor DC
        }

        // Libera os semáforos para as tarefas do motor de passo e motor DC
        sem_post(&stepper_sem);
        sem_post(&dc_motor_sem);
        delay_task(10);  // Atraso para evitar sobrecarga
    }
}

// Tarefa 3: Controle do Motor de Passo
void task_stepper_motor() {
    while (1) {
        // Aguarda o semáforo para controlar o motor de passo
        sem_wait(&stepper_sem);

        // Controla o motor de passo com base no comando recebido
        if (stepper_buffer == STEP_180) {
            motor_passo_step(1);  // 180 graus
        } else if (stepper_buffer == STEP_90) {
            motor_passo_step(0);  // 90 graus
        } else if (stepper_buffer == STEP_0) {
            motor_passo_step(2);  // 0 graus
        }

        delay_task(10);  // Atraso para evitar sobrecarga
    }
}

// Tarefa 4: Controle do Motor DC
void task_dc_motor() {
    while (1) {
        // Aguarda o semáforo para controlar o motor DC
        sem_wait(&dc_motor_sem);

        // Liga ou desliga o motor DC com base no valor de `dc_motor_buffer`
        if (dc_motor_buffer == 1) {
            acionar_sentido_1();  // Liga o motor DC no sentido 1
        } else {
            parar_motor();  // Desliga o motor DC
        }

        delay_task(10);  // Atraso para evitar sobrecarga
    }
}
