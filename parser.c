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
#define isBracket(TYPE) (TYPE == TYPE_LBRACKET || TYPE == TYPE_RBRACKET)

Symtable *globalST; // global symtable
Symtable *localST;  // local symtable
int isGlobal = 1;   // program is not in function

int precTable[6][6] = {
    {R, L, L, R, L, R},  // +
    {R, R, L, R, L, R},  // *
    {L, L, L, E, L, N},  // (
    {R, R, N, R, N, R},  // )
    {R, R, N, R, N, R},  // id
    {L, L, L, N, L, N}}; // $
  // +  *  (  )  id $

Token token, prevToken;

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
    token = newToken(0);

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
        
        if (err)
            return err;

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
ErrorType ruleStat()
{
    ErrorType err = 0;
    ErrorType errTmp = 0;
    char varType;

    if (token.type == TYPE_KEYWORD)
    {
        switch (token.attribute.keyword)
        {
        case KEYWORD_IF:
            token = newToken(0);

            // (
            if(token.type != TYPE_LBRACKET){
                fprintf(stderr, "Expected \"(\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <expr>
            err = exprAnal(&varType,0);

            // )
            if(token.type != TYPE_RBRACKET){
                fprintf(stderr, "Expected \")\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // {
            if(token.type != TYPE_LBRACES){
                fprintf(stderr, "Expected \"{\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <stat_list>
            errTmp = ruleStatList();
            if(err == 0){
                err = errTmp;
            }

            // }
            if(token.type != TYPE_RBRACES){
                fprintf(stderr, "Expected \"}\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // ELSE
            errTmp = 0;
            if(token.type != TYPE_KEYWORD){
                errTmp = 2;
            }else if(token.attribute.keyword != KEYWORD_ELSE){
                errTmp = 2;
            }
            if(errTmp){
                fprintf(stderr, "Expected ELSE after \"}\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // {
            if(token.type != TYPE_LBRACES){
                fprintf(stderr, "Expected \"{\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <stat_list>
            errTmp = ruleStatList();
            if(err == 0){
                err = errTmp;
            }

            // }
            if(token.type != TYPE_RBRACES){
                fprintf(stderr, "Expected \"}\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);
            break;

        case KEYWORD_WHILE:
            token = newToken(0);

            // (
            if(token.type != TYPE_LBRACKET){
                fprintf(stderr, "Expected \"(\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <expr>
            err = exprAnal(&varType,0);

            // )
            if(token.type != TYPE_RBRACKET){
                fprintf(stderr, "Expected \")\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // {
            if(token.type != TYPE_LBRACES){
                fprintf(stderr, "Expected \"{\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <stat_list>
            errTmp = ruleStatList();
            if(err == 0){
                err = errTmp;
            }

            // }
            if(token.type != TYPE_RBRACES){
                fprintf(stderr, "Expected \"}\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);
            break;

        case KEYWORD_FUNCTION:
            token = newToken(0);

            //FUNID
            if(token.type != TYPE_FUNID){
                fprintf(stderr, "Expected function name after \"function\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // (
            if(token.type != TYPE_LBRACKET){
                fprintf(stderr, "Expected \"(\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <params>
            err = ruleParams();

            // )
            if(token.type != TYPE_RBRACKET){
                fprintf(stderr, "Expected \")\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <funcdef>
            errTmp = ruleFuncdef();
            if(err == 0){
                err = errTmp;
            }
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
/**
 * @brief assign rule
 *
 * @return ErrorType
 */
ErrorType ruleAssign()
{
    ErrorType err = 0;
    char varType;

    // <assign> => <expr> ;
    if (token.type != TYPE_ID)
    {
        err = exprAnal(&varType,0);
        if (token.type != TYPE_SEMICOLON)
        {
            makeError(ERR_SYN);
            return ERR_SYN;
        }
        token = newToken(0);
        return err;
    }
    else // <assign> => ID <expr> ;
    {
        STItem *item = ST_searchTable(getTable(isGlobal), DS_string(token.attribute.dString));
        prevToken = token;
        token = newToken(0);

        if (token.type == TYPE_ASSIGN) // <assign> => ID = <expr> ;
        {
            token = newToken(0);
            err = exprAnal(&varType,0);
            if (token.type != TYPE_SEMICOLON)
            {
                makeError(ERR_SYN);
                return ERR_SYN;
            }
            token = newToken(0);


            /*if (varType == 0)
            {
                //printf("EMPTY");
                return err;
            }*/

            if (item == 0) // not found in ST
            {
                STItemData STdata;
                STdata.varData.VarType = varType;
                ST_insertItem(getTable(isGlobal), DS_string(prevToken.attribute.dString), ST_ITEM_TYPE_VARIABLE, STdata);
            }
            else // update type
            {
                ST_updateVarType(getTable(isGlobal), DS_string(prevToken.attribute.dString), varType);
            }
        }
        else
        {
            err = exprAnal(&varType,1);
            if (token.type != TYPE_SEMICOLON)
            {
                makeError(ERR_SYN);
                return ERR_SYN;
            }
            token = newToken(0);
        }

    }

    return err;
}

/**
 * @brief params rule
 * 
 * @return ErrorType 
 */
ErrorType ruleParams(){
    ErrorType err = 0;
    token = newToken(0);

    return err;
}

/**
 * @brief returns precedens of operand
 *
 * @param type
 * @return int
 */
int getPrecTableIndex(Token token)
{   
    if (isValueType(token.type)) return 4;

    switch (token.type)
    {
    case TYPE_ADD:
    case TYPE_SUB:
        return 0;

    case TYPE_MUL:
    case TYPE_DIV:
        return 1;

    case TYPE_LBRACKET:
        return 2;

    case TYPE_RBRACKET:
        return 3;

    case TYPE_STACKEMPTY:
        return 5;

    default:
        break;
    }

    return -1;
}

int useRule(TokenType *tokenArr){
    for (int i=0; i<3;i++){

    }
    return 0;
}

/**
 * @brief process expression
 *
 * @param varType address
 * @return ErrorType
 */
ErrorType exprAnal(char *varType, int usePrevToken)
{
    ErrorType err = 0;
    int done = 0;
    Stack *stack = STACK_init();
    Token tmpToken, endToken;

    tmpToken.type = TYPE_STACKEMPTY;
    STACK_push(stack, tmpToken);

    if (usePrevToken)
    {
        tmpToken.type = TYPE_LESSPREC;
        STACK_push(stack, tmpToken);
        STACK_push(stack, prevToken);
    }

    while (1)
    {
        TokenType tokenTypeArr[3] = {TYPE_UNDEF,TYPE_UNDEF,TYPE_UNDEF};
        int stackPrecIndex = getPrecTableIndex(*STACK_top(stack));
        int tokenPrecIndex = getPrecTableIndex(token);

        if (STACK_top(stack)->type == TYPE_EXPR){
            STACK_pop(stack);
            stackPrecIndex = getPrecTableIndex(*STACK_top(stack));
            tmpToken.type = TYPE_EXPR;
            STACK_push(stack, tmpToken);
        }

        switch (precTable[stackPrecIndex][tokenPrecIndex])
        {
            case E: // =
                STACK_push(stack, token);
                token = newToken(0);
                break;

            case L: // <
                //printf("LEFT");
                if (isOperatorType(token.type)){
                    STACK_pop(stack);
                    tmpToken.type = TYPE_LESSPREC;
                    STACK_push(stack, tmpToken);
                    tmpToken.type = TYPE_EXPR;
                    STACK_push(stack, tmpToken);
                }else{
                    tmpToken.type = TYPE_LESSPREC;
                    STACK_push(stack, tmpToken);
                }

                STACK_push(stack, token);
                token = newToken(0);
                break;

            case R: // >
                for (int index = 0; STACK_top(stack)->type != TYPE_LESSPREC; index++) // pop beteween < and >
                { 
                    if(index > 2){
                        makeError(ERR_SYN);
                        return ERR_SYN;
                    }
                    tokenTypeArr[index] = STACK_top(stack)->type;
                    STACK_pop(stack);
                }

                if(useRule(tokenTypeArr)){
                    makeError(ERR_SYN);
                    return ERR_SYN;
                }

                STACK_pop(stack); // pop <

                tmpToken.type = TYPE_EXPR;
                STACK_push(stack, tmpToken);
                break;

            case N: // error
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
        }

        if(done && STACK_top(stack)->type == TYPE_EXPR){
            STACK_pop(stack);
            if(STACK_top(stack)->type == TYPE_STACKEMPTY){
                token = endToken;
                break;
            }
            tmpToken.type = TYPE_EXPR;
            STACK_push(stack, tmpToken);
        }

        if(!isOperatorType(token.type) && !isValueType(token.type) && !isBracket(token.type) && !done){
            endToken = token;
            token.type = TYPE_STACKEMPTY;
            done = 1;
        }
    }

    // todo zapis typu vyrazu
    *varType = 'i';
    STACK_dispose(stack);
    return err;
}

/**
 * @brief main parser function
 *
 * @return int error code
 */
int parser(Token *tokenArrIN)
{
    tokenArr = tokenArrIN; // sim

    ErrorType err;
    globalST = ST_initTable(16);
    localST = ST_initTable(8);

    //generateBuiltInFunc();
    // <prog> => BEGIN DECLARE_ST <stat_list>
    err = ruleProg();

    freeST();
    // printf("Parser OK!\n");
    return err; // predelat na exit
}