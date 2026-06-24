/*
 * fnd.h
 *
 * Created: 2026-06-19 오후 5:10:29
 *  Author: kccistc
 */ 
#ifndef FND_H_
#define FND_H_

#include <avr/io.h>

void init_fnd(void);
void display_1to8(void);
void fnd_multiplex_isr(void); // 타이머 ISR에서 호출할 함수
void update_fnd_autonomous(uint8_t state, uint32_t clock_sec);
void update_fnd_finish(uint32_t clock_sec, uint32_t f, uint32_t b, uint32_t l, uint32_t r);

#endif /* FND_H_ */