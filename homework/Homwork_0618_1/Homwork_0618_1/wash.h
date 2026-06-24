#ifndef WASH_H_
#define WASH_H_

//세탁기 상태 천이도의 상태(State) 정의
typedef enum {
	STATE_STANDBY = 0,          // 대기모드
	STATE_WASH,                 // 세탁 (WASH)
	STATE_RINSE,                // 헹굼
	STATE_SPIN,                 // 탈수
	STATE_SET_WASH_TIME,        // 세탁 시간 설정
	STATE_SET_RINSE_TIME,       // 헹굼 시간 설정
	STATE_SET_SPIN_TIME         // 탈수 시간 설정
} SystemState;

//외부 입력 및 내부 타이머 이벤트 정의
typedef enum {
	EVENT_NONE = 0,
	EVENT_BTN0,                 // BTN0 (동작/대기 전환)
	EVENT_BTN1,                 // BTN1 (모드 전환 / 설정 이동)
	EVENT_BTN2,
	EVENT_BTN3,                 // BTN2 (시간 변경 및 Self-loop)
	EVENT_TIMEOUT               // 시간 종료 (WASH -> RINSE -> SPIN 순차 천이)
} SystemEvent;

//시스템 제어 변수 관리 구조체
typedef struct {
	SystemState currentState;       // 현재 시스템 상태
	unsigned int washTime;          // 세탁(WASH) 설정 시간 (초 단위)
	unsigned int rinseTime;         // 헹굼 설정 시간 (초 단위)
	unsigned int spinTime;          // 탈수 설정 시간 (초 단위)
	unsigned int currentRemaining;  // 작동 중인 모드의 남은 시간
} WashingMachine;

void init_wash(WashingMachine *fsm);
void wash_handle_event(WashingMachine *fsm, SystemEvent event);
void wash_update_timer(WashingMachine *fsm);

#endif /* WASH_H_ */