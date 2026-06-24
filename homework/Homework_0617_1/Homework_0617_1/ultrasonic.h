/*
 * ultrasonic.h
 *
 * Created: 2026-06-17 오후 1:32:50
 *  Author: kccistc
 */ 
#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#define F_CPU	16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define TRIG_DDR	DDRG
#define TRIG_PORT	PORTG
#define TRIG_PIN	4

#define ECHO_DDR	DDRE
#define ECHO_PORT	PINE
#define ECHO_PIN	4

extern void init_ultrasonic(void);
extern void make_trigger(void);
extern void ultrasonic_processsing(void);
extern void ultrasonic_show_led(void);

#endif /* ULTRASONIC_H_ */