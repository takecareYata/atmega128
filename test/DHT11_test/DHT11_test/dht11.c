/*
 * dht11.c
 *
 * Created: 2026-06-25 오후 4:20:34
 *  Author: kccistc
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "dht11.h"

// DHT11 시작 신호 (MCU -> Sensor)
void dht11_start(void)
{
	DHT11_DDR |= (1 << DHT11_BIT);   // 출력 모드
	DHT11_PORT &= ~(1 << DHT11_BIT);  // Low 출력
	_delay_ms(20);                    // 데이터시트 안정권인 20ms 유지
	
	DHT11_PORT |= (1 << DHT11_BIT);   // High 출력
	_delay_us(30);                    // 30us 대기
}

// DHT11 응답 확인 (Sensor -> MCU)
int dht11_check_response(void)
{
	DHT11_DDR &= ~(1 << DHT11_BIT);  // 입력 모드로 변경
	// ⚠️ 외부 풀업 저항이 약할 경우를 대비해 MCU 내부 풀업을 임시로 켜서 신호선을 끌어올려 줍니다.
	DHT11_PORT |= (1 << DHT11_BIT);
	
	// 센서가 응답 신호로 Low를 내려보낼 때까지 대기 (타임아웃 핸들링 추가)
	uint16_t timeout = 0;
	while(DHT11_PIN & (1 << DHT11_BIT)) 
	{
		if(timeout++ > 200) return 0; // 너무 오래 걸리면 에러 리턴
		_delay_us(1);
	}
	
	// 센서가 Low를 유지하는 구간 (약 80us) 대기
	timeout = 0;
	while(!(DHT11_PIN & (1 << DHT11_BIT))) 
	{
		if(timeout++ > 200) return 0;
		_delay_us(1);
	}
	
	// 센서가 High를 유지하는 구간 (약 80us) 대기
	timeout = 0;
	while(DHT11_PIN & (1 << DHT11_BIT)) 
	{
		if(timeout++ > 200) return 0;
		_delay_us(1);
	}
	
	return 1; // 응답 확인 완료
}

// 1바이트 읽기 함수 (정밀 타이밍 반영)
uint8_t dht11_read_byte(void)
{
	uint8_t data = 0;
	
	for (int i = 0; i < 8; i++)
	{
		// 1. 모든 비트 시작 전 50us Low 구간 대기
		while (!(DHT11_PIN & (1 << DHT11_BIT)));
		
		// 2. High 구간 진입 직후 약 40us 대기하여 비트 판별
		// (0은 26~28us만에 Low로 떨어지고, 1은 70us 동안 유지됨)
		_delay_us(40);
		
		if (DHT11_PIN & (1 << DHT11_BIT))
		{
			data |= (1 << (7 - i)); // 여전히 High면 1 저장
			// 남은 High 구간이 끝날 때까지 무한루프 방지 타임아웃 대기
			uint16_t timeout = 0;
			while (DHT11_PIN & (1 << DHT11_BIT)) {
				if(timeout++ > 100) break;
				_delay_us(1);
			}
		}
		// 이미 Low로 떨어졌다면 0 (데이터 배열에 아무것도 안 더함)
	}
	return data;
}

// 메인 호출용 함수
int read_dht11(uint8_t *humidity, uint8_t *temperature)
{
	uint8_t data[5] = {0, };
	
	dht11_start();
	
	if (dht11_check_response())
	{
		data[0] = dht11_read_byte(); // 습도 정수
		data[1] = dht11_read_byte(); // 습도 소수
		data[2] = dht11_read_byte(); // 온도 정수
		data[3] = dht11_read_byte(); // 온도 소수
		data[4] = dht11_read_byte(); // 체크섬
		
		// 체크섬 검증
		if (data[0] + data[1] + data[2] + data[3] == data[4])
		{
			if(data[0] == 0 && data[2] == 0) return 0; // 유효하지 않은 0 데이터 필터링
			
			*humidity = data[0];
			*temperature = data[2];
			return 1; // 읽기 성공
		}
	}
	return 0; // 읽기 실패
}