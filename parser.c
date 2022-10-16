#include "getTokenSim.h" //simulation DELETE ME
//#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    Token *tokenArr = initTokens();

    Token token = getTokenSim(tokenArr);
    printf("%s\n", token.atribute.dString->string);

    token = getTokenSim(tokenArr);
    printf("%d\n", token.atribute.keyword);

    token = getTokenSim(tokenArr);
    printf("%d\n", token.atribute.intV);

    token = getTokenSim(tokenArr);
    printf("%d\n", token.atribute.intV);

    return 0;
}