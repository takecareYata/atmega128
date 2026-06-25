/*
 * DHT11_test.c
 *
 * Created: 2026-06-25 오후 4:20:18
 * Author : kccistc
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include "dht11.h"
#include "uart.h"

int main(void)
{
	uint8_t humidity = 0;
	uint8_t temperature = 0;
	
	init_uart0(); 
	
	printf("--- ATmega128 system start ---\r\n");
	
	while(1)
	{
		if (read_dht11(&humidity, &temperature))
		{
			printf(" temperature: %d C, humidity: %d %%\r\n", temperature, humidity);
		}
		else
		{
			printf("error\r\n");
		}
		
		_delay_ms(1500); // DHT11 안정화를 위해 다음 측정까지 1.5초간 대기 (필수)
	}
	return 0;
}

