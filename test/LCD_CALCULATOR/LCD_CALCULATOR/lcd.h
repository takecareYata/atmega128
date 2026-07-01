/*
 * lcd.h
 * Created: 2026-07-01
 * Author: yata
 */ 
#ifndef LCD_H_
#define LCD_H_

#define F_CPU 16000000UL // 16MHz 클록
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// 핀 매핑 정의 (PORTC 사용)
#define LCD_PORT      PORTC
#define LCD_DDR       DDRC
#define LCD_RS        PC0
#define LCD_E         PC1

// D4 ~ D7 데이터 핀은 PC4 ~ PC7에 대응됩니다.

void LCD_init(void);
void LCD_command(uint8_t cmd);
void LCD_data(uint8_t data);
void LCD_string(char *str);
void LCD_clear(void);
void LCD_gotoxy(uint8_t x, uint8_t y);

#endif /* LCD_H_ */