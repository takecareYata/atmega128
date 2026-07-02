/*
 * lcd.c
 * Created: 2026-07-01
 * Author: yata
 */ 
#include "lcd.h"

// LCD에 신호 유효 타이밍 인가 - E 핀만 확실하게 토글
static void LCD_pulse_enable(void)
{
    LCD_PORT |= (1 << LCD_E);   // E = High
    _delay_us(10);              
    LCD_PORT &= ~(1 << LCD_E);  // E = Low
    _delay_us(10);              
}

// ⚠️ 제어핀(PC0, PC1)을 완벽하게 보호하는 4비트 쓰기 함수
static void LCD_write_4bit(uint8_t nibble)
{
    // 1. 현재 PORTC의 하위 4비트(제어선 상태)를 그대로 보존
    uint8_t current_control = LCD_PORT & 0x0F;
    
    // 2. 보낼 데이터의 상위 4비트만 추출합니다.
    uint8_t data_nibble = nibble & 0xF0;
    
    // 3. 기존 제어선 상태와 데이터 4비트를 명확히 결합하여 출력
    LCD_PORT = data_nibble | current_control;
    
    // 4. 신호 인가
    LCD_pulse_enable();
    _delay_us(200); // 4비트 간격 안정화 마진
}

void LCD_command(uint8_t cmd)
{
	uint8_t sreg = SREG; // 기존 인터럽트 설정 상태(Status Register)를 안전하게 저장
	cli();               // ⚠️ LCD 명령 전송 시작: 인터럽트 잠시 차단 (싱크 보호)
	
	LCD_PORT &= ~(1 << LCD_RS);
	LCD_write_4bit(cmd);
	LCD_write_4bit(cmd << 4);
	
	SREG = sreg;         // ⚠️ 전송 끝: 기존 인터럽트 상태로 원상 복구 (sei() 효과)
	
	if (cmd == 0x01 || cmd == 0x02) {
		_delay_ms(3);
		} else {
		_delay_us(100);
	}
}

void LCD_data(uint8_t data)
{
	uint8_t sreg = SREG; // 기존 인터럽트 설정 상태를 안전하게 저장
	cli();               // ⚠️ LCD 데이터 전송 시작: 인터럽트 잠시 차단 (싱크 보호)
	
	LCD_PORT |= (1 << LCD_RS);
	LCD_write_4bit(data);
	LCD_write_4bit(data << 4);
	
	SREG = sreg;         // ⚠️ 전송 끝: 기존 인터럽트 상태로 원상 복구
	
	_delay_us(100);
}

void LCD_init(void)
{
    // PC0, PC1, PC4, PC5, PC6, PC7 출력 설정
    LCD_DDR |= (1 << LCD_RS) | (1 << LCD_E) | 0xF0;
    LCD_PORT = 0x00;
    
    _delay_ms(100);             // 전원 안정화 대기
    
    // 데이터시트 규격 8비트 모드 가동 및 4비트 전환 인터페이스 시퀀스
    LCD_PORT &= ~(1 << LCD_RS); // RS = 0
    
    LCD_write_4bit(0x30);
    _delay_ms(5);
    
    LCD_write_4bit(0x30);
    _delay_us(150);
    
    LCD_write_4bit(0x30);
    _delay_us(150);
    
    // ⚠️ 4비트 모드로 변경 명령어 수행
    LCD_write_4bit(0x20);
    _delay_ms(2);
    
    // 여기서부터 정상적인 4비트 커맨드 가동
    LCD_command(0x28);          // 4비트 인터페이스, 2라인, 5x8 도트
    LCD_command(0x0C);          // Display ON, Cursor OFF, Blink OFF
    LCD_command(0x06);          // Entry Mode: 커서 우측 이동
    LCD_clear();                // 화면 초기화
}

void LCD_string(char *str)
{
    while (*str)
    {
        LCD_data(*str++);
    }
}

void LCD_clear(void)
{
    LCD_command(0x01);          
    _delay_ms(2);               
}

void LCD_gotoxy(uint8_t x, uint8_t y)
{
    uint8_t address = 0;
    
    if (y == 0) address = 0x80 + x;       
    else if (y == 1) address = 0xC0 + x;  
    
    LCD_command(address);
}