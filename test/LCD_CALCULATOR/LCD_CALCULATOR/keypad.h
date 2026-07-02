/*
 * keypad.h
 *
 * Created: 2026-06-29 오후 2:11:23
 *  Author: kccistc
 */ 
#ifndef KEYPAD_H_
#define KEYPAD_H_


#define F_CPU	16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define	KEYPAD_DDR	DDRA
#define KEYPAD_PIN	PINA
#define KEYPAD_PORT	PORTA

void init_keypad(void);
uint8_t keypad_scan(void);
uint8_t keypad_get_button(int row, int col);

extern volatile uint32_t lcd_time_count;


#endif /* KEYPAD_H_ */