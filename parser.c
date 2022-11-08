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
#define canBeAssigned(TYPE) (TYPE == TYPE_INT || TYPE == TYPE_DOUBLE || TYPE == TYPE_STRING || TYPE == TYPE_FUNID)
#define isExpression(TYPE) (TYPE == TYPE_ADD || TYPE == TYPE_SUB || TYPE == TYPE_MUL || TYPE == TYPE_DIV || TYPE == TYPE_MOD \
|| TYPE == TYPE_EQTYPES || TYPE == TYPE_NOTEQTYPES || TYPE == TYPE_LESS || TYPE == TYPE_GREATER || TYPE == TYPE_LESSEQ ||    \
 TYPE == TYPE_GREATEREQ || TYPE == TYPE_CONCAT)

Symtable *globalST;
Symtable *localST;
int isGlobal = 1;

Token *tokenArr; // simulation

void freeAll(Symtable *globalST)
{
    if (globalST != NULL)
        ST_freeTable(globalST);
}

STItem *serveSymTable(Symtable *table, Token token)
{
    STItem *item = ST_searchTable(table, DS_string(token.attribute.dString));
    if (item == NULL)
    {

    }
    else
    { // item found in ST
        printf("Incoming item is already in globalST: %s\n", item->key);
    }

    return item;
}

int checkProlog(Token *tokenArr) // remove tokenArr SIMULATION
{
    Token token = getTokenSim(tokenArr);

    if (token.type != TYPE_BEGIN)
    {
        fprintf(stderr, "Expected <?php at beggining of the file!\n");
        return (ERR_SYN);
    }

    token = getTokenSim(tokenArr); // simulation
    while (token.type == TYPE_COMM)
    {
        token = getTokenSim(tokenArr);
    }

    if (token.type != TYPE_DECLARE_ST)
    {
        fprintf(stderr, "Expected \"declare(strict_types=1);\" at beggining of the file!\n");
        return (ERR_SYN);
    }

    token = getTokenSim(tokenArr);
    if (token.type != TYPE_SEMICOLON)
    {
        fprintf(stderr, "Expected \";\" after declare(strict_types=1) on line %d!\n", token.rowNumber);
        return (ERR_SYN);
    }
    
    return 0;
}

Symtable *getTable(int global)
{
    if (global)
        return globalST;
    return localST;
}

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

int getTypeChar(TokenType type)
{
    if (type == TYPE_INT)
        return 'i';
    else if(type == TYPE_DOUBLE)
        return 'd';
    else if (type == TYPE_STRING)
        return 's';
    return 0;
}

Token nextToken(Stack *tokenStack) // odstranit tokenarr
{
    if(STACK_bottom(tokenStack) != NULL) // je nejaky token na zasobniku
    {
        Token token = *STACK_bottom(tokenStack);
        STACK_popBottom(tokenStack);
        return token;
    }
    return getTokenSim(tokenArr);
}

int ruleAssign(Stack *tokenStack, Stack *programStack)
{
    int err;
    Token token = nextToken(tokenStack);
    Token IDtoken = token;

    //search var ID in ST
    STItem *item = ST_searchTable(globalST, DS_string(IDtoken.attribute.dString));

    STACK_push(programStack, token);
    token = nextToken(tokenStack);
    STACK_push(programStack, token);
    
    // after ID =
    token = nextToken(tokenStack);
    STACK_push(tokenStack, token);
    TokenType typeFound = TYPE_UNDEF;
    while (token.type != TYPE_SEMICOLON){
        if (canBeAssigned(token.type)){
            if(!isSameType(&typeFound, token.type)){
                return ERR_TYPE;
            }

            if (token.type == typeFound && typeFound == TYPE_FUNID)
            {
                return ERR_TYPE;
            }
        }
        if ((!isExpression(token.type)) && (!canBeAssigned(token.type)))
        {
            return ERR_TYPE;
        }
        token = getTokenSim(tokenArr);
        STACK_push(tokenStack, token);
    }

    if (item == NULL) // not found
    { 
        STItemData STdata;
        STdata.varData.VarType = getTypeChar(typeFound);
        STdata.varData.initialized = 1;
        //STdata.varData.VarPosition   --dodelat
        ST_insertItem(globalST, DS_string(IDtoken.attribute.dString), ST_ITEM_TYPE_VARIABLE, STdata);
    }
    else // check type of ID and assigned expression
    {
        if (getTypeChar(typeFound) != item->data.varData.VarType)
            return ERR_TYPE;
    }

    // programStack: ID =
    STACK_popAll(tokenStack); // az bude ruleExpression smazat!
    err = nextRule(tokenStack, programStack);
    return err;
}

int ruleExpression(Stack *tokenStack, Stack *programStack)
{
    int err;
    Token token = nextToken(tokenStack);
    //vas kod
    fprintf(stderr, "EXPRESSION %d\n", token.type);
    //az bude konec radku
    //err = generateCode(tokenStack);

    err = nextRule(tokenStack, programStack);
    return err;

    return 0;
}

