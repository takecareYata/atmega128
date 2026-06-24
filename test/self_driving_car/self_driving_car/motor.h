/*
 * motor.h
 *
 * Created: 2026-06-19 오후 5:10:16
 *  Author: kccistc
 */ 
#ifndef MOTOR_H_
#define MOTOR_H_

#include <avr/io.h>

void init_motor(void);
void motor_forward(uint8_t speed);
void motor_backward(uint8_t speed);
void motor_left(uint8_t speed);
void motor_right(uint8_t speed);
void motor_stop(void);

#endif /* MOTOR_H_ */