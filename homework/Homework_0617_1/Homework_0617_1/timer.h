/*
 * timer.h
 *
 * Created: 2026-06-15 오후 2:54:37
 *  Author: kccistc
 */ 


#ifndef TIMER_H_
#define TIMER_H_

extern void init_timer0(void);
extern void init_timer2(void);

extern volatile uint32_t msec_count;
extern volatile int ultrasonic_check_time;

#endif /* TIMER_H_ */