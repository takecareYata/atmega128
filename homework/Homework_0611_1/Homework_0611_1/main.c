/*
 * 01_LED_CONTROL.c
 *
 * Created: 2026-06-10 오전 10:24:41
 * Author : yata
 */ 

#define F_CPU	16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "button.h"
#include "led.h"

void (*led_cont_ary[])(void) =
{
	led_all_off,
	led_all_on,
	led_shift_left_on,
	led_shift_right_on,
	led_shift_left_keep_on,
	led_shift_right_keep_on,
	led_flower_on,
	led_flower_off
};

int main(void)
{
	int button0_state = 0;
	
	init_led();
	init_button();
	
	while (1)
	{ 
		if(get_button(BUTTON0,BUTTON0PIN))
		{
			//button0_state = !button0_state;
			button0_state++;
			if (button0_state == 8)
			{
				button0_state = 0;
			}
			flower_on_flag = 1;
			flower_off_flag = 1;
		}
		
		led_cont_ary[button0_state]();
		 
	}
	
	return 0;
}

