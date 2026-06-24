/*
 * timer.h
 *
 * Created: 2026-06-19 오후 5:25:14
 *  Author: kccistc
 */ 
#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

extern volatile uint32_t clock_ms_count;
extern volatile uint32_t clock_sec_count;

void init_timer0(void);


#endif /* TIMER_H_ */