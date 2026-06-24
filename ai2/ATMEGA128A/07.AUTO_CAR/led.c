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
void flower_on(void);
void flower_off(void);
int led_main(void);

extern volatile uint32_t msec_count;  // volatile 최적화 방지 
extern int func_state;

#define FUNC_SU 2


void (*fp[]) () =
{
	led_shift_left_on,  // func_state=0
	led_shift_right_on,  // 1
	led_shift_left_keep_on,  // 2
	led_shift_right_keep_on,  // 3
	flower_on,  // 4
	flower_off   // func_state=5
};

int led_main(void)
{
	uint8_t led_toggle=0;
	
	init_led();
	led_all_off();
	
	while(1)
	{
#if 1
		fp[func_state] ();
		
#else // org 
		if (msec_count >= 500)  // 500ms if (msec_count == 500)
		{
			msec_count=0;
			led_toggle = !led_toggle;
			if (led_toggle) 
				led_all_on();
			else led_all_off();
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
	static int i=0;
	
	if (msec_count >= 100)
	{
		msec_count=0;
		*(unsigned char *) 0x3B = 1 << i;     // PORTA : 0x3B
        i = (i + 1) % 8;
// 		if ( (i = (i + 1) % 8) == 0)   // 다음 index 값을 계산
// 			func_state = (func_state + 1) % FUNC_SU;  // 다음 실행할 func으로 jump
	}
}


void led_shift_right_on(void)
{
	static int i=0;
	
	if (msec_count >= 100)
	{
		msec_count=0;
		*(unsigned char *) 0x3B = 0x80 >> i;     // PORTA : 0x3B
        i = (i + 1) % 8;
// 		if ( (i = (i + 1) % 8) == 0)   // 다음 index 값을 계산
// 		func_state = (func_state + 1) % FUNC_SU;  // 다음 실행할 func으로 jump
	}

}


void flower_on()
{
	PORTA=0;
	for (int i=0; i < 4; i++)
	{
		PORTA |= 0x10 << i | 0x08 >> i;
		_delay_ms(50);
	}
}

void flower_off()
{
	for (int i=0; i < 4; i++)
	{
		PORTA = ( ((0xf0 >> i) & 0xf0) | ((0x0f << i) & 0x0f));
		_delay_ms(300);
	}
	PORTA=0;
	_delay_ms(300);
}

void led_shift_left_keep_on(void)
{
	PORTA = 0;
	for (int i=0; i < 8; i++)
	{
		PORTA |= 0b00000001 << i;
		_delay_ms(300);
	}
}
void led_shift_right_keep_on(void)
{
	PORTA = 0;
	for (int i=0; i < 8; i++)
	{
		PORTA |= 0b10000000 >> i;
		_delay_ms(300);
	}
}