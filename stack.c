/**
 * @file stack.c
 * @author Vojtěch Kuchař
 * @brief Implementation of stack
 * @version 1.0
 * @date 2022-10-28
 */

#include "stack.h"

/**
 * @brief Initializes empty stack of tokens.
 *
 * @return Ptr to initialized stack
 */
Stack *STACK_init()
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->size = 0;
    stack->tokenArray = calloc(1, sizeof(Token));
    return stack;
}

/**
 * @brief Inserts one Token at the of stack.
 *
 * @param stack Stack to push to
 * @param token Token to push
 */
void STACK_push(Stack *stack, Token token)
{
    if (stack->size == 0)
    {
        stack->tokenArray = calloc(1, sizeof(Token));
    }
    else
    {
        stack->tokenArray = realloc(stack->tokenArray, (stack->size + 1) * sizeof(Token));
    }
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
 * @brief Pops stack (from the top).
 *
 * @param stack Statck to pop from
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
 * @param stack Stack to pop from
 */
void STACK_popAll(Stack *stack)
{
    while (stack->size > 0)
    {
        STACK_pop(stack);
    }
    stack->size = 0;
}

/**
 * @brief Disposes the stack and frees memory used by stack.
 *
 * @param stack Stack to dispose
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
 * @param stack Stack to check
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
 * @param stack Stack to get top token from
 * @return Ptr to the token on top of stack
 */
Token *STACK_top(Stack *stack)
{
    if (STACK_isEmpty(stack) == 0)
    {
        return &stack->tokenArray[stack->size - 1];
    }
    return NULL;
}

/**
 * @brief Returns pointer of token from the bottom od the stack.
 * In case of empty stack function returns NULL.
 *
 * @param stack Stack to get bottom token from
 * @return Ptr to the token on bottom of stack
 */
Token *STACK_bottom(Stack *stack)
{
    if (STACK_isEmpty(stack) == 0)
    {
        return &stack->tokenArray[0];
    }
    return NULL;
}

/**
 * @brief Pops stack from bottom.
 *
 * @param stack Stack to pop from
 */
void STACK_popBottom(Stack *stack)
{
    if (stack == NULL || stack->size < 0)
        return;

    for (int i = 0; i < stack->size; i++)
    {
        stack->tokenArray[i] = stack->tokenArray[i + 1];
    }
    stack->tokenArray = realloc(stack->tokenArray, (stack->size - 1) * sizeof(Token));
    stack->size--;
}