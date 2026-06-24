/*
 * pwm.h
 *
 * Created: 2026-06-18 오전 11:40:29
 *  Author: kccistc
 */ 
#ifndef PWM_H_
#define PWM_H_
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "button.h"

extern void init_pwm_pb4(void);
extern void init_pwm_pb5Npb6(void);
extern void init_timer3_pwm(void);
extern void init_motor_driver(void);
extern void dcmotor_pwm_control_main(void);

#endif /* PWM_H_ */