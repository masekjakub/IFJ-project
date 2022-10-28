#include "../stack.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printStack(Stack *stack)
{
    if (STACK_top(stack) != NULL)
    {
        printf("Top: %d |", STACK_top(stack)->attribute.intV);
        for (int i = 0; i < stack->size; i++)
        {
            printf(" %d", stack->tokenArray[i].attribute.intV);
        }
        printf("\n");
    }
    else
    {
        printf("Top: NULL\n");
    }
}

int main()
{
    Stack *stack = STACK_init();

    Token token1, token2, token3;
    (void)token1;
    (void)token2;
    (void)token3;
    token1.attribute.intV = 1;
    token1.type = TYPE_INT;

    token2.attribute.intV = 2;
    token2.type = TYPE_INT;

    token3.attribute.intV = 3;
    token3.type = TYPE_INT;

    // printf("%d\n", STACK_isEmpty(stack));
    printStack(stack);
    STACK_push(stack, token1);
    STACK_push(stack, token2);
    STACK_push(stack, token2);
    STACK_push(stack, token2);
    STACK_push(stack, token2);
    STACK_push(stack, token2);
    STACK_push(stack, token2);
    STACK_push(stack, token2);
    STACK_push(stack, token2);
    STACK_push(stack, token3);
    printStack(stack);
    STACK_popAll(stack);
    printStack(stack);
    return 0;
}