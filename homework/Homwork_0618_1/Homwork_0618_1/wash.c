#include "wash.h"
#include "pwm.h"
#include "timer.h"
#include <avr/io.h>

// 1초 카운트다운을 처리하기 위한 이전 틱 저장 변수
static unsigned long last_sec_tick = 0;

void init_wash(WashingMachine *washer) {
	washer->currentState = STATE_STANDBY;
	washer->washTime = 10;   // 기본 세탁 시간 
	washer->rinseTime = 15;  // 기본 헹굼 시간 
	washer->spinTime = 5;    // 기본 탈수 시간
	washer->currentRemaining = 0;

	OCR3C = 0;
}

void wash_handle_event(WashingMachine *washer, SystemEvent event) 
{
	if (event == EVENT_NONE)
	{ 
		return;
	}

	SystemState nextState = washer->currentState;

	switch (washer->currentState)
	{
		case STATE_STANDBY:
		if (event == EVENT_BTN0) 
		{
			nextState = STATE_WASH;                   // 대기 -> 세탁(WASH) 진입
			washer->currentRemaining = washer->washTime;
			
		} 
		else if (event == EVENT_BTN1) 
		{
			nextState = STATE_SET_WASH_TIME;          // 세탁 시간 설정 이동
		}
		break;

		case STATE_WASH:
		if (event == EVENT_BTN0) 
		{
			nextState = STATE_STANDBY;                // 강제 중지 후 대기행
		} 
		else if (event == EVENT_TIMEOUT) 
		{
			nextState = STATE_RINSE;                  // 세탁 완료 -> 자동으로 헹굼 진입
			washer->currentRemaining = washer->rinseTime;
			
		}
		break;

		case STATE_RINSE:
		if (event == EVENT_BTN0) 
		{
			nextState = STATE_STANDBY;
		} 
		else if (event == EVENT_TIMEOUT) 
		{
			nextState = STATE_SPIN;                   // 헹굼 완료 -> 자동으로 탈수 진입
			washer->currentRemaining = washer->spinTime;
			
		}
		break;

		case STATE_SPIN:
		if (event == EVENT_BTN0) 
		{
			nextState = STATE_STANDBY;
		} 
		else if (event == EVENT_TIMEOUT) 
		{
			nextState = STATE_STANDBY;                // 모든 코스 완료 -> 대기모드 복귀
		}
		break;

		case STATE_SET_WASH_TIME:
		if (event == EVENT_BTN1) 
		{
			nextState = STATE_SET_RINSE_TIME;         // BTN1: 다음 설정 단계 이동
		} 
		else if (event == EVENT_BTN2) 
		{
			washer->washTime += 5;                       // BTN2 룰렛: 세탁 시간 5초 증가
			if (washer->washTime > 60) 
			{
				washer->washTime = 5;
			}
		}
		else if (event == EVENT_BTN3)
		{
			washer->washTime -= 5;                       // BTN2 룰렛: 세탁 시간 5초 증가
			if (washer->washTime == 0)
			{
				washer->washTime = 60;
			}
		}
		break;

		case STATE_SET_RINSE_TIME:
		if (event == EVENT_BTN1) 
		{
			nextState = STATE_SET_SPIN_TIME;
		} 
		else if (event == EVENT_BTN2) 
		{
			washer->rinseTime += 5;                     
			if (washer->rinseTime > 60)
			{
				washer->rinseTime = 5;
			}
		}
		else if (event == EVENT_BTN3)
		{
			washer->rinseTime -= 5;                      
			if (washer->rinseTime == 0)
			{
				washer->rinseTime = 60;
			}
		}
		break;

		case STATE_SET_SPIN_TIME:
		if (event == EVENT_BTN1) 
		{
			nextState = STATE_STANDBY;                // 설정 완료 후 대기모드로 복귀
		} 
		else if (event == EVENT_BTN2) 
		{
			washer->spinTime += 5;                      
			if (washer->spinTime > 60)
			{
				washer->spinTime = 5;
			}
			
		}
		else if (event == EVENT_BTN3)
		{
			washer->spinTime -= 5;                       
			if (washer->spinTime == 0)
			{
				washer->spinTime = 60;
			}
		}
		break;
	}

	if (nextState != washer->currentState) 
	{
		washer->currentState = nextState;
		
		switch (washer->currentState) 
		{
			case STATE_STANDBY:
			PORTA = 0x0;
			OCR3C = 0;          // 모터 정지
			break;
			case STATE_WASH:
			PORTA = 0x1;
			PORTF = (1 << PF6); // 정회전 세탁 방향
			OCR3C = 160;        // 세탁 속도 (중속)
			break;
			case STATE_RINSE:
			PORTA = 0x2;
			PORTF = (1 << PF7); // 역회전 헹굼 방향
			OCR3C = 160;        // 헹굼 속도 (중속)
			break;
			case STATE_SPIN:
			PORTA = 0x4;
			PORTF = (1 << PF6); // 정회전 탈수 방향
			OCR3C = 250;        // 탈수 속도 (강한 고속 회전)
			break;
			case STATE_SET_WASH_TIME:
			PORTA = 0x1;
			break;
			case STATE_SET_RINSE_TIME:
			PORTA = 0x2;
			break;
			case STATE_SET_SPIN_TIME:
			PORTA = 0x4;
			break;
			default:
			OCR3C = 0;          // 시간 설정 모드에서는 정지
			break;
		}
	}
}

void wash_update_timer(WashingMachine *washer) 
{
	if (washer->currentState == STATE_WASH || washer->currentState == STATE_RINSE || washer->currentState == STATE_SPIN) 
	{
		if (msec_count - last_sec_tick >= 1000) 
		{
			last_sec_tick = msec_count;
			
			if (washer->currentRemaining > 0) 
			{
				washer->currentRemaining--;
			}
			
			if (washer->currentRemaining == 0) 
			{
				wash_handle_event(washer, EVENT_TIMEOUT); // 시간 만료 시 상태 천이 이벤트 발행
			}
		}
	} 
	else 
	{
		last_sec_tick = msec_count;
	}
}