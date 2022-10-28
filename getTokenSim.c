/**
 * @file getTokenSim.c
 * @author Jakub Mašek
 * @brief simulation of scanner
 * @version 0.1
 * @date 16-10-2022
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "getTokenSim.h"

//max size of token array
int maxTokens = 100;

int outIndex = 0;
int inIndex = 0;

/**
 * @brief save test tokens to token array
 * 
 * @param tokensArr array to save into
 * @return Token array
 */
/*Token *defineTokens(Token *tokensArr)
{
    //prolog
    makeToken(tokensArr, TYPE_BEGIN, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_DECLARE_ST, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_COMM, 0, 0, 0, NULL);

    //$var = 5;
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var");
    makeToken(tokensArr, TYPE_ASSIGN, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 5, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);

    //$var = 7+3.3;
    makeToken(tokensArr, TYPE_ID, 0, 0, 0, "var");
    makeToken(tokensArr, TYPE_ASSIGN, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_INT, 0, 7, 0, NULL);
    makeToken(tokensArr, TYPE_SEMICOLON, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_ADD, 0, 7, 0, NULL);
    makeToken(tokensArr, TYPE_DOUBLE, 0, 3.3, 0, NULL);

    //epilog
    makeToken(tokensArr, TYPE_END, 0, 0, 0, NULL);
    makeToken(tokensArr, TYPE_EOF, 0, 0, 0, NULL);
    return tokensArr;
}*/

/**
 * @brief asign local token to array
 * 
 * @param tokensArr pointer to array of tokens
 * @param type type of tokens
 * @param keyWord keyword to save
 * @param intV integer value to save
 * @param doubleV double value to save
 * @param string string to save
 * @param len lenght of string
 */
void makeToken(Token *tokensArr, TokenType type, KeyWord keyWord, int intV, double doubleV, char *string)
{
    Token token;
    token.type = type;
    switch (type){
        case TYPE_KEYWORD:
            token.attribute.keyword = keyWord;
            break;
        case TYPE_INT:
            token.attribute.intV = intV;
            break;
        case TYPE_DOUBLE:
            token.attribute.doubleV = doubleV;
            break;
        case TYPE_ID:
            token.attribute.dString = DS_init();
            token.attribute.dString = DS_appendString(token.attribute.dString, string);
            break;
        default:
            
            break;
    }

    tokensArr[inIndex] = token;
    inIndex++;
    return;
}

/**
 * @brief init array of tokens
 * 
 * @return Token array
 */
/*Token *initTokens()
{
    Token *tokenArr = (Token *)malloc(maxTokens * sizeof(Token)); // edit max size if needed!
    if (tokenArr == NULL)
    {
        exit(1);
    }
    tokenArr = defineTokens(tokenArr);

    return tokenArr;
}*/

/**
 * @brief simulate function of scanner (send new token each call)
 * 
 * @param tokenArr array of tokens
 * @return Token 
 */
Token getTokenSim(Token *tokenArr){
    // reading out of array
    if(outIndex >= inIndex){
        printf("ERROR: Reading out of simulated array!\n");
        exit(1);
    }
    Token token = tokenArr[outIndex];
    outIndex++;
    
    return token;
}