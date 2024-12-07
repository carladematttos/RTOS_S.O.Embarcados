#ifndef USER_APP_H
#define USER_APP_H

#include <xc.h>
#include "types.h"

void config_app_user(void);
void task_general_control(void);
void task_temperature_read(void);
void task_stepper_motor(void);
void task_dc_motor(void);

#endif /* USER_APP_H */