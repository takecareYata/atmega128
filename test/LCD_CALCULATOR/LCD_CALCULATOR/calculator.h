/*
 * calculator.h
 * Created: 2026-07-01
 * Author : yata
 */ 
#ifndef CALCULATOR_H_
#define CALCULATOR_H_

#include <stdint.h>

// 수식 문자열을 입력받아 괄호 및 연산자 우선순위를 판별해 최종 결과를 반환하는 함수
// 성공 시 1 반환, 0은 0으로 나누기 오류나 괄호 매칭 실패 등의 에러
uint8_t evaluate_expression(const char* expr, int32_t* result_out);

#endif /* CALCULATOR_H_ */