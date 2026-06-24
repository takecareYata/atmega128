/*
 * button.h
 *
 * Created: 2026-06-19 오후 5:12:41
 *  Author: kccistc
 */ 
#ifndef BUTTON_H_
#define BUTTON_H_

#include <avr/io.h>

#define BUTTON_PRESS	1
#define BUTTON_RELEASE	0

#define BUTTON_DDR	DDRG
#define BUTTON_PIN	PING
#define BUTTON0PIN	4
#define BUTTON_NUMBER	4

void init_button(void);
void check_button_and_control_led(void);
int get_button(int butoon_num, int button_pin);

extern volatile uint32_t button_ms_count;

#endif /* BUTTON_H_ */