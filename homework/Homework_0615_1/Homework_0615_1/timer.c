/*
 * timer.c
 *
 * Created: 2026-06-15 오후 2:54:25
 *  Author: kccistc
 */ 
#define F_CPU	16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "timer.h"

void init_timer0(void)
{
	TCNT0 = 6; // TCNT0가 6부터 256이 되는 순간이 250개 펄스를 카운트하게 된다.
	
	TCCR0 &= ~(1 << CS02 | 1 << CS01 | 1 << CS00); // 0분주 초기화
	TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00; //64분주 세팅
	TIMSK |= 1 << TOIE0; // 오버플로 인터럽트
	sei(); // 전역 interrupt 허용
}

void init_timer2(void)
{
	TCNT2 = 6; // TCNT2가 6부터 256이 되는 순간이 250개 펄스를 카운트하게 된다.
	
	TCCR2 &= ~(1 << CS22 | 1 << CS21 | 1 << CS20); // 0분주 초기화
	TCCR2 |= 1 << CS22 | 0 << CS21 | 0 << CS20; //64분주 세팅
	TIMSK |= 1 << TOIE2; // 오버플로 인터럽트
	sei(); // 전역 interrupt 허용
}