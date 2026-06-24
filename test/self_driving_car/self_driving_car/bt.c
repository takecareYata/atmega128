/*
 * bt.c
 *
 * Created: 2026-06-19 오후 5:10:52
 * Author: kccistc
 */ 
#include "bt.h"
#include <avr/interrupt.h>

volatile char rx_data = 0;
volatile uint8_t rx_ready_flag = 0;

// 1. printf 출력 스트림을 위한 파일 포인터 구조체 정의 및 맵핑
// (출력 함수, 입력 함수, 플래그 순서)
FILE uart0_stdout = FDEV_SETUP_STREAM(UART0_print_char, NULL, _FDEV_SETUP_WRITE);

void init_bt(void) 
{
    // UART0 보레이트 설정
    UBRR0H = (unsigned char)(MYUBRR >> 8);
    UBRR0L = (unsigned char)MYUBRR;

    // 송수신 가능 설정 (RX Complete 인터럽트 허용 포함)
    UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);

    // 데이터 비트 수 설정: 8비트 데이터, 1비트 정지 비트 (8N1)
    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

    // 2. 표준 출력(stdout) 대상을 방금 만든 uart0_stdout 스트림으로 교체
    stdout = &uart0_stdout;
}

// 원본 1바이트 전송 함수
void UART0_transmit(char data)
{
    while (!(UCSR0A & (1 << UDRE0)))
    {
        // 대기
    }
    UDR0 = data;
}

int UART0_print_char(char c, FILE *stream)
{
	if (c == '\n')
	{
		UART0_transmit('\r');
	}
	
	UART0_transmit(c);
	return 0;
}

uint8_t BT_receive(void)
{
	if (UCSR0A & (1 << RXC0)) return UDR0;
	return 0;
}

ISR(USART0_RX_vect)
{
    rx_data = UDR0;
    
    // 수신 확인용 에코백
    UART0_transmit(rx_data);
    
    rx_ready_flag = 1;
}