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

volatile uint32_t button_ms_count = 0; 
extern volatile int uart0_rear;
extern volatile int uart0_front;
extern volatile unsigned char rx_buff[QUEUE_SIZE][QUEUE_LENGTH];

// 계산기 수식 및 상태 관리 변수
char expr_buf[32] = {0,};
uint8_t expr_idx = 0;
uint8_t is_calculated = 0; 

// 표준 출력 스트림 구조체에 매핑 함수 등록
FILE uart_output = FDEV_SETUP_STREAM(uart0_putchar, NULL, _FDEV_SETUP_WRITE);
// ==========================================

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

// LCD 화면을 깔끔하게 갱신
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

int main(void)
{
    char key = 0;
    int32_t calc_result = 0;
    char result_str[17];

    init_uart0();          
    stdout = &uart_output;    
    init_timer0();
    init_button();
    init_keypad();
    LCD_init();
    
    sei(); 
    
    printf("\r\n=== Matrix Calculator Booted ===\r\n");
    printf("UART Configuration: 9600bps, U2X0=1, RX_INT=Enabled\r\n");
    
    LCD_clear();
    LCD_gotoxy(0, 0);
    LCD_string("  Matrix Calc   ");
    LCD_gotoxy(0, 1);
    LCD_string("  System Ready  ");
    _delay_ms(1500);
    
    refresh_calc_lcd();

    while (1)
    {
        uint8_t screen_need_update = 0; 
        
        // ==========================================
        // 2. 4x4 매트릭스 키패드 입력 처리 (keypad_scan)
        // ==========================================
        key = keypad_scan(); 
        
        if (key != 0 && key != ' ') 
        {
            // 키패드가 감지되면 표준 printf로 PC에 상태 로깅
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
                    printf("[CALC] Evaluating expression: %s\r\n", expr_buf);

                    if (evaluate_expression(expr_buf, &calc_result)) 
					{
                        snprintf(result_str, 17, "Ans: %ld", calc_result);
                        printf("[CALC] Success Result: %ld\r\n", calc_result);
                    } 
					else 
					{
                        snprintf(result_str, 17, "Ans: Error      ");
                        printf("[CALC] Syntax or Math Error!\r\n");
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

        // ==========================================
        // 3. 4개 독립 버튼 제어 (get_button 연동)
        // ==========================================
        
        // 💡 버튼 0: 백스페이스
        if (get_button(0, BUTTON0PIN) == 1) 
        {
            printf("[BUTTON] 0 Pressed -> Backspace\r\n");
            if (is_calculated) 
			{ 
                expr_idx = 0;
                expr_buf[0] = '\0';
                is_calculated = 0;
                screen_need_update = 1;
            }
            else if (expr_idx > 0) 
			{
                expr_idx--;
                expr_buf[expr_idx] = '\0';
                screen_need_update = 1;
            }
        }
        
        // 💡 버튼 1: 괄호 열기 '('
        if (get_button(1, BUTTON1PIN) == 1) 
        {
            printf("[BUTTON] 1 Pressed -> '('\r\n");
            if (is_calculated) 
			{ 
				expr_idx = 0; 
				is_calculated = 0; 
			}
            if (expr_idx < 30) 
			{
                expr_buf[expr_idx++] = '(';
                expr_buf[expr_idx] = '\0';
                screen_need_update = 1;
            }
        }
        
        // 💡 버튼 2: 괄호 닫기 ')'
        if (get_button(2, BUTTON2PIN) == 1) 
        {
            printf("[BUTTON] 2 Pressed -> ')'\r\n");
            if (is_calculated) { expr_idx = 0; is_calculated = 0; }
            if (expr_idx < 30) 
			{
                expr_buf[expr_idx++] = ')';
                expr_buf[expr_idx] = '\0';
                screen_need_update = 1;
            }
        }
        
        // 💡 버튼 3: 모드 전환용 공란
        if (get_button(3, BUTTON3PIN) == 1) 
        {
            printf("[BUTTON] 3 Pressed -> Mode Toggle\r\n");
        }

        // ==========================================
        // 4. 화면 업데이트 펜딩 처리
        // ==========================================
        if (screen_need_update) 
        {
            refresh_calc_lcd();
        }
        
        _delay_ms(5); 
    }
}