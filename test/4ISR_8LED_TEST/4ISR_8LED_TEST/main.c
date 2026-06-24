#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> // 인터럽트 헤더 파일 필수 포함

// 여러 루틴에서 동시에 접근하는 전역 변수는 volatile 키워드를 붙여줍니다.
volatile uint8_t led_pattern = 0x01; // 초기 위치: 0번 LED (0000 0001)

// [INT3] PD3 스위치: 왼쪽으로 쉬프트
ISR(INT3_vect)
{
	if (led_pattern == 0x80) led_pattern = 0x01; // 끝에 도달하면 처음으로
	else led_pattern <<= 1;
	_delay_ms(50); // 간단한 디바운싱 (스위치 채터링 방지)
}

// [INT4] PD4 스위치: 오른쪽으로 쉬프트
ISR(INT4_vect)
{
	if (led_pattern == 0x01) led_pattern = 0x80; // 끝에 도달하면 마지막으로
	else led_pattern >>= 1;
	_delay_ms(50);
}

// [INT5] PD5 스위치: 가장 왼쪽 끝으로 이동
ISR(INT5_vect)
{
	led_pattern = 0x80;
	_delay_ms(50);
}

// [INT6] PD6 스위치: 가장 오른쪽 끝으로 이동
ISR(INT6_vect)
{
	led_pattern = 0x01;
	_delay_ms(50);
}

void interrupt_init(void)
{
	// 1. PD3 ~ PD6 핀을 입력으로 설정
	DDRD &= ~((1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6));

	// 2. 외부 인터럽트 트리거 조건 설정 (Rising Edge일 때 인터럽트 발생)
	// 풀다운 스위치이므로 누르는 순간 Low -> High로 변하는 Rising Edge를 잡아야 합니다.
	EICRA |= (1 << ISC31) | (1 << ISC30); // INT3: Rising Edge
	EICRB |= (1 << ISC41) | (1 << ISC40)  // INT4: Rising Edge
	| (1 << ISC51) | (1 << ISC50)  // INT5: Rising Edge
	| (1 << ISC61) | (1 << ISC60); // INT6: Rising Edge

	// 3. 외부 인터럽트 개별 활성화 (INT3, INT4, INT5, INT6)
	EIMSK |= (1 << INT3) | (1 << INT4) | (1 << INT5) | (1 << INT6);

	// 4. 글로벌 인터럽트 전체 활성화
	sei();
}

int main(void)
{
	DDRA = 0xFF; // PORTA의 모든 핀을 출력(LED)으로 설정
	
	interrupt_init(); // 인터럽트 설정 초기화 호출

	while (1)
	{
		// 버튼을 누르기 전에는 "현재 저장된 위치의 등"이 포함된 전체 상태가 깜빡입니다.
		PORTA = led_pattern; // 지정된 위치의 LED 켜기
		_delay_ms(500);
		
		PORTA = 0x00;        // 전체 끄기
		_delay_ms(500);
	}

	return 0;
}
