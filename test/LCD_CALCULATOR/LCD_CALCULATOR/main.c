/*
 * ChartypeLCD_test.c
 * Created: 2026-07-01
 * Author : yata
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "keypad.h"
#include "button.h"
#include "calculator.h"
#include "uart.h"   
#include "rtc.h"  

// 하드웨어 및 외부 인터페이스 전역 변수
volatile uint32_t button_ms_count = 0; 
extern volatile int uart0_rear;
extern volatile int uart0_front;
extern volatile unsigned char rx_buff[QUEUE_SIZE][QUEUE_LENGTH];

FILE uart_output = FDEV_SETUP_STREAM(uart0_putchar, NULL, _FDEV_SETUP_WRITE);

// 🔄 시스템 모드 제어 변수 (0: 계산기 모드, 1: 시계 모드)
uint8_t system_mode = 0; 

// 계산기 수식 및 상태 관리 변수
char expr_buf[32] = {0,};
uint8_t expr_idx = 0;
uint8_t is_calculated = 0; 

ISR(TIMER0_OVF_vect)
{
    TCNT0 = 6;
    button_ms_count++; // 1ms마다 버튼 타이밍 카운터 증가
}

void init_timer0(void)
{
    TCNT0 = 6;
    TCCR0 = (1 << CS02) | (0 << CS01) | (0 << CS00); // 64분주로 약 1ms 주기 생성
    TIMSK |= (1 << TOIE0);
}

// 계산기 화면 갱신 함수
void refresh_calc_lcd(void)
{
    char lcd_line[17];
    LCD_clear();
    _delay_us(50); 
    
    if (expr_idx == 0) 
    {
        LCD_gotoxy(0, 0);
        LCD_string("Enter Expr...   ");
    } 
    else 
    {
        if (expr_idx > 16) 
        {
            snprintf(lcd_line, 17, "%s", &expr_buf[expr_idx - 16]);
        } 
        else 
        {
            snprintf(lcd_line, 17, "%s", expr_buf);
        }
        LCD_gotoxy(0, 0);
        LCD_string(lcd_line);
    }
    LCD_gotoxy(0, 1);
    LCD_string("Ready-----------");
}

// 🕒 DS1307 실시간 데이터를 읽어와 LCD를 갱신하는 함수
void refresh_clock_lcd(void)
{
    RTC_Time current_time;
    char time_str[17];
    char date_str[17];
    
    // DS1307 하드웨어로부터 TWI 통신을 통해 현재 시각 로드
    RTC_read_time(&current_time);
    
    LCD_clear();
    _delay_us(50);
    
    // 첫 번째 줄: 날짜 출력 (2026-07-01)
    snprintf(date_str, 17, "  20%02d-%02d-%02d   ", current_time.year, current_time.month, current_time.date);
    LCD_gotoxy(0, 0);
    LCD_string(date_str);
    
    // 두 번째 줄: 시간 출력 (13:14:05)
    snprintf(time_str, 17, "    %02d:%02d:%02d    ", current_time.hour, current_time.min, current_time.sec);
    LCD_gotoxy(0, 1);
    LCD_string(time_str);
}

int main(void)
{
    char key = 0;
    int32_t calc_result = 0;
    char result_str[17];
    
    // 시계 갱신 주기 관리를 위한 루프 카운터
    uint16_t clock_refresh_counter = 0; 

    // 하드웨어 장치들 초기화
    init_uart0();          
    stdout = &uart_output; // 👈 main.c에서 정의한 uart_output 객체를 표준 출력에 바인딩
    TWI_init();            // TWI(I2C) 통신 규격 활성화 (100kHz)
    init_timer0();
    init_button();
    init_keypad();
    LCD_init();
    
    sei(); 
    
/*
	RTC_Time init_time = {
		.year = 26,   // 2026년
		.month = 7,   // 7월
		.date = 1,    // 1일
		.hour = 14,   // 14시
		.min = 30,    // 30분
		.sec = 0      // 00초 (이때 CH 비트가 0으로 설정되면서 시계가 굴러갑니다)
	};
	RTC_write_time(&init_time);
*/
    printf("\r\n=== Matrix Calculator & DS1307 RTC Booted ===\r\n");
    
    LCD_clear();
    LCD_gotoxy(0, 0);
    LCD_string("  Matrix Calc   ");
    LCD_gotoxy(0, 1);
    LCD_string("  System Ready  ");
    _delay_ms(1500);
    
	printf("[DEBUG] Forcing DS1307 Oscillator to Start...\r\n");
	
	_delay_ms(100); // 칩이 인지할 시간을 잠시 부여
	
    refresh_calc_lcd();

    while (1)
    {
        uint8_t screen_need_update = 0; 
        
        // ==========================================
        // 1. 모드 토글 제어 (독립 버튼 3 스캔)
        // ==========================================
        if (get_button(3, BUTTON3PIN) == 1) 
        {
            system_mode = !system_mode; // 0 (계산기) <-> 1 (시계) 토글
            printf("[MODE] Switched to %s Mode\r\n", system_mode ? "CLOCK" : "CALCULATOR");
            
            if (system_mode == 0) 
			{
                refresh_calc_lcd();
            } 
			else 
			{
                clock_refresh_counter = 0; // 진입 즉시 화면 갱신
                refresh_clock_lcd(); 
            }
        }

        // ==========================================
        // 2. 현재 활성화된 모드에 따른 분기 처리
        // ==========================================
        if (system_mode == 0) 
        {
            key = keypad_scan(); 
            if (key != 0 && key != ' ') 
            {
                printf("[KEYPAD] Pressed: %c\r\n", key);

                if (is_calculated && key != '=') 
                {
                    expr_idx = 0;
                    expr_buf[0] = '\0';
                    is_calculated = 0;
                }

                if (key == '=') 
                {
                    if (expr_idx > 0) 
                    {
                        printf("[CALC] Evaluating: %s\r\n", expr_buf);
                        if (evaluate_expression(expr_buf, &calc_result)) 
                        {
                            snprintf(result_str, 17, "Ans: %ld", calc_result);
                            printf("[CALC] Success: %ld\r\n", calc_result);
                        } 
                        else 
                        {
                            snprintf(result_str, 17, "Ans: Error      ");
                            printf("[CALC] Error!\r\n");
                        }
                        
                        LCD_gotoxy(0, 1);
                        LCD_string("                "); 
                        LCD_gotoxy(0, 1);
                        LCD_string(result_str);
                        is_calculated = 1; 
                    }
                }
                else if (expr_idx < 30) 
                {
                    expr_buf[expr_idx++] = key;
                    expr_buf[expr_idx] = '\0';
                    screen_need_update = 1;
                }
            }

            // 버튼 0: 백스페이스
            if (get_button(0, BUTTON0PIN) == 1) 
            {
                printf("[BUTTON] 0 -> Backspace\r\n");
                if (is_calculated) { 
                    expr_idx = 0; expr_buf[0] = '\0'; is_calculated = 0; screen_need_update = 1;
                }
                else if (expr_idx > 0) {
                    expr_idx--; expr_buf[expr_idx] = '\0'; screen_need_update = 1;
                }
            }
            
            // 버튼 1: '('
            if (get_button(1, BUTTON1PIN) == 1) 
            {
                printf("[BUTTON] 1 -> '('\r\n");
                if (is_calculated) { expr_idx = 0; is_calculated = 0; }
                if (expr_idx < 30) { expr_buf[expr_idx++] = '('; expr_buf[expr_idx] = '\0'; screen_need_update = 1; }
            }
            
            // 버튼 2: ')'
            if (get_button(2, BUTTON2PIN) == 1) 
            {
                printf("[BUTTON] 2 -> ')'\r\n");
                if (is_calculated) { expr_idx = 0; is_calculated = 0; }
                if (expr_idx < 30) { expr_buf[expr_idx++] = ')'; expr_buf[expr_idx] = '\0'; screen_need_update = 1; }
            }

            if (screen_need_update) 
            {
                refresh_calc_lcd();
            }
        } 
        else 
        {
            // --------------------------------------
            // 🕒 [DS1307 시계 모드 제어 루프]
            // --------------------------------------
            clock_refresh_counter++;
            if (clock_refresh_counter >= 40) // 5ms * 40 = 200ms 주기로 RTC 갱신
            {
                clock_refresh_counter = 0;
                refresh_clock_lcd();
            }
        }
        
        _delay_ms(5); 
    }
}