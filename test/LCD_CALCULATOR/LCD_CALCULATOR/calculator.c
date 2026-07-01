/*
 * calculator.c
 * Created: 2026-07-01
 * Author : yata
 */ 
#include "calculator.h"
#include <string.h>
#include <ctype.h>

#include <avr/interrupt.h> // 혹시 필요할지 모를 원자성 확보용

// 8비트 MCU의 RAM 용량을 고려한 컴팩트 스택 크기
#define STACK_SIZE 16

static int32_t num_stack[STACK_SIZE];
static char op_stack[STACK_SIZE];
static int8_t num_top = -1;
static int8_t op_top = -1;

// 연산자 우선순위 판단 헬퍼
static int8_t get_precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// 스택 상단 연산 수행
static uint8_t apply_op(void) {
    if (num_top < 1 || op_top < 0) return 0; // 피연산자나 연산자가 부족한 경우
    
    int32_t val2 = num_stack[num_top--];
    int32_t val1 = num_stack[num_top--];
    char op = op_stack[op_top--];
    
    switch (op) {
        case '+': num_stack[++num_top] = val1 + val2; break;
        case '-': num_stack[++num_top] = val1 - val2; break;
        case '*': num_stack[++num_top] = val1 * val2; break;
        case '/': 
            if (val2 == 0) return 0; // ⚠️ 0으로 나누기 에러 방지
            num_stack[++num_top] = val1 / val2; 
            break;
        default: return 0;
    }
    return 1;
}

uint8_t evaluate_expression(const char* expr, int32_t* result_out) {
    num_top = -1;
    op_top = -1;
    uint16_t i = 0;
    uint16_t len = strlen(expr);
    
    while (i < len) {
        // 1. 공백은 가볍게 패스
        if (expr[i] == ' ') {
            i++;
            continue;
        }
        
        // 2. 열린 괄호는 연산자 스택에 바로 푸시
        if (expr[i] == '(') {
            if (op_top >= STACK_SIZE - 1) return 0;
            op_stack[++op_top] = expr[i];
            i++;
        }
        // 3. 숫자가 나오면 다중 자릿수(예: 125)를 한 번에 파싱해서 숫자 스택에 푸시
        else if (isdigit((unsigned char)expr[i])) {
            int32_t val = 0;
            while (i < len && isdigit((unsigned char)expr[i])) {
                val = (val * 10) + (expr[i] - '0');
                i++;
            }
            if (num_top >= STACK_SIZE - 1) return 0;
            num_stack[++num_top] = val;
        }
        // 4. 닫힌 괄호가 나오면 열린 괄호를 만날 때까지 연산 수행
        else if (expr[i] == ')') {
            while (op_top >= 0 && op_stack[op_top] != '(') {
                if (!apply_op()) return 0;
            }
            if (op_top < 0) return 0; // ⚠️ 매칭되는 열린 괄호가 없는 오류
            op_top--; // '(' 제거
            i++;
        }
        // 5. 일반 연산자가 나오면 우선순위를 비교하여 처리 후 푸시
        else if (expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/') {
            while (op_top >= 0 && get_precedence(op_stack[op_top]) >= get_precedence(expr[i])) {
                if (!apply_op()) return 0;
            }
            if (op_top >= STACK_SIZE - 1) return 0;
            op_stack[++op_top] = expr[i];
            i++;
        }
        else {
            return 0; // 유효하지 않은 문자 예외처리
        }
    }
    
    // 남아있는 모든 연산 처리
    while (op_top >= 0) {
        if (op_stack[op_top] == '(') return 0; // 괄호 짝 붕괴 에러
        if (!apply_op()) return 0;
    }
    
    // 최종 결과 도출 검증
    if (num_top == 0) {
        *result_out = num_stack[0];
        return 1; // 성공
    }
    
    return 0; // 에러
}