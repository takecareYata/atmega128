#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "timer.h"
#include "button.h"
#include "pwm.h"
#include "fnd.h"
#include "wash.h"

volatile unsigned long ms_count = 0;
volatile unsigned long stopwatch_ms_count = 0;
volatile long sec_count = 0;
volatile unsigned char dot_display = 0;
volatile unsigned long stopwatch_sec_count = 0;
volatile unsigned char stopwatch_state = 0;

extern unsigned char fnd_font[];

unsigned char ani1_font[] = {~0x01, ~0x00, ~0x00, ~0x00, ~0x08, ~0x06};
unsigned char ani2_font[] = {~0x00, ~0x01, ~0x30, ~0x08, ~0x00, ~0x00};

void display_system_status(WashingMachine *washer) 
{
	static unsigned char digit_select = 0;
	unsigned int display_value = 0;
	int ani_cnt;
	
	// 시간설정
	switch (washer->currentState) 
	{
		case STATE_WASH:
		case STATE_RINSE:
		case STATE_SPIN:
		display_value = washer->currentRemaining;
		break;
		case STATE_SET_WASH_TIME:
		display_value = washer->washTime;         
		break;
		case STATE_SET_RINSE_TIME:
		display_value = washer->rinseTime;
		break;
		case STATE_SET_SPIN_TIME:
		display_value = washer->spinTime;
		break;
		case STATE_STANDBY:
		default:
		display_value = 0;
		break;
	}

	//역회전 애니메이션 선택
	if ((PORTF & (1 << PF7)) && washer->currentState == STATE_RINSE)
	{
		ani_cnt = 5 - sec_count;
	}
	else
	{
		ani_cnt = sec_count;
	}
	

	//fnd출력
	switch(digit_select) 
	{
		case 0:
		FND_DIGIT_PORT = 0x80;
		FND_DATA_PORT = fnd_font[display_value % 10];
		break;
		case 1:
		FND_DIGIT_PORT = 0x40;
		FND_DATA_PORT = fnd_font[(display_value / 10) % 10];
		break;
		case 2:
		FND_DIGIT_PORT = 0x20;
		if(washer->currentState == STATE_RINSE || washer->currentState == STATE_WASH || washer->currentState == STATE_SPIN)
		{
			FND_DATA_PORT = ani1_font[ani_cnt];
		}
		else
		{
			FND_DATA_PORT = ~0x00;
		}
		break;
		case 3:
		FND_DIGIT_PORT = 0x10;
		if(washer->currentState == STATE_RINSE || washer->currentState == STATE_WASH || washer->currentState == STATE_SPIN)
		{
			FND_DATA_PORT = ani2_font[ani_cnt];
		}
		else
		{
			FND_DATA_PORT = ~0x00;
		}
		break;
	}
	digit_select = (digit_select + 1) % 4;
}

int main(void) 
{
	WashingMachine washer;

	init_timer0();
	init_button();
	init_led();
	init_timer3_pwm();
	init_motor_driver();
	init_fnd();

	init_wash(&washer);

	sei();

	unsigned long last_fnd_tick = 0;
	unsigned long last_ani_tick = 0;
	

	while (1) 
	{
		SystemEvent event = EVENT_NONE;
		
		if (get_button(BUTTON0, BUTTON0PIN))
		{
			event = EVENT_BTN0;
		}
		else if (get_button(BUTTON1, BUTTON1PIN))
		{
			event = EVENT_BTN1;
		}
		else if (get_button(BUTTON2, BUTTON2PIN)) 
		{
			event = EVENT_BTN2;
		}
		else if (get_button(BUTTON3, BUTTON3PIN))
		{
			event = EVENT_BTN3;
		}
		
		if (event != EVENT_NONE) 
		{
			wash_handle_event(&washer, event);
		}

		wash_update_timer(&washer);

		if (msec_count - last_ani_tick >= 100)
		{
			last_ani_tick = msec_count;
			sec_count++; 
			if (sec_count >= 6)
			{
				sec_count = 0;
			}
		}
		
		if (msec_count - last_fnd_tick >= 2) 
		{
			last_fnd_tick = msec_count;
			display_system_status(&washer);
		}
	}
	
	return 0;
}