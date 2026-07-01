/*
 * rtc.c
 * Created: 2026-07-01
 * Author: yata
 */ 
#include "rtc.h"

// 1. TWI(I2C) 통신 초기화 (SCL 속도를 100kHz로 설정)
void TWI_init(void)
{
    // 16MHz 기준 100kHz 공식: SCL Freq = F_CPU / (16 + 2*TWBR * Prescaler)
    TWBR = 72; // 분주비 1일 때 정확히 100kHz
    TWSR = 0;  // 프리스케일러 1 설정 (TWPS0=0, TWPS1=0)
}

// BCD 형태를 10진수 정수로 변환 (예: 0x25 -> 25)
static uint8_t bcd_to_dec(uint8_t val)
{
    return ((val >> 4) * 10) + (val & 0x0F);
}

// 10진수 정수를 BCD 형태로 변환 (예: 25 -> 0x25)
static uint8_t dec_to_bcd(uint8_t val)
{
    return ((val / 10) << 4) | (val % 10);
}

// TWI 시작 신호 송신
static void TWI_start(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

// TWI 정지 신호 송신
static void TWI_stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    _delay_us(10); // 정지 신호 안정화 대기
}

// TWI 1바이트 데이터 송신
static void TWI_write(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

// TWI 1바이트 데이터 수신 (ACK 응답 포함 - 계속 읽을 때)
static uint8_t TWI_read_ack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

// TWI 1바이트 데이터 수신 (NACK 응답 포함 - 마지막 바이트일 때)
static uint8_t TWI_read_nack(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    return TWDR;
}

// RTC로부터 전체 시간 데이터 로드
void RTC_read_time(RTC_Time *time)
{
    TWI_start();
    TWI_write(DS1307_ADDR);   // Slave Address + Write
    TWI_write(0x00);          // 읽기 시작할 레지스터 주소 (00h: 초)
    
    TWI_start();              // Repeated Start 선언
    TWI_write(DS1307_ADDR | 1); // Slave Address + Read
    
    // DS1307 내부 주소 0x00 ~ 0x06까지 순차적으로 데이터 마스킹 후 가져오기
    time->sec   = bcd_to_dec(TWI_read_ack() & 0x7F); // CH비트(최상위) 제외
    time->min   = bcd_to_dec(TWI_read_ack());
    time->hour  = bcd_to_dec(TWI_read_ack() & 0x3F); // 24시간제 기준 마스킹
    time->day   = bcd_to_dec(TWI_read_ack());         // 요일 (1:일 ~ 7:토)
    time->date  = bcd_to_dec(TWI_read_ack());         // 일
    time->month = bcd_to_dec(TWI_read_ack());         // 월
    time->year  = bcd_to_dec(TWI_read_nack());        // 년 (마지막 데이터이므로 NACK)
    
    TWI_stop();
}

// 최초 시간 설정용 함수 (필요시 호출하여 초기값 세팅)
void RTC_write_time(RTC_Time *time)
{
    TWI_start();
    TWI_write(DS1307_ADDR);
    TWI_write(0x00); // 00h번지(초)부터 쓰기 시작
    
    TWI_write(dec_to_bcd(time->sec));
    TWI_write(dec_to_bcd(time->min));
    TWI_write(dec_to_bcd(time->hour));
    TWI_write(dec_to_bcd(time->day));
    TWI_write(dec_to_bcd(time->date));
    TWI_write(dec_to_bcd(time->month));
    TWI_write(dec_to_bcd(time->year));
    
    TWI_stop();
}