/*
 * fnd.h
 *
 * Created: 2026-06-12 오전 10:45:30
 *  Author: yata
 */ 
#ifndef FND_H_
#define FND_H_
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define FND_DATA_DDR	DDRC
#define FND_DATA_PORT	PORTC

#define FND_DIGIT_DDR	DDRB
#define FND_DIGIT_PORT	PORTB
#define FND_DIGIT_D1	4
#define FND_DIGIT_D2	5
#define FND_DIGIT_D3	6
#define FND_DIGIT_D4	7

extern void init_fnd(void);
extern void fnd_display(void);
extern int fnd_main(void);
extern void fnd_timer(void);
extern void fnd_sec_clock(void);
extern void fnd_stopwatch(void);

extern volatile uint32_t ms_count;
extern volatile uint32_t stopwatch_ms_count;
extern volatile int32_t sec_count; //sec를 재는 count
extern volatile uint8_t dot_display;
extern volatile uint32_t stopwatch_sec_count;
extern volatile uint8_t stopwatch_state;

#endif /* FND_H_ */