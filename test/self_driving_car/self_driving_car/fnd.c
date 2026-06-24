#include "fnd.h"

// Common Anode 0~9 원본 패턴 (순수 숫자 출력용)
static const uint8_t fnd_pattern[] = {
	0xC0, // 0
	0xF9, // 1
	0xA4, // 2
	0xB0, // 3
	0x99, // 4
	0x92, // 5
	0x82, // 6
	0xF8, // 7
	0x80, // 8
	0x90  // 9
};

// ⭐ 메인 함수와 인터럽트 함수가 공유할 단 하나의 일관된 출력 버퍼
static uint8_t fnd_display_buffer[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// [Common Anode 기준 완벽한 확장 폰트 배열]
static const uint8_t fnd_font[] = {
	0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, // 0~9 숫자 패턴
	0xFF, // 10: 공백 (BLANK, 전면 소등)
	0x8E, // 11: 'F' (전진)
	0x92, // 12: 'S' (정지)
	0x83, // 13: 'b' (후진)
	0xC7, // 14: 'l' (좌회전)
	0xAF,  // 15: 'r' (우회전)
	0x87  // 16: 't' (시간 표시)
};

void init_fnd(void)
{
	// 데이터 포트 PORTC 전체 출력 설정
	DDRC = 0xFF;
	PORTC = 0xFF; // Anode 타입이므로 초기화 시 전체 소등(High)

	// 상위 자릿수 선택 핀 (PA4~PA7) 출력 설정
	DDRA |= (1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7);
	PORTA &= ~((1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7));

	// 하위 자릿수 선택 핀 (PB0~PB3) 출력 설정
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
	PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3));
}

void display_1to8(void)
{
	int i;
	for (i = 0; i < 8; i++)
	{
		// 디버깅 모드일 때 1~8 패턴을 Anode 비트 마스크로 직접 맵핑하여 대입
		fnd_display_buffer[i] = fnd_pattern[i + 1];
	}
}

void fnd_multiplex_isr(void)
{
	static uint8_t digit_index = 0;

	// 1. [잔상 제거] 고스트 현상 방지를 위해 이전 자릿수 핀 전면 차단
	PORTA &= ~((1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7));
	PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3));

	// 2. 버퍼에 이미 완벽한 Anode 비트 패턴 코드가 들어있으므로 다이렉트로 출력
	PORTC = fnd_display_buffer[digit_index];

	// 3. 1ms 주기에 맞춰 해당 자릿수 선택 핀만 활성화
	if (digit_index == 0)       PORTA |= (1 << PA4);
	else if (digit_index == 1)  PORTA |= (1 << PA5);
	else if (digit_index == 2)  PORTA |= (1 << PA6);
	else if (digit_index == 3)  PORTA |= (1 << PA7);
	else if (digit_index == 4)  PORTB |= (1 << PB0);
	else if (digit_index == 5)  PORTB |= (1 << PB1);
	else if (digit_index == 6)  PORTB |= (1 << PB2);
	else if (digit_index == 7)  PORTB |= (1 << PB3);

	// 4. 다음 자릿수로 인덱스 증가 및 순환
	digit_index++;
	if (digit_index >= 8)
	{
		digit_index = 0;
	}
}

void update_fnd_autonomous(uint8_t state, uint32_t clock_sec)
{
	// 1. 상위 FND (0~3번 칸): 분/초 계산 후 비트 패턴 버퍼에 다이렉트 등록
	uint8_t min = (clock_sec / 60) % 100;
	uint8_t sec = clock_sec % 60;

	fnd_display_buffer[4] = fnd_font[min / 10]; // 분 10의 자리 패턴
	fnd_display_buffer[5] = fnd_font[min % 10]; // 분 1의 자리 패턴
	fnd_display_buffer[6] = fnd_font[sec / 10]; // 초 10의 자리 패턴
	fnd_display_buffer[7] = fnd_font[sec % 10]; // 초 1의 자리 패턴

	// 2. 하위 FND (4~7번 칸): 일단 모두 공백 패턴(0xFF)으로 초기화
	fnd_display_buffer[0] = fnd_font[10];
	fnd_display_buffer[1] = fnd_font[10];
	fnd_display_buffer[2] = fnd_font[10];
	fnd_display_buffer[3] = fnd_font[10];

	// 3. 차량 상태에 따른 특정 칸 알파벳 매핑 (정확히 단 하나의 출력 버퍼에 타겟팅)
	switch (state)
	{
		case 1: // STATE_FORWARD (첫 번째 칸 F)
		fnd_display_buffer[0] = fnd_font[11];
		break;
		case 0: // STATE_STOP (첫 번째 칸 S)
		fnd_display_buffer[0] = fnd_font[12];
		break;
		case 2: // STATE_BACKWARD (두 번째 칸 b)
		fnd_display_buffer[1] = fnd_font[13];
		break;
		case 3: // STATE_LEFT (세 번째 칸 l)
		fnd_display_buffer[2] = fnd_font[14];
		break;
		case 4: // STATE_RIGHT (네 번째 칸 r)
		fnd_display_buffer[3] = fnd_font[15];
		break;
		case 5: // MODE_SHOW_TIME (네 번째 칸 t)
		fnd_display_buffer[0] = fnd_font[16];
		break;
	}
}
