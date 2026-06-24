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

int func_state = 0;
#define FUNC_STATE	6

void (* fp[])(void) = 
{
	led_shift_left_on,	//func_state = 0
	led_shift_right_on,
	led_shift_left_keep_on,	
	led_shift_right_keep_on,
	led_flower_on,
	led_flower_off		//func_state = 5
};

extern volatile uint32_t msec_count;

int led_main(void)
{
	uint8_t led_toggle = 0;
	init_led();
	led_all_off();
	
	while(1)
	{
#if 1
		fp[func_state]();
		
#else		
		if (msec_count >= 500)
		{
			msec_count = 0;
			led_toggle = !led_toggle;
		}
		
		if(led_toggle)
		{
			led_all_on();
		}
		else
		{
			led_all_off();
		}
#endif
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
	// timer iSR을 이용한 방식
	static int i = 0;
	
	if (msec_count >= 100 )
	{
		msec_count = 0;
		PORTA = 1 << i;
		if( (i = (i + 1) % 8) == 0)
		{
			func_state = (func_state + 1) % FUNC_STATE; // 다음 실행한 func으로 jump
		} 
	}

}

void led_shift_right_on(void)
{
// timer iSR을 이용한 방식
	static int i = 0;
	
	if (msec_count >= 100 )
	{
		msec_count = 0;
		PORTA = 0x80 >> i;
		if( (i = (i + 1) % 8) == 0)
		{
			func_state = (func_state + 1) % FUNC_STATE; // 다음 실행한 func으로 jump
		}
	}
}

void led_shift_left_keep_on(void)
{
	
	static int i = 0;
	
	if (msec_count >= 100)
	{
		if (PORTA == 0xff)
		{
			PORTA = 0x0;
		}
		msec_count = 0;
		PORTA |= 1 << i;
		if( (i = (i + 1) % 8) == 0)
		{
			func_state = (func_state + 1) % FUNC_STATE; // 다음 실행한 func으로 jump
		}
	}
}

void led_shift_right_keep_on(void)
{
	static int i = 0;
	
	if (msec_count >= 100)
	{
		if (PORTA == 0xff)
		{
			PORTA = 0x0;
		}
		msec_count = 0;
		PORTA |= 1 << (7 - i);
		if( (i = (i + 1) % 8) == 0)
		{
			func_state = (func_state + 1) % FUNC_STATE; // 다음 실행한 func으로 jump
		}
	}
}

void led_flower_on(void)
{
	static int i = 0;
	
	if (msec_count >= 100)
	{
		msec_count = 0;
		if (i == 0)
		{
			PORTA = 0x00;
		}
		PORTA |= 1 << (4 - i - 1) | 1 << (4 + i);
		if( (i = (i + 1) % 4) == 0)
		{
			func_state = (func_state + 1) % FUNC_STATE; // 다음 실행한 func으로 jump
		}
	}
}

void led_flower_off(void)
{
	static int i = 0;
	if (msec_count >= 100)
	{
		msec_count = 0;
		if (i == 0)
		{
			PORTA = 0xff;
		}
		PORTA &= ~(1 << (7 - i) | 1 << (i));
		if( (i = (i + 1) % 4) == 0)
		{
			func_state = (func_state + 1) % FUNC_STATE; // 다음 실행한 func으로 jump
		}
	}
}