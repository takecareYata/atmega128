/*
 * rtc.h
 * Created: 2026-07-01
 * Author: yata
 */ 
#ifndef RTC_H_
#define RTC_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DS1307_ADDR 0xD0 // DS1307 I2C 7비트 주소(0x68) + Write비트(0) = 0xD0

// 시간을 담을 구조체 정의
typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
} RTC_Time;

void TWI_init(void);
void RTC_read_time(RTC_Time *time);
void RTC_write_time(RTC_Time *time);
void TWI_start(void);
void TWI_stop(void);
void TWI_write(uint8_t data);
uint8_t TWI_read_ack(void);
uint8_t TWI_read_nack(void);

#endif /* RTC_H_ */