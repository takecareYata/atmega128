/*
 * Homework_0612_1.c
 *
 * Created: 2026-06-12 오후 1:10:26
 * Author : yata
 */ 
#define F_CPU	16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "button.h"
#include "led.h"
#include "fnd.h"

void (*button_cont_ary[])(void) =
{
	fnd_display,
	fnd_sec_clock,
	fnd_stopwatch
};

int main(void)
{
	int button0_state = 0;
	
	init_led();
	init_button();
	init_fnd();
	
	while (1)
	{
		if(get_button(BUTTON0,BUTTON0PIN))
		{
			button0_state = (button0_state + 1) % 3;
		}
		
		button_cont_ary[button0_state]();
		fnd_timer();
	}
	
	return 0;
}

