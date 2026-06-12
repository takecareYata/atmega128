/*
 * led.c
 *
 * Created: 2026-06-10 오후 3:10:22
 *  Author: user
 */ 

#include "led.h"
void init_led(void);
void led_all_on(void);
void led_all_off(void);
void led_shift_left_on(void);
void led_shift_right_on(void);
void led_shift_left_keep_on(void);
void led_shift_right_keep_on(void);
void led_flower_on(void);
void led_flower_off(void);
int led_main(void);

int led_main(void)
{
	led_all_off();
	while(1)
	{
		led_shift_left_on();
		led_shift_right_on();
		led_shift_left_keep_on();
		led_shift_right_keep_on();
		led_flower_on();
		led_flower_off();
	}
	return 0;
}

void init_led(void)
{
	DDRA=0xff;  // PORTA 를 출력 모드로 설정
	PORTA=0x00;  // PORTA에 물려있는 led를 all off
}

void led_all_on(void)
{
	PORTA=0xff;
}

void led_all_off(void)
{
	PORTA=0x00;
}

void led_shift_left_on(void)
{
#if 1
	static int i = 0;
	
	PORTA = 1 << i;
	_delay_ms(30);
	
	i = (i + 1) % 8; //다음 index 값을 계산

#else
	for (int i = 0; i < 8; i++)
	{
		PORTA = 1 << i; 
		_delay_ms(30);
	}
#endif
}

void led_shift_right_on(void)
{
#if 1
	static int i = 0;
	
	PORTA = 0x80 >> i;
	_delay_ms(30);	
	
	i = (i + 1) % 8; //다음 index 값을 계산
#else
	for (int i = 8; i > 0; i--)
	{
		PORTA = 1 << (i - 1); 
//		*(unsigned char *)0x3B = 1 << (i - 1); // hw번지 직접 접근
		_delay_ms(30);
	}
	/*
	for (int i = 0; i < 8; i++)
	{
		PORTA = 0x80 >> i;
		_delay_ms(30);
	}
	*/
#endif
}

void led_shift_left_keep_on(void)
{
	PORTA = 0x0;
	for (int i = 0; i < 8; i++)
	{
		PORTA |= 1 << i; 
		_delay_ms(300);
	}
}

void led_shift_right_keep_on(void)
{
	PORTA = 0x0;
	for (int i = 8; i > 0; i--)
	{
		PORTA |= 1 << (i - 1); 
		_delay_ms(300);
	}
}

void led_flower_on(void)
{
	PORTA = 0x0;
	for(int i = 0; i < 4 ; i++)
	{
		PORTA |= 1 << (4 - i - 1) | 1 << (4 + i);
		_delay_ms(300);
	}
}

void led_flower_off(void)
{
	PORTA = 0xFF;
	for(int i = 0; i < 4 ; i++)
	{
		PORTA &= ~(1 << (7 - i) | 1 << (i));
		_delay_ms(300);
	}
	
}