

#define F_CPU 16000000UL // 16MHz 클럭 설정
#include <avr/io.h>
#include <util/delay.h>

// 방향 제어 편의를 위한 매크로 정의 (PORTF 6번, 7번)
#define MOTOR_DIR_PORT   PORTF
#define MOTOR_INA        PORTF6
#define MOTOR_INB        PORTF7

void Motor_Init(void)
{
	// 1. 방향 제어 핀 설정 (PF6, PF7을 출력으로)
	DDRF |= (1 << DDF6) | (1 << DDF7);
	
	// 2. PWM 속도 제어 핀 설정 (PE5 / OC3C 핀을 출력으로)
	DDRE |= (1 << DDE5);

	// 3. 타이머/카운터 3번 설정 (PE5 = OC3C 채널 사용)
	// WGM33:0 = 0101 -> Fast PWM, 8-bit 모드 (TOP: 0x00FF = 255)
	// COM3C1:0 = 10 -> OC3C 핀을 비반전 모드(Non-inverting)로 설정
	TCCR3A |= (1 << COM3C1) | (1 << WGM30);
	
	// CS32:0 = 011 -> 분주비(Prescaler)를 64로 설정 (주파수: 약 976Hz)
	TCCR3B |= (1 << WGM32) | (1 << CS31) | (1 << CS30);

	// 초기 상태: 모터 정지 (속도 0)
	OCR3C = 0;
	MOTOR_DIR_PORT &= ~((1 << MOTOR_INA) | (1 << MOTOR_INB));
}

// 모터 방향 및 속도 제어 함수
// direction: 1(정회전), 2(역회전), 0(정지)
// speed: 0 ~ 255 (PWM Duty Cycle)
void Control_Motor(uint8_t direction, uint8_t speed)
{
	// 속도 설정 (OCR3C 레지스터에 값 입력)
	OCR3C = speed;

	// 방향 설정
	if (direction == 1)      // 정회전 (Forward)
	{
		MOTOR_DIR_PORT |= (1 << MOTOR_INA);
		MOTOR_DIR_PORT &= ~(1 << MOTOR_INB);
	}
	else if (direction == 2) // 역회전 (Backward)
	{
		MOTOR_DIR_PORT &= ~(1 << MOTOR_INA);
		MOTOR_DIR_PORT |= (1 << MOTOR_INB);
	}
	else                     // 정지 (Stop)
	{
		MOTOR_DIR_PORT &= ~(1 << MOTOR_INA);
		MOTOR_DIR_PORT &= ~(1 << MOTOR_INB);
		OCR3C = 0;
	}
}

int main(void)
{
	Motor_Init(); // 모터 및 타이머 초기화

	while (1)
	{
		// 속도 255(최대)로 무조건 정회전 고정 테스트
		Control_Motor(1, 255);
	}
}
