/**
 * @file stack.c
 * @author Vojtěch Kuchař
 * @brief Implementation of stack
 * @version 1.0
 * @date 2022-10-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "stack.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Initializes empty stack of tokens.
 *
 * @return Stack*
 */
Stack *STACK_init()
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->size = 0;
    stack->tokenArray = calloc(1, sizeof(Token));
    // Stack stack = calloc(1, sizeof(Token));
    return stack;
}

/**
 * @brief Inserts one Token at the of stack.
 *
 * @param stack
 * @param token
 * @return Stack*
 */
void STACK_push(Stack *stack, Token token)
{
    stack->tokenArray = realloc(stack->tokenArray, (stack->size + 1) * sizeof(Token));
    if (stack->tokenArray == NULL)
    {
        exit(ERR_INTERN);
    }
    else
    {
        stack->tokenArray[stack->size] = token;
        stack->size++;
    }
}

/**
 * @brief Function pops the top of the stack.
 * In case of empty stack, function does nothing.
 *
 * @param stack
 */
void STACK_pop(Stack *stack)
{
    if (stack->size > 0)
    {
        stack->tokenArray = realloc(stack->tokenArray, (stack->size - 1) * sizeof(Token));
        stack->size--;
    }
}

/**
 * @brief Pops all tokens in stack.
 *
 * @param stack
 */
void STACK_popAll(Stack *stack)
{
    while (stack->size > 0)
    {
        STACK_pop(stack);
    }
}

/**
 * @brief Disposes the stack and frees memory used by stack.
 *
 * @param stack
 */
void STACK_dispose(Stack *stack)
{
    STACK_popAll(stack);
    free(stack->tokenArray);
    free(stack);
}

/**
 * @brief Returns 1 if stack is empty and 0 if not.
 *
 * @param stack
 */
int STACK_isEmpty(Stack *stack)
{
    if (stack->size == 0)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief Returns pointer of token on top od the stack.
 * In case of empty stack function returns NULL.
 *
 * @param stack
 * @return Token*
 */
Token *STACK_top(Stack *stack)
{
    if (STACK_isEmpty(stack) == 0)
    {
        return &stack->tokenArray[stack->size - 1];
    }
    return NULL;
}