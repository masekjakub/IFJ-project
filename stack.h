/**
 * @file stack.h
 * @author Vojtěch Kuchař
 * @brief Stack header file
 * @version 1.0
 * @date 2022-10-28
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "scanner.h"

typedef struct
{
    Token *tokenArray;
    int size;
} Stack;

Stack *STACK_init();
void STACK_push(Stack *stack, Token token);
Token *STACK_pop(Stack *stack);
void STACK_popAll(Stack *stack);
void STACK_dispose(Stack *stack);
Token *STACK_top(Stack *stack);
int STACK_isEmpty(Stack *stack);
Token *STACK_bottom(Stack *stack);
Token *STACK_popBottom(Stack *stack);
