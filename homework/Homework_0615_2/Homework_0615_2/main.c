/*
 * Homework_0615_1.c
 *
 * Created: 2026-06-15 오후 2:40:23
 * Author : yata
 */ 

#define F_CPU	16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "fnd.h"
#include "timer.h"
#include "button.h"

volatile uint32_t ms_count = 0;
volatile uint32_t stopwatch_ms_count = 0;
volatile uint32_t stopwatch_sec_count = 0;
volatile int32_t sec_count = 0; //sec를 재는 count
volatile uint8_t dot_display = 0;
volatile uint32_t button0_count = 0;
volatile uint8_t stopwatch_state = 0;
volatile uint32_t button_ms_count = 0;

void (*button_cont_ary[])(void) =
{
	fnd_display,
	fnd_sec_clock,
	fnd_stopwatch
};


ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6; // TCNT0가 6부터 256이 되는 순간이 250개 펄스를 카운트하게 된다.
	ms_count++; // 4ms count
	button0_count++;
	button_ms_count++;
	
	if(ms_count >= 1000)
	{
		ms_count = 0;
		sec_count++;
		dot_display = !dot_display;
	}
	
	if (stopwatch_state) // 스톱워치가 켜져있을 때만 카운트 증가!
	{
		stopwatch_ms_count++;
		if(stopwatch_ms_count >= 1000)
		{
			stopwatch_ms_count = 0;
			stopwatch_sec_count++;
		}
	}
}

ISR(TIMER2_OVF_vect)
{
	TCNT2 = 6;
	// 초가 늦어진다????????
#if 0
	if (stopwatch_state) // 스톱워치가 켜져있을 때만 카운트 증가!
	{
		stopwatch_ms_count++;
		if(stopwatch_ms_count >= 1000)
		{
			stopwatch_ms_count = 0;
			stopwatch_sec_count++;
		}
	}
#endif
}

int main(void)
{
	int button0_state = 0;
	
	init_timer0();
	init_timer2();
	init_button();
	init_fnd();
	
    while (1) 
    {
		if(button0_count >= 1 )
		{
			button0_count = 0;
			
			if(get_button(BUTTON0,BUTTON0PIN))
			{
				button0_state = (button0_state + 1) % 3;
			}

			button_cont_ary[button0_state]();
		}
    }
}

