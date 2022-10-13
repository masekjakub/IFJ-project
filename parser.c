#include "getTokenSim.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *index = malloc(sizeof(int));
    *index = 0;
    Token *tokenArr = initTokens();

    Token token = getTokenSim(tokenArr, index);
    printf("%s\n", token.atribute.dString->string);

    token = getTokenSim(tokenArr, index);
    printf("%d\n", token.atribute.keyword);

    token = getTokenSim(tokenArr, index);
    printf("%d\n", token.atribute.intV);

    token = getTokenSim(tokenArr, index);
    printf("%d\n", token.atribute.intV);
    return 0;
}