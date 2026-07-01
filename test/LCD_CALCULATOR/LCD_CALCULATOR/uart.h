/*
 * uart.h
 *
 * Created: 2026-06-16 오전 9:57:53
 *  Author: yata
 */ 
#ifndef UART_H_
#define UART_H_

#define F_CPU	16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

// circular queue
#define QUEUE_SIZE 10
#define QUEUE_LENGTH 80
extern volatile int rear;  // interrupt에서 queue에 data를 저장하는 위치값
extern volatile int front;  // pc_command_processing에서 가져가는 위치값
extern volatile unsigned char rx_buff[QUEUE_SIZE][QUEUE_LENGTH];

extern void init_uart0(void);
extern void uart0_transmit(uint8_t data);
int uart0_putchar(char c, FILE *stream);

#endif /* UART_H_ */