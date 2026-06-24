#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// [TRIG 핀맵] PORTA의 순수 비트 번호(0, 1, 2) 지정
#define TRIG_DDR       DDRA
#define TRIG_PORT      PORTA
#define TRIG_PIN_L     0         // 왼쪽 센서 TRIG (PORTA0 의미)
#define TRIG_PIN_C     1         // 중앙 센서 TRIG (PORTA1 의미)
#define TRIG_PIN_R     2         // 오른쪽 센서 TRIG (PORTA2 의미)

// [ECHO 핀맵] PORTE의 순수 비트 번호(4, 5, 6) 지정
#define ECHO_DDR       DDRE
#define ECHO_PORT      PINE
#define ECHO_PIN_L     4         // INT4 (PE4 비트 번호 4)
#define ECHO_PIN_C     5         // INT5 (PE5 비트 번호 5)
#define ECHO_PIN_R     6         // INT6 (PE6 비트 번호 6)

// 센서 인덱스 정의
#define US_LEFT        0
#define US_CENTER      1
#define US_RIGHT       2

void init_ultrasonic(void);
void make_trigger(uint8_t sensor_idx);
int get_distance(uint8_t sensor_idx);

#endif /* ULTRASONIC_H_ */