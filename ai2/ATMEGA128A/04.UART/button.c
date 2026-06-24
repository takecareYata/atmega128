/*
 * button.c
 *
 * Created: 2026-06-10 오후 1:21:26
 *  Author: yata
 */ 
#include "button.h"

void init_button(void)
{
	BUTTON_DDR &= ~((1 << BUTTON0PIN) | (1 << BUTTON1PIN) | (1 << BUTTON2PIN) | (1 << BUTTON3PIN));
}

int get_button(int butoon_num, int button_pin)
{
	// 버튼을 눌렀다 때면 1을 리턴
	// 버튼 동작 X, 0을 리턴
	
	static unsigned char button_status[BUTTON_NUMBER] = {BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE};
	// static 지역변수를 전역변수화
	int current_state;
	
	current_state = BUTTON_PIN & (1 << button_pin);
	
	if(current_state && button_status[butoon_num] == BUTTON_RELEASE) // 버튼이 처음눌러진 상태
	{
		if (button_ms_count >= 15)
		{	
			button_ms_count = 0;
			button_status[butoon_num] = BUTTON_PRESS;
			return 0;
		}
	}
	else if(current_state == BUTTON_RELEASE && button_status[butoon_num] == BUTTON_PRESS)
	{
		if (button_ms_count >= 15)
		{
			button_ms_count = 0;
			button_status[butoon_num] = BUTTON_RELEASE;
			return 1;
		}
		
	}
	
	return 0;
}
