#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "timer.h"
#include "motor.h"
#include "fnd.h"
#include "button.h"
#include "led.h"
#include "ultrasonic.h"
#include "bt.h"

// 🔢 버튼 순환용 7단계 세부 모드 정의 (회원님 사양 반영)
#define MODE_REMOTE_CONTROL      0  // 1. 유선 제어 모드 (기존 MODE_BLUETOOTH 대체)
#define MODE_AUTONOMOUS          1  // 2. 자율 주행 모드
#define MODE_SHOW_TIME           2  // 3. 정지 모드 (자율주행 최종 누적 시간 출력)
#define MODE_SHOW_FORWARD        3  // 4. 전진 횟수 출력
#define MODE_SHOW_BACKWARD       4  // 5. 후진 횟수 출력
#define MODE_SHOW_LEFT           5  // 6. 좌회전 횟수 출력
#define MODE_SHOW_RIGHT          6  // 7. 우회전 횟수 출력

#define STATE_STOP       0
#define STATE_FORWARD    1
#define STATE_BACKWARD   2
#define STATE_LEFT       3
#define STATE_RIGHT      4
#define STATE_SHOW_TIME	 5

extern volatile uint32_t button_ms_count;
extern volatile uint32_t clock_sec_count;
extern volatile uint32_t ultrasonic_ms_count;
extern volatile uint32_t fnd_ms_count;
extern volatile uint8_t rx_data;
extern volatile uint8_t rx_ready_flag;

