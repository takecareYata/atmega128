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
volatile uint32_t lcd_time_count = 0;
volatile uint32_t clock_refresh_counter = 0; // 시계 갱신 주기 관리를 위한 루프 카운터
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

// 🛠️ RTC 시간 설정 관련 상태 변수들
uint8_t edit_mode = 0; // 0: 일반 시계 모드, 1: 년, 2: 월, 3: 일, 4: 시, 5: 분, 6: 초 설정 모드
RTC_Time edit_time;    // 설정 중인 임시 시간 데이터를 담을 구조체

ISR(TIMER0_OVF_vect)
{
    TCNT0 = 6;
    button_ms_count++;   // 1ms마다 버튼 타이밍 카운터 증가
    lcd_time_count++;    // 1ms마다 시계모드 타임아웃 카운터 증가
    clock_refresh_counter++; // 1ms마다 시계 디스플레이 갱신 카운터 증가
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
	char lcd_flag = 0;
	int32_t calc_result = 0;
	char result_str[17];

	// 하드웨어 장치들 초기화
	init_uart0();
	stdout = &uart_output;
	TWI_init();            // TWI(I2C) 통신 규격 활성화 (100kHz)
	init_timer0();
	init_button();
	init_keypad();
	LCD_init();
	
	sei();
	
	printf("\r\n=== Matrix Calculator & DS1307 RTC Booted ===\r\n");
	
	LCD_clear();
	LCD_gotoxy(0, 0);
	LCD_string("  Matrix Calc   ");
	LCD_gotoxy(0, 1);
	LCD_string("  System Ready  ");
	_delay_ms(1500);
	
	printf("[DEBUG] Forcing DS1307 Oscillator to Start...\r\n");
	_delay_ms(100);
	
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
				lcd_time_count = 0; // 계산기 모드로 복귀 시 타임아웃 타이머 초기화
				lcd_flag = 0;       // 자동 전환 플래그 초기화
				refresh_calc_lcd();
			}
			else
			{
				// 진입하는 즉시 카운터를 깨끗하게 비우고 화면을 바로 뿌립니다.
				clock_refresh_counter = 0;
				refresh_clock_lcd();
			}
		}
		
		// ⏳ 60초간 입력 없을 시 시계 모드로 자동 변환 (계산기 모드일 때만 동작)
		if (system_mode == 0 && lcd_time_count >= 60000 && lcd_flag == 0)
		{
			system_mode = 1;
			clock_refresh_counter = 0; // 진입 직시 시계창을 띄우기 위해 카운터 초기화
			refresh_clock_lcd();
			lcd_flag = 1;
		}
		
		// ==========================================
		// 2. 현재 활성화된 모드에 따른 분기 처리
		// ==========================================
		if (system_mode == 0)
		{
			// 💡 계산기 모드 내부에서 clock_refresh_counter를 강제로 지우던 코드는 삭제했습니다.
			// uint32_t 변수이므로 49일 동안 오버플로우 걱정 없이 안정적으로 상향 누적됩니다.

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
				lcd_time_count = 0;
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
				lcd_time_count = 0;
				if (is_calculated) { expr_idx = 0; is_calculated = 0; }
				if (expr_idx < 30) { expr_buf[expr_idx++] = '('; expr_buf[expr_idx] = '\0'; screen_need_update = 1; }
			}
			
			// 버튼 2: ')'
			if (get_button(2, BUTTON2PIN) == 1)
			{
				printf("[BUTTON] 2 -> ')'\r\n");
				lcd_time_count = 0;
				if (is_calculated) { expr_idx = 0; is_calculated = 0; }
				if (expr_idx < 30) { expr_buf[expr_idx++] = ')'; expr_buf[expr_idx] = '\0'; screen_need_update = 1; }
			}

			if (screen_need_update)
			{
				refresh_calc_lcd();
			}
			
			_delay_ms(5);
		}
		
		else
		{
			// --------------------------------------
			// 🕒 [DS1307 시계 모드 제어 루프]
			// --------------------------------------
			
			// =======================================================
			// 버튼 0: 변경 모드 진입 및 항목 순환 (일반 -> 년 -> 월 -> 일 -> 시 -> 분 -> 초 -> 저장 후 일반)
			// =======================================================
			if (get_button(0, BUTTON0PIN) == 1)
			{
				edit_mode++;
				if (edit_mode > 6) // 초 설정까지 끝나고 누르면 저장 및 복귀
				{
					edit_mode = 0;
					cli();
					RTC_write_time(&edit_time); // 변경된 임시 데이터를 DS1307 하드웨어에 최종 기록
					sei();
					printf("[RTC_SETUP] Time Saved Successfully!\r\n");
				}
				else
				{
					// 일반 모드(0)에서 최초로 년도 설정 모드(1)로 진입할 때만 현재 실시간 시간을 뼈대로 가져옴
					if (edit_mode == 1)
					{
						cli();
						RTC_read_time(&edit_time);
						sei();
					}
					printf("[RTC_SETUP] Mode Switched. Current Edit Item Code: %d\r\n", edit_mode);
				}
				clock_refresh_counter = 1000; // 버튼 누르면 LCD에 즉시 반영되도록 트리거
			}

			// =======================================================
			// 버튼 1: 값 감소 '('
			// =======================================================
			if (get_button(1, BUTTON1PIN) == 1)
			{
				if (edit_mode > 0) // 설정 모드일 때만 동작
				{
					switch (edit_mode)
					{
						case 1: if (edit_time.year > 0)  edit_time.year--;  else edit_time.year = 99; break; // 년 (00~99)
						case 2: if (edit_time.month > 1) edit_time.month--; else edit_time.month = 12; break; // 월 (01~12)
						case 3: // 일 감소 (01 ~ 각 달의 최대 일수)
						{
							uint8_t max_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
							// 윤년 처리 (4로 나누어 떨어지는 해는 2월이 29일)
							if (edit_time.year % 4 == 0) max_days[1] = 29;
							
							if (edit_time.date > 1) edit_time.date--;
							else edit_time.date = max_days[edit_time.month - 1];
						}
						break;
						case 4: if (edit_time.hour > 0) edit_time.hour--; else edit_time.hour = 23; break; // 시 (00~23)
						case 5: if (edit_time.min > 0)  edit_time.min--;  else edit_time.min = 59;  break; // 분 (00~59)
						case 6: if (edit_time.sec > 0)  edit_time.sec--;  else edit_time.sec = 59;  break; // 초 (00~59)
					}
					printf("[RTC_SETUP] Item %d Decreased\r\n", edit_mode);
					clock_refresh_counter = 1000; // 수정 즉시 반영 화면 갱신
				}
			}

			// =======================================================
			// 버튼 2: 값 증가 ')'
			// =======================================================
			if (get_button(2, BUTTON2PIN) == 1)
			{
				if (edit_mode > 0) // 설정 모드일 때만 동작
				{
					switch (edit_mode)
					{
						case 1: if (edit_time.year < 99)  edit_time.year++;  else edit_time.year = 0; break; // 년
						case 2: if (edit_time.month < 12) edit_time.month++; else edit_time.month = 1; break; // 월
						case 3: // 일 증가
						{
							uint8_t max_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
							if (edit_time.year % 4 == 0) max_days[1] = 29;
							
							if (edit_time.date < max_days[edit_time.month - 1]) edit_time.date++;
							else edit_time.date = 1;
						}
						break;
						case 4: if (edit_time.hour < 23) edit_time.hour++; else edit_time.hour = 0; break; // 시
						case 5: if (edit_time.min < 59)  edit_time.min++;  else edit_time.min = 0;  break; // 분
						case 6: if (edit_time.sec < 59)  edit_time.sec++;  else edit_time.sec = 0;  break; // 초
					}
					printf("[RTC_SETUP] Item %d Increased\r\n", edit_mode);
					clock_refresh_counter = 1000; // 수정 즉시 반영 화면 갱신
				}
			}
			
			// =======================================================
			// 🕒 LCD 디스플레이 주기적 갱신 제어 (1초 주기)
			// =======================================================
			if (clock_refresh_counter >= 1000)
			{
				clock_refresh_counter = 0;
				
				if (edit_mode == 0)
				{
					// 1. 일반 시계 모드: 인터럽트를 짧게 닫고 실시간으로 DS1307을 읽어와서 출력
					refresh_clock_lcd();
				}
				else
				{
					// 2. 설정 모드: DS1307을 읽지 않고 현재 수정 중인 임시 버퍼(edit_time) 정보를 LCD에 렌더링
					// 내가 현재 어떤 항목을 고치고 있는지 유저에게 시각적으로 알려주는 처리를 포함.
					char time_str[17];
					char date_str[17];
					
					// 기본 문자열 가공
					snprintf(date_str, 17, "  20%02d-%02d-%02d   ", edit_time.year, edit_time.month, edit_time.date);
					snprintf(time_str, 17, "    %02d:%02d:%02d    ", edit_time.hour, edit_time.min, edit_time.sec);
					
					// 현재 수정 중인 항목 뒤에 * 표시
					switch (edit_mode)
					{
						case 1: date_str[6] = '*'; break;  // 년도 뒤에 표시
						case 2: date_str[9] = '*'; break;  // 월 뒤에 표시
						case 3: date_str[12] = '*'; break; // 일 뒤에 표시
						case 4: time_str[6] = '*'; break;  // 시간 뒤에 표시
						case 5: time_str[9] = '*'; break;  // 분 뒤에 표시
						case 6: time_str[12] = '*'; break; // 초 뒤에 표시
					}
					
					LCD_gotoxy(0, 0);
					LCD_string(date_str);
					LCD_gotoxy(0, 1);
					LCD_string(time_str);
				}
			}
		}
	}
}