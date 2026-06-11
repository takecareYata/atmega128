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

#define HOMEWORK_0610
#ifdef HOMEWORK_0610

int main(void)
{
	int button0_state = 0;
	int button1_state = 0;
	
	init_led();
	init_button();
	
	while (1)
	{
		 // polling 방식
		 
		if(get_button(BUTTON0,BUTTON0PIN))
		{
			//button0_state = !button0_state;
			button0_state++;
		}
		// 위에서는 이벤트처리 만하고, if문 밖에서 제어를 진행한다. 
		if(button0_state % 4 == 0)
		{
			led_all_off();
		}
		else if(button0_state % 4 == 1)
		{
			led_all_on();
		}
		else if(button0_state % 4 == 2)
		{
			led_right_on();
		}
		else if(button0_state % 4 == 3)
		{
			led_left_on();
		}
		
		if(get_button(BUTTON1,BUTTON1PIN))
		{
			button1_state++;
		}
		// 위에서는 이벤트처리 만하고, if문 밖에서 제어를 진행한다.
		if(button1_state % 4 == 0)
		{
			led_all_off();
		}
		else if(button1_state % 4 == 1)
		{
			led_odd_on();
		}
		else if(button1_state % 4 == 2)
		{
			led_even_on();
		}
		else if(button1_state % 4 == 3)
		{
			led_all_on();
		}
		
		if(get_button(BUTTON3,BUTTON3PIN))
		{
			button0_state = 0;
			button1_state = 0;
			led_all_off();
		}
	}
	
	return 0;
}
#endif
#if 0
int main(void)
{
	
	DDRA = 0xFF; // DDR (Data Direction Register) 출력제어 ( 1 out , 0 in )
	
	
    while (1) 
    {
		PORTA |= 0xFF;
		_delay_ms(1000);
		PORTA &= 0x00;
		_delay_ms(1000);
    }
	
	return 0;
}
#endif