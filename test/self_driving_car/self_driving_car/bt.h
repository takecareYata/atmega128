#ifndef BT_H_
#define BT_H_

#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)

#include <avr/io.h>
#include <stdio.h> // printf 스트림 설정을 위해 필수 포함

void init_bt(void);
void UART0_transmit(char data);
uint8_t BT_receive(void);
int UART0_print_char(char c, FILE *stream);

#endif /* BT_H_ */
