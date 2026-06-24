/*
 * button.h
 *
 * Created: 2026-06-10 오전 11:45:03
 *  Author: yata
 */ 
#ifndef BUTTON_H_
#define BUTTON_H_
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define BUTTON_DDR	DDRD
#define BUTTON_PIN	PIND
#define BUTTON0PIN	3
#define BUTTON1PIN	4
#define BUTTON2PIN	5
#define BUTTON3PIN	6

#define BUTTON0	0
#define BUTTON1	1
#define BUTTON2	2
#define BUTTON3	3
#define BUTTON_NUMBER	4

#define BUTTON_PRESS	1
#define BUTTON_RELEASE	0

extern void init_button(void);
extern int get_button(int butoon_num, int button_pin);

extern volatile uint32_t button_ms_count;

#endif /* BOTTON_H_ */