/*
 * fnd.c
 *
 * Created: 2026-06-12 오전 10:52:39
 *  Author: yata
 */ 
#include "fnd.h"
#include "button.h"

uint8_t fnd_font[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8,0x80, 0x98, 0x7f};//0 ~ .
	
unsigned char d1_ani_font[] = {~0x01, ~0x00, ~0x00, ~0x00, ~0x08, ~0x06};
unsigned char d2_ani_font[] = {~0x00, ~0x01, ~0x30, ~0x08, ~0x00, ~0x00};

void init_fnd(void)
{
	FND_DATA_DDR = 0xff;
	FND_DIGIT_DDR |= 1 << FND_DIGIT_D1 | 1 << FND_DIGIT_D2 | 1 << FND_DIGIT_D3 | 1 << FND_DIGIT_D4;
	FND_DATA_PORT = 0xff;
}

void fnd_dot_display(void)
{
	if(dot_display == 1)
	{
		FND_DATA_PORT &= fnd_font[10];
	}
}

void fnd_display(void)
{
	
	static int digit_select = 0;
	
	switch(digit_select)
	{
		case 0:
		FND_DIGIT_PORT = 0x80;
		FND_DATA_PORT = fnd_font[sec_count%10];
		break;
		
		case 1:
		FND_DIGIT_PORT = 0x40;
		FND_DATA_PORT = fnd_font[sec_count/10%6];
		fnd_dot_display();
		break;
		
		case 2:
		FND_DIGIT_PORT = 0x20;
		FND_DATA_PORT = fnd_font[sec_count/60%10];
		break;
		
		case 3:
		FND_DIGIT_PORT = 0x10;
		FND_DATA_PORT = fnd_font[sec_count/600%6];
		break;
		
		default:
		break;
	}
	
	digit_select = (digit_select + 1) % 4;
}

void fnd_sec_clock(void)
{
	static int digit_select = 0;
	int ani_cnt = sec_count % 6;
	
	switch(digit_select)
	{
		case 0:
		FND_DIGIT_PORT = 0x80;
		FND_DATA_PORT = fnd_font[sec_count%10];
		break;
		
		case 1:
		FND_DIGIT_PORT = 0x40;
		FND_DATA_PORT = fnd_font[sec_count/10%6];
		break;
		
		case 2:
		FND_DIGIT_PORT = 0x20;
		FND_DATA_PORT = d1_ani_font[ani_cnt];
		break;
		
		case 3:
		FND_DIGIT_PORT = 0x10;
		FND_DATA_PORT = d2_ani_font[ani_cnt];
		break;
		
		default:
		break;
	}
	
	digit_select = (digit_select + 1) % 4;
	
}

void fnd_stopwatch(void)
{
	static int digit_select = 0;
	
	if(get_button(BUTTON1,BUTTON1PIN))
	{
		stopwatch_state = !stopwatch_state;
	}
	
	if(get_button(BUTTON2,BUTTON2PIN))
	{
		stopwatch_ms_count = 0;
		stopwatch_sec_count = 0;
		stopwatch_state = !stopwatch_state;
		FND_DIGIT_PORT = 0xf0;
		FND_DATA_PORT = fnd_font[0];
	}
	
	switch(digit_select)
	{
		case 0:
		FND_DIGIT_PORT = 0x80;
		FND_DATA_PORT = fnd_font[stopwatch_ms_count/10%10];
		break;
			
		case 1:
		FND_DIGIT_PORT = 0x40;
		FND_DATA_PORT = fnd_font[stopwatch_ms_count/100%10];
		break;
			
		case 2:
		FND_DIGIT_PORT = 0x20;
		FND_DATA_PORT = fnd_font[stopwatch_sec_count%10];
		break;
		
		case 3:
		FND_DIGIT_PORT = 0x10;
		FND_DATA_PORT = fnd_font[stopwatch_sec_count/10%6];
		break;
		
		default:
		break;
	}

	digit_select = (digit_select + 1) % 4;
}