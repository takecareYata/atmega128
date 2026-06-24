/*
 * 03.TIMER_CONTROL.c
 *
 * Created: 2026-06-15 오전 11:36:48
 * Author : yata
 */ 
#define F_CPU	16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


extern void led_main(void);
volatile uint32_t msec_count = 0;

/*
ISR (Interrupt Service Routine) : 인터럽트 처리 함수
TIMER0_OVF_vect : Timer 0 Overflow INT가 발생이되면 이곳으로 진입함.
250개 펄스를 Count (1ms) 하면 이곳으로 자동 진입한다.
ISR은 가능한 짧게 작성한다.
*/
ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6; // TCNT0가 6부터 256이 되는 순간이 250개 펄스를 카운트하게 된다.
	msec_count++; // 1ms count
}

void init_timer0(void);

int main(void)
{
    init_timer0();
	
	led_main();
    while (1) 
    {
		
    }
}
/*
1. timer 0을 초기화한다.
	AVR에서 8bit timer 0번과 2번 중에서 0번을 초기화한다.
	입베디드에서 가장 신경을 써야 할 부분이 초기화하는 것이다.
	초기화가 잘못되면 이후가 다 꼬인다.
2. 8bit 가지고 1ms를 측정하는 timer / counter를 만들고자 한다.
	2.1 분주비를 설정(64분주)
		16000000Hz /64 ==> 250000Hz
	2.2 1주기가 잡아먹는 시간을 계산
		T = 1/f = 1/250000 => 0.000004sec (4us , 0.004ms)
	2.3 8bit 가지고 count하는 시간을 계산 (8bit timer OV (overflow))
		0.004ms X 256EA ==> 1.024ms
		0.004ms X 250EA ==> 1ms
*/
void init_timer0(void)
{
	TCNT0 = 6; // TCNT0가 6부터 256이 되는 순간이 250개 펄스를 카운트하게 된다.
	
	TCCR0 &= ~(1 << CS02 | 1 << CS01 | 1 << CS00); // 0분주 초기화
	TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00; //64분주 세팅
	TIMSK |= 1 << TOIE0; // 오버플로 인터럽트
	sei(); // 전역 interrupt 허용
}

