#ifndef __DCMOTOR_H__
#define __DCMOTOR_H__
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <lvgl.h>
#include "delay.h"
#include "stm32f4xx_hal.h"



#define SHUTDOWN1_Pin    GET_PIN(F, 10)

#define ShutDonwPinInit rt_pin_mode(SHUTDOWN1_Pin, PIN_MODE_OUTPUT);
#define ENABLE_MOTOR    rt_pin_write(SHUTDOWN1_Pin, PIN_HIGH);
#define DISABLE_MOTOR   rt_pin_write(SHUTDOWN1_Pin, PIN_LOW);

void dcmotor_init(uint16_t period);
void dcmotor_start(void);
void dcmotor_stop(void);
void dcmotor_dir(uint8_t para);
void dcmotor_speed(uint16_t para);
void motor_pwm_set(float para);

#endif