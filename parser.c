/**
 * @file parser.c
 * @author Jakub Mašek
 * @brief parser for IFJ22 translator
 * @version 0.1
 * @date 16-10-2022
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "parser.h"
#define isKeyword(TOKEN, KEYWORD) TOKEN.attribute.keyword == KEYWORD
#define canBeAssigned(TYPE) (TYPE == TYPE_INT || TYPE == TYPE_FLOAT || TYPE == TYPE_STRING || TYPE == TYPE_FUNID)
#define isOperator(TYPE) (TYPE == TYPE_ADD || TYPE == TYPE_SUB || TYPE == TYPE_MUL || TYPE == TYPE_DIV || TYPE == TYPE_MOD \
    || TYPE == TYPE_EQTYPES || TYPE == TYPE_NOTEQTYPES || TYPE == TYPE_LESS || TYPE == TYPE_GREATER || TYPE == TYPE_LESSEQ ||    \
    TYPE == TYPE_GREATEREQ || TYPE == TYPE_CONCAT)

Symtable *globalST; // global symtable
Symtable *localST;  // local symtable
int isGlobal = 1;   // program is not in function

Token *tokenArr; // simulation

/**
 * @brief Free symtables
 *
 */
void freeST()
{
    if (globalST != NULL)
        ST_freeTable(globalST);
    if (localST != NULL)
        ST_freeTable(localST);
}

/**
 * @brief Returns new token from scanner
 * 
 * @param includingComms 
 * @return Token 
 */
Token newToken(int includingComms)
{
    Token token = getTokenSim(tokenArr); // odstranit tokenarr

    if (!includingComms && token.type == TYPE_COMM)
        token = newToken(includingComms);
    return token;
}


void makeError(ErrorType err){
    //exit(err);
}

/**
 * @brief Rule for assign ($id =)
 * 
 * @param tokenStack 
 * @param programStack 
 * @return int error
 */
/*int ruleAssign(Stack *tokenStack, Stack *programStack) //predelat
{
    int err;
    Token token = newToken(tokenStack, 0);
    Token IDtoken = token;
    STACK_push(programStack, token);

    //search var ID in ST
    STItem *item = ST_searchTable(getTable(isGlobal), DS_string(IDtoken.attribute.dString));

    token = nextToken(tokenStack, 0);
    STACK_push(programStack, token);
    
    // after ID =
    token = nextToken(tokenStack, 0);
    STACK_push(tokenStack, token);
    TokenType typeFound = TYPE_UNDEF;
    while (token.type != TYPE_SEMICOLON){
        if (canBeAssigned(token.type)){
            // two different types in assigned expression
            if(!isSameType(&typeFound, token.type)){
                return ERR_TYPE;
            }
            
            // two functions in assignment
            if (token.type == typeFound && typeFound == TYPE_FUNID)
            {
                return ERR_TYPE;
            }
        }
        token = newToken(0);
        STACK_push(tokenStack, token);
    }

    if (item == NULL) // not found in ST
    { 
        STItemData STdata;
        STdata.varData.VarType = getTypeChar(typeFound);
        STdata.varData.initialized = 1;
        //STdata.varData.VarPosition   --dodelat
        ST_insertItem(getTable(isGlobal), DS_string(IDtoken.attribute.dString), ST_ITEM_TYPE_VARIABLE, STdata);
    }
    else // check type of ID and assigned expression in ST
    {
        if (getTypeChar(typeFound) != item->data.varData.VarType)
            return ERR_TYPE;
    }

    // programStack: ID =
    STACK_popAll(tokenStack); // az bude ruleExpression smazat!
    return err;
}*/

/**
 * @brief Get the Table object
 *
 * @param global
 * @return Symtable*
 */
Symtable *getTable(int global)
{
    if (global)
        return globalST;
    return localST;
}

/**
 * @brief Check if types are same, if curType is undefined, saves newType to curType
 *
 * @param curType
 * @param newType
 * @return int
 */
int isSameType(TokenType *curType, TokenType newType)
{
    if (*curType == TYPE_UNDEF)
    {
        *curType = newType;
        return 1;
    }

    if (*curType == newType)
    {
        return 1;
    }
    return 0;
}

/**
 * @brief Get the char representation of TokenType ('i','f','s')
 *
 * @param type
 * @return ascii value of char
 */
int getTypeChar(TokenType type)
{
    if (type == TYPE_INT)
        return 'i';
    else if (type == TYPE_FLOAT)
        return 'f';
    else if (type == TYPE_STRING)
        return 's';
    return 0;
}

// ---------RULES----------

/**
 * @brief Checks if prolog is right
 *
 * @param tokenArr
 * @return int error code
 */
ErrorType ruleProg() // remove tokenArr SIMULATION
{
    ErrorType err = 0;
    Token token = newToken(1);
    // prolog
    if (token.type != TYPE_BEGIN)
    {
        fprintf(stderr, "Expected <?php at beggining of the file!\n");
        makeError(ERR_SYN);
        return (ERR_SYN);
    }

    token = newToken(1); // simulation
    while (token.type == TYPE_COMM)
    {
        token = newToken(1);
    }

    if (token.type != TYPE_DECLARE_ST)
    {
        fprintf(stderr, "Expected \"declare(strict_types=1);\" at beggining of the file!\n");
        makeError(ERR_SYN);
        return (ERR_SYN);
    }

    token = newToken(1);
    if (token.type != TYPE_SEMICOLON)
    {
        fprintf(stderr, "Expected \";\" after declare(strict_types=1) on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }

    // st-list
    err = ruleStatList();
    return err;
}


ErrorType ruleStatList()
{
    ErrorType err = 0;
    Token token = newToken(0);

    // epilog
    if (token.type == TYPE_EOF)
        return err;

    if (token.type == TYPE_END)
    {
        token = newToken(1);
        if (token.type == TYPE_EOF)
        {
            return err;
        }
        fprintf(stderr, "Expected EOF after \"?>\" on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return (ERR_SYN);
    }


    //vas kod
    fprintf(stderr, "EXPRESSION %d\n", token.type);

    return err;
}

/**
 * @brief main parser function
 * 
 * @return int error code
 */
int parser(Token *tokenArrIN)
{
    tokenArr = tokenArrIN;
    int err;
    globalST = ST_initTable(16);
    localST = ST_initTable(8);

    // prog
    err = ruleProg(); // remove tokenArr SIMULATION
    return err;

    freeST();
    // printf("Parser OK!\n");
    return 0;
}