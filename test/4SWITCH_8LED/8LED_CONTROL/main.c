#define F_CPU 16000000UL // 16MHz 크리스탈 사용 시 (자신의 환경에 맞게 수정)
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	//입출력 방향 설정
	DDRA = 0xFF;  // PORTA의 모든 핀을 출력(Output)으로 설정 (LED 연결)
	DDRD &= ~((1 << PD3) | (1 << PD4) | (1 << PD5) | (1 << PD6)); // PD3~PD6을 입력(Input)으로 설정

	//초기 출력값 설정
	uint8_t led_pattern = 0x01; // 처음에는 0번 LED만 켜짐 (0000 0001)
	PORTA = led_pattern;

	while (1)
	{
		if (PIND & (1 << PD3))
		{
			if (led_pattern == 0x01) led_pattern = 0x80;
			else led_pattern >>= 1;
			
			PORTA = led_pattern;
			_delay_ms(200); // 디바운싱(스위치 떨림 방지) 및 연속 입력 제한
			while(PIND & (1 << PD3)); // 스위치에서 손을 뗄 때까지 대기
		}

		if (PIND & (1 << PD4))
		{
			// 가장 오른쪽(0x01)에 도달하면 다시 0x80으로 순환 (선택 사항)
			if (led_pattern == 0x80) led_pattern = 0x01;
			else led_pattern <<= 1;
			
			PORTA = led_pattern;
			_delay_ms(200);
			while(PIND & (1 << PD4));
		}


		if (PIND & (1 << PD5))
		{
			led_pattern = 0x01;
			PORTA = led_pattern;
			_delay_ms(200);
			while(PIND & (1 << PD5));
		}

		if (PIND & (1 << PD6))
		{
			led_pattern = 0x80;
			PORTA = led_pattern;
			_delay_ms(200);
			while(PIND & (1 << PD6));
		}
	}

	return 0;
}

