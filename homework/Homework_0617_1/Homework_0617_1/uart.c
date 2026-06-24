/*
 * uart.c
 *
 * Created: 2026-06-16 오전 9:57:38
 *  Author: yata
 */
#include "uart.h"

extern void (* led_fp[])();
extern int func_state;

volatile int rear = 0;  // interrupt에서 queue에 data를 저장하는 위치값
volatile int front = 0;  // pc_command_processing에서 가져가는 위치값
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
		if ((rear + 1) % QUEUE_SIZE == front % QUEUE_SIZE)
		{
			return;// queue full 상태
		}
		rx_buff[rear][i] = '\0';
		i = 0; // 다음 문자열을 저장하기위해서 초기화
		rear = (rear + 1) % QUEUE_SIZE; // 0~9 반복
	}
	else
	{
		if ((rear + 1) % QUEUE_SIZE == front % QUEUE_SIZE)
		{
			return;// queue full 상태
		}
		rx_buff[rear][i++] = data;
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

void pc_command_process(void) // led 제어
{
	if (front != rear) //data가 rx_buff에 존재하는지 check
	{
		printf("%s",rx_buff[front]);
		if(strncmp((char *)rx_buff[front],"led_shift_left_on", strlen("led_shift_left_on")) == 0)
		{
			func_state = 0;
		}
		else if(strncmp((char *)rx_buff[front],"led_shift_right_on", strlen("led_shift_left_on")) == 0)
		{
			func_state = 1;
		}
		else if(strncmp((char *)rx_buff[front],"led_shift_left_keep_on", strlen("led_shift_left_keep_on")) == 0)
		{
			func_state = 2;
		}
		else if(strncmp((char *)rx_buff[front],"led_shift_right_keep_on", strlen("led_shift_right_keep_on")) == 0)
		{
			func_state = 3;
		}
		else if(strncmp((char *)rx_buff[front],"led_flower_on", strlen("led_flower_on")) == 0)
		{
			func_state = 4;
		}
		else if(strncmp((char *)rx_buff[front],"led_flower_off", strlen("led_flower_off")) == 0)
		{
			func_state = 5;
		}
		front = (front + 1 ) % QUEUE_SIZE;
	}
	
	led_fp[func_state]();	
}