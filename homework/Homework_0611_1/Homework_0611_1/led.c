/*
 * led.c
 *
 * Created: 2026-06-10 오후 2:50:22
 *  Author: yata
 */ 

#include "led.h"

#if 0
int flower_on_flag = 0;
int flower_off_flag = 0;
#endif

void init_led(void)
{
	DDRA = 0xff;
	led_all_off();
}

void led_all_on(void)
{
	PORTA = 0xff;
}

void led_all_off(void)
{
	PORTA = 0x00;
}

void led_right_on(void)
{
	PORTA = 0x0f;
}

void led_left_on(void)
{
	PORTA = 0xf0;
}

void led_odd_on(void)
{
	PORTA =  1 << 1 | 1 << 3 | 1 << 5 | 1 << 7;
}

void led_even_on(void)
{
	PORTA =  1 << 0 | 1 << 2 | 1 << 4 | 1 << 6;
}

void led_shift_right_on(void)
{
	static int i = 0;
	
	PORTA = 0x80 >> i;
	_delay_ms(30);
	
	i = (i + 1) % 8; //다음 index 값을 계산
}

void led_shift_left_on(void)
{
	static int i = 0;
	
	PORTA = 1 << i;
	_delay_ms(30);
	
	i = (i + 1) % 8; //다음 index 값을 계산
}

void led_shift_left_keep_on(void)
{
#if 0
	
	for (int i = 0; i < 8; i++)
	{
		PORTA |= 1 << i;
		_delay_ms(30);
	}
#endif
	if (PORTA == 0xff)
	{
		PORTA = 0x0;
	}	
	static int i = 0;
	PORTA |= 1 << i;
	_delay_ms(30);
	i = (i + 1) % 8;
}

void led_shift_right_keep_on(void)
{
#if 0
	PORTA = 0x0;
	for (int i = 0; i < 8; i++)
	{
		PORTA |= 1 << (7 - i);
		_delay_ms(300);
	}
#endif
	if (PORTA == 0xff)
	{
		PORTA = 0x0;
	}
	static int i = 0;
	PORTA |= 1 << (7 - i);
	_delay_ms(30);
	i = (i + 1) % 8;
}

void led_flower_on(void)
{
#if 0
	PORTA = 0x0;
	for(int i = 0; i < 4 ; i++)
	{
		PORTA |= 1 << (4 - i - 1) | 1 << (4 + i);
		_delay_ms(30);
	}
#endif
#if 0
	if (flower_on_flag == 1)
	{
		PORTA = 0x0;
		flower_on_flag = 0;
	}
	if(PORTA == 0xff)
	{
		PORTA = 0x00;
	}
#endif
	static int i = 0;
	if (i == 0)
	{
		PORTA = 0x00;
	}
	PORTA |= 1 << (4 - i - 1) | 1 << (4 + i);
	_delay_ms(30);
	i = (i + 1) % 4;
}

void led_flower_off(void)
{
#if 0
	PORTA = 0xFF;
	for(int i = 0; i < 4 ; i++)
	{
		PORTA &= ~(1 << (7 - i) | 1 << (i));
		_delay_ms(30);
	}
#endif
#if 0
	if (flower_off_flag == 1)
	{
		PORTA = 0xff;
		flower_off_flag = 0;
	}
	if(PORTA == 0x00)
	{
		PORTA = 0xff;
	}
#endif
	static int i = 0;
	if (i == 0)
	{
		PORTA = 0xff;
	}
	PORTA &= ~(1 << (7 - i) | 1 << (i));
	_delay_ms(30);
	i = (i + 1) % 4;
}