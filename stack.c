/**
 * @file stack.c
 * @author Vojtěch Kuchař
 * @brief Implementation of stack
 * @version 0.1
 * @date 2022-10-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "scanner.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// stackInit
// array

Token *STACK_init()
{
    Token *stack = (Token *)malloc(sizeof(Token));
    return stack;
}

Token *STACK_insert(Token *stack, Token token)
{

    return stack;
}