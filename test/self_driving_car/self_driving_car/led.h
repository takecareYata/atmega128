/*
 * led.h
 *
 * Created: 2026-06-19 오후 5:12:14
 *  Author: kccistc
 */ 
#ifndef LED_H_
#define LED_H_

#include <avr/io.h>

#define LED_ON()  (PORTG |= (1 << PG3))
#define LED_OFF() (PORTG &= ~(1 << PG3))

void init_led(void);



#endif /* LED_H_ */