int ruleIf(Stack *tokenStack, Stack *programStack)
{
    int err;
    Token token = nextToken(tokenStack);
    //vas kod
    fprintf(stderr, "KEYWORD IF\n");

    err = nextRule(tokenStack, programStack);
    return err;

    return 0;
}

int ruleWhile(Stack *tokenStack, Stack *programStack)
{
    int err;
    Token token = nextToken(tokenStack);
    // vas kod
    fprintf(stderr, "KEYWORD WHILE\n");

    err = nextRule(tokenStack, programStack);
    return err;
}

int ruleFunccal(Stack *tokenStack, Stack *programStack)
{
    int err;
    Token token = nextToken(tokenStack);
    // vas kod
    fprintf(stderr, "FUNCAL\n");

    err = nextRule(tokenStack, programStack);
    return err;
}

int ruleFuncdef(Stack *tokenStack, Stack *programStack)
{
    int err;
    Token token = nextToken(tokenStack);
    //vas kod
    fprintf(stderr, "FUNCDEF\n");

    err = nextRule(tokenStack, programStack);
    return err;

    return 0;
}

/**
 * @brief calls next rule
 * 
 * @param tokenStack 
 * @return int error code
 */
int nextRule(Stack *tokenStack, Stack *programStack)
{
    int err = 0;
    Token token = nextToken(tokenStack);//simulation
    //fprintf(stderr, "TOKEN: %d\n", token.type);
    switch(token.type){
        case TYPE_ID:
            STACK_push(tokenStack, token);
            token = getTokenSim(tokenArr); // getToken
            STACK_push(tokenStack, token);
            //fprintf(stderr,"TYPE: %d", token.type);

            if(token.type == TYPE_ASSIGN)
                err = ruleAssign(tokenStack, programStack);

            else if (isExpression(token.type))
                err = ruleExpression(tokenStack, programStack);

                else
                { // unexpected type after TYPE_ID
                    token = *STACK_bottom(tokenStack);
                    fprintf(stderr, "Unexpected character after $%s on line %d!\n", DS_string(token.attribute.dString), token.rowNumber);
                    return (ERR_SYN);
            }
            break;

        case TYPE_KEYWORD:
            STACK_push(tokenStack, token);
            if (isKeyword(token, KEYWORD_IF)){
                err = ruleIf(tokenStack, programStack);
            }

            else if (isKeyword(token, KEYWORD_WHILE))
            {
                err = ruleWhile(tokenStack, programStack);
            }

            else if (isKeyword(token, KEYWORD_FUNCTION))
            {
                token = getTokenSim(tokenArr); // getToken
                if (token.type != TYPE_FUNID){
                    fprintf(stderr, "Unexpected character after \"function\" on line %d!\n", token.rowNumber);
                    return (ERR_SYN);
                }

                while (token.type != TYPE_RBRACKET){ //find closing bracket after 
                    STACK_push(tokenStack, token);
                    token = getTokenSim(tokenArr);
                }
                STACK_push(tokenStack, token);
                token = getTokenSim(tokenArr);

                if (token.type == TYPE_SEMICOLON){
                    err = ruleFunccal(tokenStack, programStack);
                }
                else if (token.type == TYPE_COLON){
                    err = ruleFuncdef(tokenStack, programStack);
                }
                else
                { // unexpected type after TYPE_ID
                    token = *STACK_top(tokenStack);
                    fprintf(stderr, "Unexpected character after \"function %s\" on line %d!\n", DS_string(token.attribute.dString), token.rowNumber);
                    return (ERR_SYN);
                }
            }
            break;

        case TYPE_EOF: //epilog
            return 0;
            break;

        case TYPE_END: // epilog
            token = nextToken(tokenStack);
            if (token.type == TYPE_EOF){
                return 0;
            }
            fprintf(stderr, "Expected EOF after \"?>\" on line %d!\n", token.rowNumber);
            return (ERR_SYN);
            break;
        
        default:
            break;
    }
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
    Stack *tokenStack = STACK_init();
    Stack *programStack = STACK_init();
    globalST = ST_initTable(16);
    localST = ST_initTable(8);
    // Token token = getTokenSim(tokenArr); // simulation
    // check prolog
    err = checkProlog(tokenArr); // remove tokenArr SIMULATION
    if (err != 0)
    {
        return err;
    }

    err = nextRule(tokenStack, programStack);
    if (err != 0)
    {
        return err;
    }
    /* while tokens available - smazat
    while (token.type != TYPE_EOF)
    {
        if (token.type == TYPE_FUNID || token.type == TYPE_ID || token.type == TYPE_LABEL)
        {
            serveSymTable(globalST, token);
        }

        // free used token memory
        if (token.type == TYPE_FUNID || token.type == TYPE_ID || token.type == TYPE_LABEL)
        {
            DS_dispose(token.attribute.dString);
        }

        token = getTokenSim(tokenArr);
    } // while (token.type != TYPE_EOF)*/

    STACK_dispose(tokenStack);
    freeAll(globalST);
    // printf("Parser OK!\n");
    return 0;
}
