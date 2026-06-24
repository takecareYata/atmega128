/*
 * 06.DCMOTOR_PWM_CONTROL.c
 *
 * Created: 2026-06-18 오후 2:04:08
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
#include "button.h"
#include "pwm.h"

FILE OUTPUT = FDEV_SETUP_STREAM( uart0_transmit,NULL,_FDEV_SETUP_WRITE);// stdout으로 콘솔이 설정된다.

int main(void)
{
	init_led();
	init_timer0();
	init_uart0();
	init_button();
	init_timer3_pwm();
	init_motor_driver();
	
    stdout = &OUTPUT; //printf가 동작할 수 있도록 stdout을 설정
    
    sei(); // 전역 interrupt 허용

	dcmotor_pwm_control_main();
	
    while (1) 
    {

    }
}

