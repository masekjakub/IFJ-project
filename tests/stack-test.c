#include "../stack.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printStack(Stack *stack)
{
    if (STACK_top(stack) != NULL)
    {
        printf("Top: %d     Bot: %d    | ", STACK_top(stack)->attribute.intV, STACK_bottom(stack)->attribute.intV);
        for (int i = 0; i < stack->size; i++)
        {
            printf(" %d", stack->tokenArray[i].attribute.intV);
        }
        printf("\n");
    }
    if (STACK_top(stack) == NULL)
    {
        printf("Top: NULL");
    }
    if (STACK_bottom(stack) == NULL)
    {
        printf("  Bot: NULL |\n");
    }
}

int main()
{
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

    Stack *stack = STACK_init();
    printStack(stack);

    STACK_push(stack, token1);
    STACK_push(stack, token2);
    STACK_push(stack, token3);
    printStack(stack);

    STACK_popBottom(stack);
    printStack(stack);
    return 0;
}