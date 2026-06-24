/*
 * ultrasonic.c
 *
 * Created: 2026-06-17 오후 1:32:22
 *  Author: yata
 */ 
#include "ultrasonic.h"
#include "timer.h"

volatile int ultrasonic_distance = 0;
volatile char scm[50];
volatile int trigger_flag = 0;

// p278 표 12-3 참고
// INT4 는 PE4이므로 초음파 센서의 상승,하강 에지 발생시 이곳으로 진입
ISR(INT4_vect)
{
	// 1.상승에지
	if ( ECHO_PORT & (1 << ECHO_PIN))
	{
		TCNT1 = 0; //타이머1 초기화
		trigger_flag = 0;
	}
	// 2.하강에지
	else
	{
		// TCNT1 : Timer count 1
		// TCNT1 --> 10 이라고 가정해보자
		// 15.625kHz의 한 주기가 64us 이다.
		// 64us * 10EA == 640us
		// 640us / 58(1cm 이동하는데 소요시간) : 11cm
		// 1sec : 1000000us
		ultrasonic_distance = TCNT1 * 1000000.0 * 1024 /F_CPU;
		sprintf(scm, "dis: %dcm\n", ultrasonic_distance / 58);
		trigger_flag = 1;
	}
}

void init_ultrasonic(void)
{
	TRIG_DDR |= 1 << TRIG_PIN; // output mode
	ECHO_DDR &= ~(1 << ECHO_PIN); // input mode
	EICRB |= 0 << ISC41 | 1 << ISC40; //p289 참조 상승에지, 하강에지 둘다 INT뜨도록 세팅
	
	// 16bit timer count 1번사용
	// timer int는 사용하지 않는다.
	// 16 bit 최대값 65535 , 분주비 1024로 설정
	// 1 / 16000000Hz * 1024 = 64us
	TCCR1B |= 1 << CS12 | 1 << CS10; //1024분주
	
	// ETNT4 설정 p287 그림 12-6
	EIMSK |= 1 << INT4; //ECHO
}

void make_trigger(void)
{
	TRIG_PORT &= ~(1 << TRIG_PIN); // 초기값 LOW
	_delay_us(1);
	TRIG_PORT |= 1 << TRIG_PIN;
	_delay_us(15); // 추가적인 delay 여유값 추가, 규격은 10us
	TRIG_PORT &= ~(1 << TRIG_PIN);
}

void ultrasonic_processsing(void)
{
#if 0
	if (ultrasonic_check_time >= 1000 )
	{
		ultrasonic_check_time = 0;
		printf("%s", scm);
		make_trigger();
	}
#endif
	if (trigger_flag == 1)
	{
		trigger_flag = 0; 
		printf("%s", scm); 
		_delay_ms(100);     
		make_trigger();  
	}
}

void ultrasonic_show_led(void)
{
	int ultrasonic_led_dis = ultrasonic_distance /58;
	if (ultrasonic_led_dis <= 2)
	{
		PORTA = 0x01;
	}
	else if (ultrasonic_led_dis <= 3)
	{
		PORTA = 0x03;
	}
	else if (ultrasonic_led_dis <= 4)
	{
		PORTA = 0x07;
	}
	else if (ultrasonic_led_dis <= 5)
	{
		PORTA = 0x0f;
	}
	else if (ultrasonic_led_dis <= 6)
	{
		PORTA = 0x1f;
	}
	else if (ultrasonic_led_dis <= 7)
	{
		PORTA = 0x3f;
	}
	else if (ultrasonic_led_dis <= 10)
	{
		PORTA = 0x7f;
	}
	else if (ultrasonic_led_dis >= 10)
	{
		PORTA = 0xff;
	}
	
}