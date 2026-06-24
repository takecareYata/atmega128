/*
 * Homework_0615_1.c
 *
 * Created: 2026-06-15 오후 2:40:23
 * Author : kccistc
 */ 

#define F_CPU	16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "led.h"
#include "timer.h"

volatile uint32_t msec_count = 0;
volatile uint32_t stopwatch_msec_count = 0;

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6; // TCNT0가 6부터 256이 되는 순간이 250개 펄스를 카운트하게 된다.
	msec_count++; // 1ms count
}

ISR(TIMER2_OVF_vect)
{
	TCNT2 = 6;
	stopwatch_msec_count++;
}


int main(void)
{
	init_timer0();
	init_timer2();
	init_led();
	
	led_main();
    while (1) 
    {
		
    }
}

