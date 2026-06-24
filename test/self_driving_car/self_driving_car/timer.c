/*
 * timer.c
 *
 * Created: 2026-06-19 오후 5:25:03
 *  Author: kccistc
 */ 
#include "timer.h"
#include "fnd.h"

volatile uint32_t button_ms_count = 0;
volatile uint32_t ultrasonic_ms_count = 0;
volatile uint32_t fnd_ms_count = 0;
volatile uint32_t clock_ms_count = 0;
volatile uint32_t clock_sec_count = 0;

ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6; // TCNT0가 6부터 256이 되는 순간이 250개 펄스를 카운트하게 된다.
	// 1/16000000Hz * 64 * 250 = 1ms
	button_ms_count++;
	clock_ms_count++;
	ultrasonic_ms_count++;
	fnd_ms_count++;
	
	if (clock_ms_count >= 1000)
	{
		clock_ms_count = 0;
		clock_sec_count++;
	}
}

void init_timer0(void)
{
	TCNT0 = 6; // TCNT0가 6부터 256이 되는 순간이 250개 펄스를 카운트하게 된다.
	
	TCCR0 &= ~(1 << CS02 | 1 << CS01 | 1 << CS00); // 0분주 초기화
	TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00; //64분주 세팅
	TIMSK |= 1 << TOIE0; // 오버플로 인터럽트
}