/*
 * uart.c
 *
 * Created: 2026-06-25 오후 4:24:39
 *  Author: kccistc
 */ 
#include "uart.h"

// 1. UART0 초기화 함수
void init_uart0(void)
{
	// 보레이트 설정 (UBRR0H, UBRR0L)
	UBRR0H = (unsigned char)(MYUBRR >> 8);
	UBRR0L = (unsigned char)(MYUBRR);
	
	// 송신(TX) 및 수신(RX) 활성화
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	
	// 데이터 비트 수 설정: 8비트, 정지 비트: 1비트, 패리티: 없음 (비동기 모드)
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	// ✨ 핵심: printf 기능을 UART0 송신 스트림과 연동합니다.
	// fdevopen(송신함수포인터, 수신함수포인터)
	fdevopen(uart0_transmit_char, NULL);
}

// 2. printf 스트림 연동용 1문자 송신 함수
// avr-libc 규격상 매개변수에 FILE *stream 형식이 반드시 포함되어야 합니다.
int uart0_transmit_char(char ch, FILE *stream)
{
	// Carriage Return ('\n')이 들어오면 줄바꿈과 커서 맨 앞으로 가기('\r')를 함께 처리
	if (ch == '\n')
	{
		while (!(UCSR0A & (1 << UDRE0))); // 송신 버퍼가 비어있을 때까지 대기
		UDR0 = '\r';
	}
	
	while (!(UCSR0A & (1 << UDRE0)));     // 송신 버퍼가 비어있을 때까지 대기
	UDR0 = ch;                            // 데이터 레지스터에 문자 저장하여 전송
	
	return 0;
}