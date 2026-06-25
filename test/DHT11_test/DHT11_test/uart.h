/*
 * uart.h
 *
 * Created: 2026-06-25 오후 4:24:53
 *  Author: kccistc
 */ 
#ifndef UART_H_
#define UART_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>

// 보레이트 설정 (일반적으로 가장 많이 쓰는 9600 기준)
#define BAUDRATE 9600
#define MYUBRR   ((F_CPU / (16UL * BAUDRATE)) - 1)

// 함수 원형 선언
void init_uart0(void);
int uart0_transmit_char(char ch, FILE *stream);


#endif /* UART_H_ */