/**
 * @file stack.h
 * @author Vojtěch Kuchař
 * @brief Header file for stack.c
 * @version 1.0
 * @date 2022-10-28
 */
#ifndef _IFJ_STACK
#define _IFJ_STACK

#include "scanner.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Structure representing stack
typedef struct
{
    Token *tokenArray;
    int size;
} Stack;

Stack *STACK_init();
void STACK_push(Stack *stack, Token token);
void STACK_pop(Stack *stack);
void STACK_popAll(Stack *stack);
void STACK_dispose(Stack *stack);
Token *STACK_top(Stack *stack);
int STACK_isEmpty(Stack *stack);
Token *STACK_bottom(Stack *stack);
void STACK_popBottom(Stack *stack);

#endif