#ifndef HARDWARE_IO_H
#define HARDWARE_IO_H

#include <xc.h>

// Motor de Passo Functions
void motor_passo_step(int step);
void parar_motor_passo(void);
void configura_motor_passo(void);

// Motor DC Functions
void config_dc_motor(void);
void config_dip_switch(void);
void parar_motor(void);
void acionar_sentido_1(void);
void acionar_sentido_2(void);
void config_leds(void);

// ADC Configuration and Reading
void config_adc(void);
float read_temperature(void);

#endif /* HARDWARE_IO_H */