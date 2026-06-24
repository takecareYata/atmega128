/*
 * motor.c
 *
 * Created: 2026-06-19 오후 5:10:05
 *  Author: kccistc
 */ 
#include "motor.h"

void init_motor(void)
{
	DDRF |= (1 << PF0) | (1 << PF1) | (1 << PF2) | (1 << PF3);
	PORTF &= ~((1 << PF0) | (1 << PF1) | (1 << PF2) | (1 << PF3));

	DDRB |= (1 << PB5) | (1 << PB6);

	TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << CS11) | (1 << CS10);

	OCR1A = 0;
	OCR1B = 0;
}

// 1. 전진 함수
void motor_forward(uint8_t speed)
{
	// 왼쪽 전진 (H / L)
	PORTF |= (1 << PF0);
	PORTF &= ~(1 << PF1);

	// 오른쪽 전진 (L / H)
	PORTF &= ~(1 << PF2);
	PORTF |= (1 << PF3);

	OCR1A = speed;
	OCR1B = speed;
}

// 2. 후진 함수 (전진 신호를 정반대로 뒤집음)
void motor_backward(uint8_t speed)
{
	// 왼쪽 후진 (L / H)
	PORTF &= ~(1 << PF0);
	PORTF |= (1 << PF1);

	// 오른쪽 후진 (H / L)
	PORTF |= (1 << PF2);
	PORTF &= ~(1 << PF3);

	OCR1A = speed;
	OCR1B = speed;
}

// 3. 제자리 좌회전 (왼쪽 바퀴는 후진, 오른쪽 바퀴는 전진)
void motor_left(uint8_t speed)
{
	// 왼쪽 후진 (L / H)
	PORTF &= ~(1 << PF0);
	PORTF |= (1 << PF1);

	// 오른쪽 전진 (L / H)
	PORTF &= ~(1 << PF2);
	PORTF |= (1 << PF3);

	OCR1A = speed;
	OCR1B = speed;
}

// 4. 제자리 우회전 (왼쪽 바퀴는 전진, 오른쪽 바퀴는 후진)
void motor_right(uint8_t speed)
{
	// 왼쪽 전진 (H / L)
	PORTF |= (1 << PF0);
	PORTF &= ~(1 << PF1);

	// 오른쪽 후진 (H / L)
	PORTF |= (1 << PF2);
	PORTF &= ~(1 << PF3);

	OCR1A = speed;
	OCR1B = speed;
}

// 5. 급정지 및 브레이크 함수
void motor_stop(void)
{
	// 모든 제어 핀을 Low로 만들어 모터에 공급되는 전류를 차단
	PORTF &= ~((1 << PF0) | (1 << PF1) | (1 << PF2) | (1 << PF3));

	// PWM 출력도 0으로 설정
	OCR1A = 0;
	OCR1B = 0;
}

