/*
 * 08.PWM_SERVO_PIEZO_BUZZER.c
 *
 * Created: 2026-06-25 오전 9:46:25
 * Author : kccistc
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "pwm.h"
#include "button.h"

#define TONE_1KHZ   999
#define TONE_2KHZ   499
#define TONE_3KHZ   332
#define TONE_4KHZ   249

#define TONE_261HZ  3830
#define TONE_329HZ  3038
#define TONE_392HZ  2550
#define TONE_554HZ  1804

extern void init_speaker(void);
extern void delay_ms(int ms);
extern void Music_Player(int *tone, int *Beats);

extern const int LG_Washer_Tune[];
extern const int LG_Washer_Beats[];

volatile int play_state = 0; // 0: 정지상태, 1: 전원멜로디, 2: 오픈멜로디, 3: 세탁종료멜로디
int power_on_step = 0;
int open_buzzer_step = 0;

int check_stop_condition(int current_btn) 
{
	if (get_button(current_btn, current_btn + 3)) 
	{
		OCR3A = 0;
		play_state = 0;
		return 1; 
	}
	return 0;
}

void power_on_melody(void)
{
	power_on_step++;
	if (power_on_step > 5) power_on_step = 1;

	switch (power_on_step) 
	{
		case 1:
		OCR3A = TONE_1KHZ;
		_delay_ms(70);
		break;
		case 2:
		OCR3A = TONE_2KHZ;
		_delay_ms(70);
		break;
		case 3:
		OCR3A = TONE_3KHZ;
		_delay_ms(70);
		break;
		case 4:
		OCR3A = TONE_4KHZ;
		_delay_ms(70);
		break;
		case 5:
		OCR3A = 0;
		delay_ms(3000);
		break;
	}
	OCR3A = 0;
	play_state = 0;
}

void open_buzzer(void)
{
	open_buzzer_step++;
	if (open_buzzer_step > 5) open_buzzer_step = 1;

	switch (open_buzzer_step) 
	{
		case 1:
		OCR3A = TONE_261HZ;
		_delay_ms(70);
		break;
		case 2:
		OCR3A = TONE_329HZ;
		_delay_ms(70);
		break;
		case 3:
		OCR3A = TONE_392HZ;
		_delay_ms(70);
		break;
		case 4:
		OCR3A = TONE_554HZ;
		_delay_ms(70);
		break;
		case 5:
		OCR3A = 0;
		delay_ms(3000);
		break;
	}
	
	OCR3A = 0;
	play_state = 0;
}

void drum_washing_stop_melody(void) 
{
	int *tone = (int *)LG_Washer_Tune;
	int *beats = (int *)LG_Washer_Beats;
	
	while(*tone != '\0') 
	{
		OCR3A = *tone;
		
		for(int t = 0; t < *beats; t++) 
		{
			_delay_ms(1);
			if(check_stop_condition(BUTTON2)) return;
		}
		
		tone++;
		beats++;
		OCR3A = 0;
		
		for(int t = 0; t < 10; t++) 
		{
			_delay_ms(1);
			if(check_stop_condition(BUTTON2)) return;
		}
	}
	
	play_state = 0;
}

int main(void)
{
	init_speaker();
	init_button();
	
	while (1)
	{
	
		if (play_state == 0) {
			if (get_button(BUTTON0, BUTTON0PIN)) 
			{ 
				play_state = 1;
			}
			else if (get_button(BUTTON1, BUTTON1PIN)) 
			{
				play_state = 2;
			}
			else if (get_button(BUTTON2, BUTTON2PIN)) 
			{ 
				play_state = 3;
			}
		}
		
		switch(play_state) {
			case 1:
			power_on_melody();
			break;
			case 2:
			open_buzzer();
			break;
			case 3:
			drum_washing_stop_melody();
			break;
			default:
			OCR3A = 0; 
			break;
		}
		
		_delay_ms(1);
	}
	return 0;
}
