#include "hardware_io.h"

// Função para controlar cada passo do motor de passo
void motor_passo_step(int step) {
    // A função recebe o número do passo e configura os pinos de PORTC
    // para controlar o motor de passo.
    switch (step) {
        case 0: PORTC = 0b00000001; break; // Passo 1
        case 1: PORTC = 0b00000100; break; // Passo 2
        case 2: PORTC = 0b00000010; break; // Passo 3
        case 3: PORTC = 0b00001000; break; // Passo 4
    }
}

// Função para parar o motor de passo
void parar_motor_passo(void) {
    PORTC = 0b00000000; // Reseta todos os pinos de PORTC (desliga o motor)
}

// Função para configurar os pinos do motor de passo
void configura_motor_passo(void) {
    TRISC = 0b11110000; // Configura os 4 primeiros pinos de PORTC como saída (motor de passo)
    PORTC = 0b00000000; // Limpa PORTC (garante que o motor está desligado inicialmente)
}

// Função para configurar os pinos do motor DC
void config_dc_motor(void) {
    TRISCbits.RC4 = 0; // Configura o pino RC4 como saída (direção do motor DC)
    TRISCbits.RC5 = 0; // Configura o pino RC5 como saída (direção do motor DC)
}

// Função para parar o motor DC
void parar_motor(void) {
    PORTCbits.RC4 = 0; // Desliga o pino RC4 (interrompe direção 1 do motor DC)
    PORTCbits.RC5 = 0; // Desliga o pino RC5 (interrompe direção 2 do motor DC)
}

// Função para acionar o motor DC no sentido 1
void acionar_sentido_1(void) {
    PORTCbits.RC4 = 1; // Liga o pino RC4 (direção 1 do motor DC)
    PORTCbits.RC5 = 0; // Desliga o pino RC5
}

// Função para acionar o motor DC no sentido 2
void acionar_sentido_2(void) {
    PORTCbits.RC4 = 0; // Desliga o pino RC4
    PORTCbits.RC5 = 1; // Liga o pino RC5 (direção 2 do motor DC)
}

// Função para configurar os pinos dos LEDs
void config_leds() {
    TRISBbits.TRISB6 = 0; // Configura o pino RB6 como saída (LED 1)
    TRISBbits.TRISB7 = 0; // Configura o pino RB7 como saída (LED 2)
}

// Função para configurar o ADC para o sensor de temperatura
void config_adc(void) {
    // Configura os pinos AN0 e AN1 como entradas analógicas
    ADCON1bits.PCFG = 0b1100;  // Configure AN0 e AN1 como entradas analógicas
    ADCON1bits.VCFG0 = 0;      // Usa Vdd como referência de tensão
    ADCON1bits.VCFG1 = 0;
    ADCON2bits.ADFM = 1;       // Alinha o resultado à direita (mais comum)
    ADCON2bits.ACQT = 0b111;   // Define o tempo de aquisição para 20 TAD
    ADCON2bits.ADCS = 0b100;   // Define o clock do ADC como Fosc/4
    ADCON0bits.ADON = 1;       // Habilita o módulo ADC
}

// Função para ler a temperatura do sensor LM35
float read_temperature(void) {
    ADCON0bits.CHS = 0b0001;  // Seleciona o canal AN1 (onde está o sensor LM35)
    ADCON0bits.GO = 1;        // Inicia a conversão ADC
    while (ADCON0bits.GODONE); // Espera até a conversão terminar

    // Combina os bits de ADRESH (8 bits mais significativos) e ADRESL (2 bits menos significativos)
    int adc_value = (ADRESH << 8) | ADRESL;
    // Converte o valor ADC para a temperatura em °C (10mV/°C)
    return (adc_value * 5.0f / 1023.0f) * 100.0f;
}
