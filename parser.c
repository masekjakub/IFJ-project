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
#define isValueType(TYPE) (TYPE == TYPE_INT || TYPE == TYPE_FLOAT || TYPE == TYPE_STRING || TYPE == TYPE_FUNID || TYPE == TYPE_ID)
#define isOperatorType(TYPE) (TYPE == TYPE_ADD || TYPE == TYPE_SUB || TYPE == TYPE_MUL || TYPE == TYPE_DIV || TYPE == TYPE_MOD || TYPE == TYPE_EQTYPES || TYPE == TYPE_NOTEQTYPES || TYPE == TYPE_LESS || TYPE == TYPE_GREATER || TYPE == TYPE_LESSEQ || \
                          TYPE == TYPE_GREATEREQ || TYPE == TYPE_CONCAT)

Symtable *globalST; // global symtable
Symtable *localST;  // local symtable
int isGlobal = 1;   // program is not in function

int precTable[5][5] = {
    {R, L, L, R, L}, // +
    {R, R, L, R, L}, // *
    {L, L, L, E, L}, // (
    {R, R, N, R, N}, // )
    {R, R, N, R, N}  // id
};// +  *  (  )  id

Token token;

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
    token = getTokenSim(tokenArr); // odstranit tokenarr

    if (!includingComms && token.type == TYPE_COMM)
        token = newToken(includingComms);
    return token;
}

/**
 * @brief error function
 *
 * @param err
 */
void makeError(ErrorType err)
{
    // exit(err);
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
    token = newToken(1);
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

/**
 * @brief statement list rule
 * 
 * @return ErrorType 
 */
ErrorType ruleStatList()
{
    ErrorType err = 0;

    while (1)
    {
        if (err) return err;
        token = newToken(0);

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

        // statement rule
        err = ruleStat();
    }

    return err;
}

/**
 * @brief statement rule
 * 
 * @return ErrorType 
 */
// udelat: kontroly syntaxe if, while, function
ErrorType ruleStat()
{
    ErrorType err = 0;

    if (token.type == TYPE_KEYWORD)
    {
        switch (token.attribute.keyword)
        {
        case KEYWORD_IF:
            // todo
            break;
        case KEYWORD_WHILE:
            // todo
            break;
        case KEYWORD_FUNCTION:
            // todo
            break;
        default:
            fprintf(stderr, "Unexpected keyword on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return ERR_SYN;
            break;
        }
    }
    else // not keyword
    {
        err = ruleAssign();
    }
    return err;
}

/**
 * @brief identificator rule
 * 
 * @return ErrorType 
 */
/* udelat: kontrola inicializace
*/
ErrorType ruleId()
{
    ErrorType err = 0;

    return err;
}

ErrorType ruleFuncdef()
{
    ErrorType err = 0;
    token = newToken(0);

    return err;
}

ErrorType ruleAssign()
{
    ErrorType err = 0;
    char varType;

    // <assign> => <expr> ;
    if (token.type != TYPE_ID)
    {
        err = exprAnal(&varType);
        return err;
    }
    else // <assign> => ID <expr> ;
    {
        Token IDtoken = token;
        STItem *item = ST_searchTable(getTable(isGlobal), DS_string(token.attribute.dString));
        token = newToken(0);

        if (token.type == TYPE_ASSIGN) // <assign> => ID = <expr> ;
        {
            token = newToken(0);
            err = exprAnal(&varType);
            if (item == NULL) // not found in ST
            {
                STItemData STdata;
                STdata.varData.VarType = varType;
                ST_insertItem(getTable(isGlobal), DS_string(IDtoken.attribute.dString), ST_ITEM_TYPE_VARIABLE, STdata);
            }
            else // update type
            {
                ST_updateVarType(getTable(isGlobal), DS_string(IDtoken.attribute.dString), varType);
            }

        }
        else
        {
            err = exprAnal(&varType);

        }
    }

    return err;
}

ErrorType exprAnal(char *varType)
{
    ErrorType err = 0;

    if(!isOperatorType(token.type) && !isValueType(token.type)){
        return ERR_SYN;
    }
    while (isOperatorType(token.type) || isValueType(token.type)){ // tmp simulation
        token = newToken(0);
    }
    
    //todo zapis typu vyrazu
    *varType = 'i';
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

    freeST();
    // printf("Parser OK!\n");
    return err;
}