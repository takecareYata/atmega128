/*
 * fnd.c
 *
 * Created: 2026-06-12 오전 10:52:39
 *  Author: kccistc
 */ 
#include "fnd.h"

void init_fnd(void);
void fnd_display(void);
int fnd_main(void);

uint32_t ms_count = 0; //ms를 재는 count
uint32_t sec_count = 0; //sec를 재는 count
uint8_t	dot_display = 0;

int fnd_main(void)
{
	init_fnd();
	
	while(1)
	{
		fnd_display();
		_delay_ms(1);
		ms_count++;
		if(ms_count >= 1000)
		{
			ms_count = 0;
			sec_count++;
			dot_display = !dot_display;
		}
	}
	
	return 0;
}

void init_fnd(void)
{
	FND_DATA_DDR = 0xff;
	FND_DIGIT_DDR |= 1 << FND_DIGIT_D1 | 1 << FND_DIGIT_D2 | 1 << FND_DIGIT_D3 | 1 << FND_DIGIT_D4;
	
	//FND all off
#ifdef COMMON_ANODE
	FND_DATA_PORT = 0xff;
#else	//COMMON_CATHODE
	FND_DATA_PORT = ~0xff;
#endif

}

void fnd_display(void)
{
	//분 초 시계를 만들어보자아아..!
#ifdef COMMON_ANODE
	uint8_t fnd_font[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xd8,0x80, 0x98, 0x7f};//0 ~ .
#else	
	uint8_t fnd_font[] = {0x3f, 0x06, 0x5B, 0x4F, 0x66, 0x6d, 0x7d, 0x27, 0x7f, 0x67, 0x80}; //0 ~ .
#endif	
	static int digit_select = 0;
	
	switch(digit_select)
	{
		case 0:
#ifdef COMMON_ANODE
		FND_DIGIT_PORT = 0x80;
#else
		FND_DIGIT_PORT = 0x7f; // ~0x80
#endif	
		FND_DATA_PORT = fnd_font[sec_count%10];	
		break;
		
		case 1:
#ifdef COMMON_ANODE
		FND_DIGIT_PORT = 0x40;
#else
		FND_DIGIT_PORT = ~0x40;
#endif
		FND_DATA_PORT = fnd_font[sec_count/10%6];
		break;
		
		case 2:
#ifdef COMMON_ANODE
		FND_DIGIT_PORT = 0x20;
#else
		FND_DIGIT_PORT = ~0x20;
#endif
		FND_DATA_PORT = fnd_font[sec_count/60%10];
		break;
		
		case 3:
#ifdef COMMON_ANODE
		FND_DIGIT_PORT = 0x10;
#else
		FND_DIGIT_PORT = ~0x10;
#endif
		FND_DATA_PORT = fnd_font[sec_count/600%6];
		break;
		
		default:
		break;
	}
	digit_select = (digit_select + 1) % 4;
	
}