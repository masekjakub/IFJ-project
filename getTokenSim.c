#include "getTokenSim.h"

int maxTokens = 100;

int outIndex = 0;
int inIndex = 0;

Token *defineTokens(Token *tokensArr)
{
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var", 3);
    makeToken(tokensArr, TYPE_KEYWORD, KEYWORD_EQ, 0, 0, NULL, 0);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL, 0);
    return tokensArr;
}

void makeToken(Token *tokensArr, TokenType type, KeyWord keyWord, int intV, double doubleV, char *string, int len)
{
    Token token;
    token.type = type;
    switch (type){
        case TYPE_KEYWORD:
            token.atribute.keyword = keyWord;
            break;
        case TYPE_INT:
            token.atribute.intV = intV;
            break;
        case TYPE_DOUBLE:
            token.atribute.doubleV = doubleV;
            break;
        case TYPE_ID:
            token.atribute.dString = (DynamicString *)malloc(sizeof(char *)+sizeof(int));
            token.atribute.dString->string = (char *)malloc((len+2) * sizeof(char));
            strcpy(token.atribute.dString->string,string);
            token.atribute.dString->length = len;
            break;
        default:
            printf("ERROR: Type %d no found!", type);
            break;
    }

    tokensArr[inIndex] = token;
    inIndex++;
    return;
}

Token *initTokens()
{
    Token *tokenArr = (Token *)malloc(maxTokens * sizeof(Token)); // edit max size if needed!
    if (tokenArr == NULL)
    {
        exit(1);
    }
    tokenArr = defineTokens(tokenArr);

    return tokenArr;
}

Token getTokenSim(Token *tokenArr){
    if(outIndex >= inIndex){
        printf("ERROR: Reading out of simulated array!\n");
        exit(1);
    }
    Token token = tokenArr[outIndex];
    outIndex++;
    
    return token;
}