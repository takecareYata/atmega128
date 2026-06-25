/*
 * pwm.c
 *
 * Created: 2026-06-18 오후 2:23:02
 *  Author: user
 */ 

#include "pwm.h"
#include "button.h"
#include <stdint.h>

#define MOTOR_PWM_DDR  DDRB
#define MOTOR_LEFT_PORT_DDR  5    // OC1A
#define MOTOR_RIGHT_PORT_DDR  6   // OC1B

#define MOTOR_DIRECTION_PORT  PORTF
#define MOTOR_DIRECTION_PORT_DDR  DDRF

void init_timer1_pwm(void);

#if 0
void init_timer1_pwm(void)
{
	// 모드 14 :  고속 PWM 모드 사용 하겠다. timer1 (P327 표14-5)
	TCCR1A |= 1 << WGM11;     // TOP --> ICR1으로 설정
	TCCR1B |= 1 << WGM13 | 1 << WGM12;
	
	// 비반전 모드 top : ICR1 비교일치 값 (PWM) 지정 OCR1A P350 표15-7
	// 비교 일치 발생시 OCR1, 출력핀은 LOW로 바뀌고 BOTTOM에서 HIGH로 바뀐다.
	TCCR1A |= 1 << COM1A1;
	TCCR1A |= 1 << COM1B1;
	
	//---- 분주비 설정 ----------
	// 분주비 8
	// 16000000Hz / 8 --> 2MHz
	// T=1/f  1/2MHz ---> 0.5us
	// 16bit 최대 65535 count
	// 65535 x 0.5us ---> 0.0327675s --> 32.7675ms
	// 20ms 길이(Duty)를 가지는것을 만든다면,
	// 0.5us x 40000 --> 20ms --> 50Hz
	TCCR1B |= ~(1 << CS12 | 1 << CS11 | 1 << CS10);
	TCCR1B |= 1 << CS11 ;   // 분주비 8
	
	ICR1 = 40000 - 1;  // 20ms TOP 값
}

int servo_motor_main(void)
{
	// servo motor가 연결된 PB5를 출력으로 설정
	DDRB |= 1 << 5;
	
	init_timer1_pwm();
	
	while(1)
	{
		// 0도 1ms 90도 1.5ms 180도 2ms
		
		// 1ms : 2000개 count
		// 40000(20ms) / 20 --> 1ms(2000)
		OCR1A=1000;
		_delay_ms(1000);
		
	
		// 1.5ms : 3000개 count
		OCR1A=3000;
		_delay_ms(1000);
		
		OCR1A=4900;
		_delay_ms(1000);
		
	}
	
	return 0;
}
#else
void init_timer1_pwm(void)
{
	// 1. 모드 14 (Fast PWM, TOP = ICR1) 설정
	// TCCR1A(0x4F)의 WGM11 비트를 1로 설정
	*(volatile uint8_t *)0x4F |= (1 << WGM11);
	
	// TCCR1B(0x4E)의 WGM13, WGM12 비트를 1로 설정
	*(volatile uint8_t *)0x4E |= (1 << WGM13) | (1 << WGM12);
	
	// 2. 비반전 모드(Non-Inverting Mode) 설정
	// PB5 핀이 OC1B이므로, COM1B1 비트를 활성화해야 합니다. (COM1A1도 유지해도 무방)
	*(volatile uint8_t *)0x4F |= (1 << COM1A1); // OC1A (PD5) 비반전
	*(volatile uint8_t *)0x4F |= (1 << COM1B1); // OC1B (PB5) 비반전
	
	// 3. 분주비(Prescaler) 설정 : 8분주
	// 안전하게 기존 분주비 비트(CS12, CS11, CS10)를 클리어한 후 8분주를 켭니다.
	*(volatile uint8_t *)0x4E &= ~((1 << CS12) | (1 << CS11) | (1 << CS10));
	*(volatile uint8_t *)0x4E |= (1 << CS11);   // CS11만 1로 세팅 (8분주)
	
	// 4. PWM 주기 설정 (20ms)
	// ICR1(0x46)은 16비트 레지스터이므로 uint16_t 포인터로 접근합니다.
	*(volatile uint16_t *)0x46 = 40000 - 1;  // 20ms TOP 값 (39999)
}

int servo_motor_main(void)
{
	// 1. DDRB (메모리 주소 0x37)를 출력으로 설정
	// PB5 포트를 출력으로 설정 (DDRB |= 1 << 5;)
	*(volatile uint8_t *)0x37 |= (1 << 5);
	
	// 타이머1 PWM 초기화 함수 호출 (이 내부도 주소값으로 처리되어 있어야 합니다)
	init_timer1_pwm();
	
	while(1)
	{
		// 2. OCR1B (메모리 주소 0x4A) 레지스터 제어
		// PB5 핀을 제어하므로 OCR1A(0x4C)가 아닌 OCR1B(0x4A)를 사용해야 합니다.
		
		// 0도 부근 (작성하신 카운트 기준 1000 복사)
		*(volatile uint16_t *)0x4A = 1000;
		_delay_ms(1000);
		
		// 90도 부근 (작성하신 카운트 기준 3000 복사)
		*(volatile uint16_t *)0x4A = 3000;
		_delay_ms(1000);
		
		// 180도 부근 (작성하신 카운트 기준 4900 복사)
		*(volatile uint16_t *)0x4A = 4900;
		_delay_ms(1000);
	}
	
	return 0;
}
#endif