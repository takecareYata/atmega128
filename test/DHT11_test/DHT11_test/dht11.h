/*
 * dht11.h
 *
 * Created: 2026-06-25 오후 4:20:48
 *  Author: kccistc
 */ 
#ifndef DHT11_H_
#define DHT11_H_

#include <avr/io.h>
#include <util/delay.h>

#define DHT11_DDR   DDRG
#define DHT11_PIN   PING
#define DHT11_PORT  PORTG
#define DHT11_BIT   PG0

void dht11_start(void);
int dht11_check_response(void);
uint8_t dht11_read_bit(void);
uint8_t dht11_read_byte(void);
int read_dht11(uint8_t *humidity, uint8_t *temperature);


#endif /* DHT11_H_ */