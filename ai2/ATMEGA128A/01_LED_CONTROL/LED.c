/*
 * led.c
 *
 * Created: 2026-06-10 오후 2:50:22
 *  Author: yata
 */ 

#include "led.h"

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
	if(PORTA == 0xAA)
	{
		PORTA >>= 1;
	}
	else
	{
		PORTA = 0x55;
	}
}

void led_even_on(void)
{
	if(PORTA == 0x55)
	{
		PORTA <<= 1;
	}
	else
	{
		PORTA = 0xAA;
	}
}