/*
 * led.c
 *
 * Created: 2026-06-10 오후 2:50:22
 *  Author: yata
 */ 

#include "led.h"

void (* led_fp[])(void) = 
{
	led_all_on,
	led_all_off,
	led_odd_on,
	led_even_on,
	led_shift_left_on,	//func_state = 0
	led_shift_right_on,
	led_shift_left_keep_on,	
	led_shift_right_keep_on,
	led_flower_on,
	led_flower_off		//func_state = 5
};

int func_state = 0;
#define FUNC_STATE	10

extern volatile uint32_t msec_count;

int led_main(void)
{
	init_led();
	led_all_off();
	
	while(1)
	{
		led_fp[func_state]();
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
	static int i = 0;
	
	if (msec_count >= 100 )
	{
		msec_count = 0;
		PORTA=0xff;
		if( (i = (i + 1) % 8) == 0)
		{
			func_state = (func_state + 1) % FUNC_STATE; // 다음 실행한 func으로 jump
		}
	}
}

void led_all_off(void)
{
	static int i = 0;
	
	if (msec_count >= 100 )
	{
		msec_count = 0;
		PORTA=0x00;
		if( (i = (i + 1) % 8) == 0)
		{
			func_state = (func_state + 1) % FUNC_STATE; // 다음 실행한 func으로 jump
		}
	}
}

void led_odd_on(void)
{
	static int i = 0;
	
	if (msec_count >= 100 )
	{
		msec_count = 0;
		PORTA =  1 << 1 | 1 << 3 | 1 << 5 | 1 << 7;
		if( (i = (i + 1) % 2) == 0)
		{
			func_state = (func_state + 1) % FUNC_STATE; // 다음 실행한 func으로 jump
		}
	}
	
}

void led_even_on(void)
{
	static int i = 0;
	
	if (msec_count >= 100 )
	{
		msec_count = 0;
		PORTA =  1 << 0 | 1 << 2 | 1 << 4 | 1 << 6;
		if( (i = (i + 1) % 2) == 0)
		{
			func_state = (func_state + 1) % FUNC_STATE; // 다음 실행한 func으로 jump
		}
	}
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