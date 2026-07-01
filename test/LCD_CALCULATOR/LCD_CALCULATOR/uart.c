/*
 * uart.c
 *
 * Created: 2026-06-16 오전 9:57:38
 *  Author: yata
 */
#include "uart.h"

volatile int uart0_rear = 0;  // interrupt에서 queue에 data를 저장하는 위치값
volatile int uart0_front = 0;  // pc_command_processing에서 가져가는 위치값
volatile unsigned char rx_buff[QUEUE_SIZE][QUEUE_LENGTH];
/*
1. 전송속도 9600bps
2. start/atop 설정
3. RX : interrupt 로 설정
*/

/*
PC로 부터 1byte가 들어오면 자동으로 이곳으로 진입한다.
*/
ISR(USART0_RX_vect)
{
	volatile uint8_t data;
	volatile static int i = 0;
	data = UDR0;
	
	if(data == '\n' || data == '\r')
	{
		if ((uart0_rear + 1) % QUEUE_SIZE == uart0_front % QUEUE_SIZE)
		{
			return;// queue full 상태
		}
		rx_buff[uart0_rear][i] = '\0';
		i = 0; // 다음 문자열을 저장하기위해서 초기화
		uart0_rear = (uart0_rear + 1) % QUEUE_SIZE; // 0~9 반복
	}
	else
	{
		if ((uart0_rear + 1) % QUEUE_SIZE == uart0_front % QUEUE_SIZE)
		{
			return;// queue full 상태
		}
		rx_buff[uart0_rear][i++] = data;
	}
}


void init_uart0(void)
{
	UBRR0H = 0x0;
	UBRR0L = 207; //9600bps
	UCSR0A |= 1 << U2X0; //2배속 설정
	//uart0 를 송신, 수신이 다 가능하고 RX INT가 가능하도록 설정한다.
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	
}

/*
uart0 로 1byte전송하는 함수
*/
void uart0_transmit(uint8_t data)
{
	while(!(UCSR0A & (1 << UDRE0))); // data가 송신중이면 송신이 끝날 때 까지 기다림
	UDR0 = data; // 데이터 전송
}

int uart0_putchar(char c, FILE *stream)
{
	if (c == '\n') { // 개행 문자(\n) 처리 시 캐리지 리턴(\r) 자동 추가
		uart0_transmit('\r');
	}
	uart0_transmit(c); // 실제 uart.c의 함수 호출
	return 0;
}

