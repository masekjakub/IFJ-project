#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int curIndex = 0;


void makeToken(Token *tokensArr, TokenType type, KeyWord keyWord, int intV, double doubleV, char *string, int len)
{
    Token token;
    token.type = type;
    if (type == TYPE_KEYWORD)
        token.atribute.keyword = keyWord;
    if (type == TYPE_INT)
        token.atribute.intV = intV;
    if (type == TYPE_DOUBLE)
        token.atribute.doubleV = doubleV;
    if (type == TYPE_ID){
        token.atribute.dString = (DynamicString *)malloc(sizeof(char *)+sizeof(int));
        token.atribute.dString->string = (char *)malloc((len+2) * sizeof(char));
        strcpy(token.atribute.dString->string,string);
        token.atribute.dString->length = len;
    }
    tokensArr[curIndex] = token;
    curIndex++;
    return;
}

Token *initTokens()
{
    Token *tokensArr = (Token *)malloc(100 * sizeof(Token)); //edit max size if needed!
    if (tokensArr == NULL)
    {
        exit(1);
    }
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var", 3);
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_EQ, 0, 0, NULL, 0);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL, 0);
    return tokensArr;
}

Token getTokenSim(Token *tokenArr, int *index){
    if(*index > curIndex-1){
        printf("ERROR: Reading out of simulated array!\n");
        exit(1);
    }
    Token token = tokenArr[*index];
    *index+=1;
    return token;
}