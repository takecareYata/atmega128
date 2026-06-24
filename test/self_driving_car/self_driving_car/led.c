/*
 * led.c
 *
 * Created: 2026-06-19 오후 5:12:05
 *  Author: kccistc
 */ 
#include "led.h"

void init_led(void) {
	// PG3 출력 설정
	DDRG |= (1 << PG3);
	// 초기 상태: LED 끄기
	PORTG &= ~(1 << PG3);
}