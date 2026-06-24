/*
 * 05. ULTRASONIC
 *
 * Created: 2026-06-15 오후 2:40:23
 * Author : yata
 */ 

#define F_CPU	16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "timer.h"
#include "uart.h"
#include "led.h"
#include "ultrasonic.h"

FILE OUTPUT = FDEV_SETUP_STREAM( uart0_transmit,NULL,_FDEV_SETUP_WRITE);// stdout으로 콘솔이 설정된다.

int main(void)
{
	init_timer0();
	init_uart0();
	init_led();
	init_ultrasonic();
	
	stdout = &OUTPUT; //printf가 동작할 수 있도록 stdout을 설정
	
	sei(); // 전역 interrupt 허용
	
	make_trigger();
	
	while (1) 
    {
		//pc_command_process();
		ultrasonic_processsing();
		ultrasonic_show_led();
    }
}
