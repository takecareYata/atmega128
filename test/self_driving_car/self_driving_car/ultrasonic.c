/*
 * ultrasonic.c
 *
 * Created: 2026-06-19 오후 5:11:21
 *  Author: kccistc
 */ 
#include "ultrasonic.h"
#include <avr/interrupt.h>

// 좌, 중, 우 각각의 실시간 거리를 저장하는 배열 (cm 단위)
volatile int target_distance[3] = {0, };

// [왼쪽 센서] INT4 (PE4)
ISR(INT4_vect)
{
	if (ECHO_PORT & (1 << ECHO_PIN_L))
	{
		TCNT3 = 0; // 타이머3 초기화
	}
	else
	{
		target_distance[US_LEFT] = (TCNT3 * 64) / 58;
	}
}

// [중앙 센서] INT5 (PE5)
ISR(INT5_vect)
{
	if (ECHO_PORT & (1 << ECHO_PIN_C))
	{
		TCNT3 = 0; // 타이머3 초기화
	}
	else
	{
		target_distance[US_CENTER] = (TCNT3 * 64) / 58;
	}
}

// [오른쪽 센서] INT6 (PE6)
ISR(INT6_vect)
{
	if (ECHO_PORT & (1 << ECHO_PIN_R))
	{
		TCNT3 = 0; // 타이머3 초기화
	}
	else
	{
		target_distance[US_RIGHT] = (TCNT3 * 64) / 58;
	}
}

void init_ultrasonic(void)
{
	// TRIG 핀들 출력 설정
	TRIG_DDR |= (1 << TRIG_PIN_L) | (1 << TRIG_PIN_C) | (1 << TRIG_PIN_R);
	
	// ECHO 핀들 입력 설정
	ECHO_DDR &= ~((1 << ECHO_PIN_L) | (1 << ECHO_PIN_C) | (1 << ECHO_PIN_R));

	// 외부 인터럽트 설정 (INT4, 5, 6 양방향 에지 트리거)
	EICRB |= (1 << ISC40) | (1 << ISC50) | (1 << ISC60);
	EICRB &= ~((1 << ISC41) | (1 << ISC51) | (1 << ISC61));

	// 타이머 3번 제어 레지스터 설정 (일반 모드, 1024 분주비)
	TCCR3A = 0x00;
	TCCR3B |= (1 << CS32) | (1 << CS30);

	// 외부 인터럽트 마스크 허용
	EIMSK |= (1 << INT4) | (1 << INT5) | (1 << INT6);
}

void make_trigger(uint8_t sensor_idx)
{
	uint8_t pin = 0;
	
	if (sensor_idx == US_LEFT)        
	{
		pin = TRIG_PIN_L;
	}
	else if (sensor_idx == US_CENTER)
	{ 
		pin = TRIG_PIN_C;
	}
	else if (sensor_idx == US_RIGHT)
	{
		pin = TRIG_PIN_R;
	}

	TRIG_PORT &= ~(1 << pin);
	_delay_us(2);
	
	TRIG_PORT |= (1 << pin);
	_delay_us(10);
	
	TRIG_PORT &= ~(1 << pin);
}

int get_distance(uint8_t sensor_idx)
{
	if (sensor_idx > US_RIGHT)
	{
		return 0;
	}
	return target_distance[sensor_idx];
}
