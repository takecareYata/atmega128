/*
 * led.h
 *
 * Created: 2026-06-10 오후 2:50:57
 *  Author: yata
 */ 
#ifndef LED_H_
#define LED_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

extern void init_led(void);
extern void led_all_on(void);
extern void led_all_off(void);
extern void led_right_on(void);
extern void led_left_on(void);
extern void led_odd_on(void);
extern void led_even_on(void);
extern void led_shift_right_on(void);
extern void led_shift_left_on(void);
extern void led_shift_right_keep_on(void);
extern void led_shift_left_keep_on(void);
extern void led_flower_on(void);
extern void led_flower_off(void);

extern int flower_on_flag;
extern int flower_off_flag;

#endif /* LED_H_ */