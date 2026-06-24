/*
 * button.c
 *
 * Created: 2026-06-19 오후 5:12:30
 *  Author: kccistc
 */ 
#include "button.h"
#include "led.h"

void init_button(void) {
	// PC7 입력 설정
	DDRC &= ~(1 << PC7);
	// 내부 풀업 레지스터 비활성화 (하드웨어 풀다운 적용됨)
	PORTC &= ~(1 << PC7);
}

// 스위치 상태를 읽어 LED를 제어하는 함수
void check_button_and_control_led(void)
{
	// 풀다운이므로 누르면 High(1)가 됨
	if ((PINC & (1 << PC7)) != 0)
	{
		LED_ON();
	}
	else
	{
		LED_OFF();
	}
}

int get_button(int butoon_num, int button_pin)
{
	// 버튼을 눌렀다 때면 1을 리턴
	// 버튼 동작 X, 0을 리턴
	
	static unsigned char button_status[BUTTON_NUMBER] = {BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE};
	
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