int main(void)
{	
	uint8_t current_mode = MODE_REMOTE_CONTROL;
	uint8_t active_sensor = US_LEFT;
	uint8_t current_state = STATE_STOP;
	uint8_t last_state = STATE_STOP;

	// 자율주행 방향 제어 카운터
	uint32_t count_forward = 0;
	uint32_t count_backward = 0;
	uint32_t count_left = 0;
	uint32_t count_right = 0;
	
	// 자율주행 종료 시점의 주행 시간 박제용 변수
	uint32_t final_autonomous_time = 0;

	init_timer0();
	init_motor();
	init_fnd();
	init_button();
	init_ultrasonic();
	init_bt();

	sei();
	
	motor_stop();
	current_state = STATE_STOP;
	LED_OFF();

	printf("\n=== System Boot Success (7-Step Mission Mode Ready) ===\n");

	while (1)
	{
		// 1. [FND 7단계 관리]
		if (fnd_ms_count >= 1)
		{
			fnd_ms_count = 0;
			
			switch (current_mode)
			{
				case MODE_REMOTE_CONTROL:
				update_fnd_autonomous(current_state, clock_sec_count);
				break;
				case MODE_AUTONOMOUS:
				update_fnd_autonomous(current_state, clock_sec_count);
				break;
				case MODE_SHOW_TIME:
				update_fnd_autonomous(STATE_SHOW_TIME, final_autonomous_time);
				break;
				case MODE_SHOW_FORWARD:
				update_fnd_autonomous(STATE_FORWARD, count_forward);
				break;
				case MODE_SHOW_BACKWARD:
				update_fnd_autonomous(STATE_BACKWARD, count_backward);
				break;
				case MODE_SHOW_LEFT:
				update_fnd_autonomous(STATE_LEFT, count_left);
				break;
				case MODE_SHOW_RIGHT:
				update_fnd_autonomous(STATE_RIGHT, count_right);
				break;
			}
			
			fnd_multiplex_isr();
		}

		// 2. [초음파 3채널 순차 트리거] 
		if (ultrasonic_ms_count >= 60)
		{
			ultrasonic_ms_count = 0;
			make_trigger(active_sensor);
			active_sensor++;
			if (active_sensor > US_RIGHT) active_sensor = US_LEFT;
		}

		// 3. [유선 모드 전용 시계 관리] 
		if (current_mode == MODE_REMOTE_CONTROL)
		{
			clock_sec_count = 0; // 유선 모드 시 시계 초기화 유지
			
			// UART 인터럽트 수신 완료 플래그가 들어왔을 때 실행
			if (rx_ready_flag == 1)
			{
				uint8_t cmd = rx_data;
				rx_ready_flag = 0; // 플래그 클리어

				switch (cmd)
				{
					case 'f': case 'F': motor_forward(150);  current_state = STATE_FORWARD;  break;
					case 'b': case 'B': motor_backward(150); current_state = STATE_BACKWARD; break;
					case 'l': case 'L': motor_left(150);     current_state = STATE_LEFT;     break;
					case 'r': case 'R': motor_right(150);    current_state = STATE_RIGHT;    break;
					case 's': case 'S': motor_stop();         current_state = STATE_STOP;     break;
					default:	break;
				}
			}
		}
		// 4. [버튼 입력 처리] 7단계 순환 시퀀스
		if (get_button(0, BUTTON0PIN) == 1)
		{
			if (current_mode == MODE_REMOTE_CONTROL)
			{
				current_mode = MODE_AUTONOMOUS;
				LED_ON();
				clock_sec_count = 0; // 자율주행 진입 시 시계 영점 조절
				count_forward = 0; count_backward = 0; count_left = 0; count_right = 0;
				current_state = STATE_STOP;
				last_state = STATE_STOP;
				printf("\n>>> SWITCH TO AUTONOMOUS MODE <<<\n\n");
			}
			else if (current_mode == MODE_AUTONOMOUS)
			{
				current_mode = MODE_SHOW_TIME;
				LED_OFF();
				motor_stop();
				current_state = STATE_STOP;
				
				final_autonomous_time = clock_sec_count; // 자율주행 끝난 시점의 시간 박제
				printf("\n>>> MISSION FINISHED! TIME: %ld sec <<<\n\n", final_autonomous_time);
			}
			else if (current_mode == MODE_SHOW_TIME)     current_mode = MODE_SHOW_FORWARD;
			else if (current_mode == MODE_SHOW_FORWARD)  current_mode = MODE_SHOW_BACKWARD;
			else if (current_mode == MODE_SHOW_BACKWARD) current_mode = MODE_SHOW_LEFT;
			else if (current_mode == MODE_SHOW_LEFT)     current_mode = MODE_SHOW_RIGHT;
			else if (current_mode == MODE_SHOW_RIGHT)
			{
				current_mode = MODE_REMOTE_CONTROL;
				motor_stop();
				current_state = STATE_STOP;
				clock_sec_count = 0;
				printf("\n>>> RESET TO REMOTE CONTROL MODE <<<\n\n");
			}
		}

		// 5. [자율주행 알고리즘 및 상태 매핑]
		if (current_mode == MODE_AUTONOMOUS)
		{
			int dist_L = get_distance(US_LEFT);
			int dist_C = get_distance(US_CENTER);
			int dist_R = get_distance(US_RIGHT);

			if (dist_C > 0 && dist_C < 10 && dist_L > 0 && dist_L < 10 && dist_R > 0 && dist_R < 10)
			{
				motor_backward(150);
				current_state = STATE_BACKWARD;
			}
			else if (dist_C > 0 && dist_C < 10)
			{
				if (dist_L > dist_R) { motor_left(150);  current_state = STATE_LEFT; }
				else                 { motor_right(150); current_state = STATE_RIGHT; }
			}
			else if (dist_L > 0 && dist_L < 10)
			{
				motor_right(150);
				current_state = STATE_RIGHT;
			}
			else if (dist_R > 0 && dist_R < 10)
			{
				motor_left(150);
				current_state = STATE_LEFT;
			}
			else
			{
				motor_forward(150);
				current_state = STATE_FORWARD;
			}

			if (current_state != last_state)
			{
				switch (current_state)
				{
					case STATE_FORWARD:  count_forward++;  break;
					case STATE_BACKWARD: count_backward++; break;
					case STATE_LEFT:     count_left++;     break;
					case STATE_RIGHT:    count_right++;    break;
				}
				last_state = current_state;
			}
		}
		else if (current_mode >= MODE_SHOW_TIME)
		{
			motor_stop();
			current_state = STATE_STOP;
		}
	}
	return 0;
}