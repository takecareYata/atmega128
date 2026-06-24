/*
 * pwm.c
 *
 * Created: 2026-06-18 오전 11:40:12
 *  Author: kccistc
 */ 
#include "pwm.h"

//16bit 3번 timer/counter 사용
/*
PWM 출력 신호
PE3 : OC3A
PE4 : OC3B : INT4 사용 (초음파 센서)
PE5 : OC3C : DC motor (PWM출력)
BTN1: speed up
BTN2: speed down
*/
void init_timer3_pwm(void)
{
	DDRE |= 1 << 3 | 1 << 5;
	
	TCCR3A |= 1 << WGM30; // 8bit 고속pwm
	TCCR3B |= 1 << WGM32;
	// 비반전 모드 top : 0x00ff 비교일치 값 지정
	TCCR3A |= 1 << COM3C1;
	
	//분주비 설정
	// 분주비 64
	// 16000000Hz /64 == 250kHz
	// 1/250kHz == 4us
	// 250000hz에서 256개 펄스를 count 하면 소요시간 1.02ms
	// 250000hz에서 127개 펄스를 count 하면 소요시간 0.5ms
	// TCNT3 : 0~255(0xff) 까지 count한 후 0으로 다시 돌아간다.
	
	TCCR3B |= 1 << CS31 | 1<< CS30; // 분주비 64
	OCR3C = 0; // Output Compare Regitster : PWM값
	// OCR3C : 50 인 경우 Duty가 이렇게된다.
	// Duty_cycle = (OCR3C / Top) * 100 = 50 / 255 * 100 = 19.61(%)
}

/*
PE5 : PWM control
PF6 : IN1 방향설정
PF7 : IN2 방향설정
*/
void init_motor_driver(void)
{
	DDRF |= 1 << 6 | 1 << 7;
	PORTF &= ~(1 << 6 | 1 << 7);
	PORTF |= 1 << 6; //정회전
}

void dcmotor_pwm_control_main(void)
{
	int start_button = 0;
	int forward = 0;
	
	while(1)
	{
		if (get_button(BUTTON0,BUTTON0PIN))
		{
			start_button = !start_button;
			if(start_button)
			{
				OCR3C = 250;
			}
			else
			{
				OCR3C = 0;
			}
		}
		else if (get_button(BUTTON1,BUTTON1PIN))
		{
			if (OCR3C >= 250)
			{
				OCR3C = 250; 
			}
			else
			{
				OCR3C += 20;
			}
		}
		else if (get_button(BUTTON2,BUTTON2PIN))
		{
			if (OCR3C <= 70)
			{
				OCR3C = 70;
			}
			else
			{
				OCR3C -= 20;
			}
		}
		else if (get_button(BUTTON3,BUTTON3PIN))
		{
			forward = !forward;
			PORTF &= ~(1 << 6 | 1 << 7 );
			if(forward)
			{
				PORTF |= 1 << 6 ; //정회전
			}
			else
			{
				PORTF |= 1 << 7; // 역회전
			}
		}
	}
}

void init_pwm_pb4(void) // 고속 PWM
{
	// 파형 출력 핀인 PB4 핀(OC0 핀)을 출력으로 설정
	DDRB |= (1 << PB4);

	// 타이머/카운터 0번을 고속 PWM 모드로 설정
	TCCR0 |= (1 << WGM01) | (1 << WGM00);
	// 비반전 모드
	TCCR0 |= (1 << COM01);
	// 분주비를 1024로 설정
	TCCR0 |= (1 << CS02) | (1 << CS01) | (1 << CS00);
}

void init_pwm_pb5Npb6(void)//위상교정 PWM
{
	DDRB |= (1 << PB5) | (1 << PB6);

	// 타이머/카운터 1번을 8비트 위상 교정 PWM 모드로 설정
	TCCR1A |= (1 << WGM10);
	
	// 비교일치 A는 비반전 모드, 비교일치 B는 반전 모드로 설정
	TCCR1A |= (1 << COM1A1);
	TCCR1A |= (1 << COM1B1) | (1 << COM1B0);
	
	// 분주비를 256으로 설정
	TCCR1B |= (1 << CS12); // | (1 << CS10);
}