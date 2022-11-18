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
#define isKeywordType(KEYWORD) (KEYWORD == KEYWORD_INT || KEYWORD == KEYWORD_FLOAT || KEYWORD == KEYWORD_STRING || KEYWORD == KEYWORD_NULL)

Symtable *globalST; // global symtable
Symtable *localST;  // local symtable
int isGlobal = 1;   // program is not in function
TokenType returnType = TYPE_VOID;   //Return type of currently parsed function

const int precTable[6][6] = {
    {R, L, L, R, L, R},  // +
    {R, R, L, R, L, R},  // *
    {L, L, L, E, L, N},  // (
    {R, R, N, R, N, R},  // )
    {R, R, N, R, N, R},  // id
    {L, L, L, N, L, N}}; // $
                         // +  *  (  )  id $

Token token, prevToken;

Token *tokenArr; // simulation

#define numOfExprRules 12
// rules are flipped because of stack
TokenType exprRules[numOfExprRules][3] = {
    {TYPE_ID, TYPE_UNDEF, TYPE_UNDEF},
    {TYPE_INT, TYPE_UNDEF, TYPE_UNDEF},
    {TYPE_FLOAT, TYPE_UNDEF, TYPE_UNDEF},
    {TYPE_STRING, TYPE_UNDEF, TYPE_UNDEF},
    {TYPE_FUNID, TYPE_UNDEF, TYPE_UNDEF},
    {TYPE_EXPR, TYPE_ADD, TYPE_EXPR},
    {TYPE_EXPR, TYPE_SUB, TYPE_EXPR},
    {TYPE_EXPR, TYPE_MUL, TYPE_EXPR},
    {TYPE_EXPR, TYPE_COMMA, TYPE_EXPR},
    {TYPE_RBRACKET, TYPE_EXPR, TYPE_LBRACKET},
    {TYPE_EXPR, TYPE_DIV, TYPE_EXPR},
    {TYPE_SUB, TYPE_INT, TYPE_UNDEF}};

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
 * @brief Converts keyword type to variable type (tokne type)
 * 
 * @param keyword Keyword of the type to convert
 * (KEYWORD_INT, KEYWORD_FLOAT, KEYWORD_STRING, KEYWORD_NULL, KEYWORD_VOID)
 * @return Token type for constant of given keyword type
 * (for other keywords returns TYPE_UNDEF)
 */
TokenType keywordType2VarType(KeyWord keyword){
    switch (keyword)
    {
    case KEYWORD_INT:
        return TYPE_INT;
        break;
    case KEYWORD_FLOAT:
        return TYPE_FLOAT;
        break;
    case KEYWORD_STRING:
        return TYPE_STRING;
        break;
    case KEYWORD_NULL:
        return TYPE_NULL;
        break;
    case KEYWORD_VOID:
        return TYPE_VOID;
        break;
    default:
        return TYPE_UNDEF;
        break;
    }
    return TYPE_UNDEF;
}

/**
 * @brief Get the char representation of TokenType ('i','f','s')
 *
 * @param type Type of token
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
    ErrorType tmpErr = 0;

    while (1)
    {        
        if (err)
            return err;

        //<return>
        tmpErr = ruleReturn();
        if(!err){
            err = tmpErr;
        }

        // EPILOG
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

        // Check end of {<stat-list>}
        if (token.type == TYPE_RBRACES)
            break;

        // statement rule
        tmpErr = ruleStat();
        if(!err){
            err = tmpErr;
        }
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
            if (token.type != TYPE_LBRACKET)
            {
                fprintf(stderr, "Expected \"(\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }

            // (<expr>)
            err = exprAnal(&varType,0);

            // {
            if (token.type != TYPE_LBRACES)
            {
                fprintf(stderr, "Expected \"{\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <stat_list>
            errTmp = ruleStatList();
            if (err == 0)
            {
                err = errTmp;
            }

            // }
            if (token.type != TYPE_RBRACES)
            {
                fprintf(stderr, "Expected \"}\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // ELSE
            errTmp = 0;
            if (token.type != TYPE_KEYWORD)
            {
                errTmp = 2;
            }
            else if (token.attribute.keyword != KEYWORD_ELSE)
            {
                errTmp = 2;
            }
            if (errTmp)
            {
                fprintf(stderr, "Expected ELSE after \"}\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // {
            if (token.type != TYPE_LBRACES)
            {
                fprintf(stderr, "Expected \"{\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <stat_list>
            errTmp = ruleStatList();
            if (err == 0)
            {
                err = errTmp;
            }

            // }
            if (token.type != TYPE_RBRACES)
            {
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
            if (token.type != TYPE_LBRACKET)
            {
                fprintf(stderr, "Expected \"(\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }

            // (<expr>)
            err = exprAnal(&varType,0);

            // {
            if (token.type != TYPE_LBRACES)
            {
                fprintf(stderr, "Expected \"{\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // <stat_list>
            errTmp = ruleStatList();
            if (err == 0)
            {
                err = errTmp;
            }

            // }
            if (token.type != TYPE_RBRACES)
            {
                fprintf(stderr, "Expected \"}\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);
            break;

        case KEYWORD_FUNCTION:
            token = newToken(0);

            // FUNID
            if (token.type != TYPE_FUNID)
            {
                fprintf(stderr, "Expected function name after \"function\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }
            token = newToken(0);

            // (
            if (token.type != TYPE_LBRACKET)
            {
                fprintf(stderr, "Expected \"(\" on line %d!\n", token.rowNumber);
                makeError(ERR_SYN);
                return ERR_SYN;
                break;
            }

            // <params>
            err = ruleParams();

            // <funcdef>
            errTmp = ruleFuncdef();
            if (err == 0)
            {
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

/**
 * @brief function definition rule
 * 
 * @return ErrorType 
 */
//udelat: pridat fci do symtablu
ErrorType ruleFuncdef()
{
    ErrorType err = 0;
    ErrorType tmpErr = 0;
    isGlobal = 0;   //Set parsing in function
    returnType = TYPE_VOID; //Set unspecified return type


    // : TYPE
    if(token.type == TYPE_COLON){
        token = newToken(0);

        // TYPE
        if(token.type != TYPE_KEYWORD){
            tmpErr = 1;
        }else if(!isKeywordType(token.attribute.keyword)){
            tmpErr = 1;
        }
        if(tmpErr){
            fprintf(stderr, "Expected return type after \":\" on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return ERR_SYN;
        }
        returnType = keywordType2VarType(token.attribute.keyword);  //Set specified return type
        token = newToken(0);
    }
    
    // {
    if (token.type != TYPE_LBRACES){
        fprintf(stderr, "Expected \"{\" or \":\" on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return ERR_SYN;
    }
    token = newToken(0);

    err = ruleStatList();

    // }
    if (token.type != TYPE_RBRACES){
        fprintf(stderr, "Expected \"}\" on line %d!\n", token.rowNumber);
        makeError(ERR_SYN);
        return ERR_SYN;
    }
    token = newToken(0);

    //add to symtable

    returnType = TYPE_VOID; //Set unspecified return type
    isGlobal = 1; //Set parsing in global scale

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
        err = exprAnal(&varType, 0);
        if (token.type != TYPE_SEMICOLON)
        {
            fprintf(stderr, "Expected \";\" on line %d!\n", token.rowNumber);
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
            err = exprAnal(&varType, 0);
            if (token.type != TYPE_SEMICOLON)
            {
                fprintf(stderr, "Expected \";\" on line %d!\n", token.rowNumber);
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
            err = exprAnal(&varType, 1);
            if (token.type != TYPE_SEMICOLON)
            {
                fprintf(stderr, "Expected \";\" on line %d!\n", token.rowNumber);
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
ErrorType ruleParams()
{
    ErrorType err = 0;
    
    //(<params>)
    while(token.type != TYPE_RBRACKET){
        token = newToken(0);
    }
    token = newToken(0);

    return err;
}

/**
 * @brief return rule
 * 
 * @return ErrorType 
 */
ErrorType ruleReturn(){
    ErrorType err = 0;
    char varType = 'i';

    // RETURN
    if(token.type != TYPE_KEYWORD) return 0;                //epsilon
    if(token.attribute.keyword != KEYWORD_RETURN) return 0; //epsilon
    token = newToken(0);
    
    // <expr> ;
    if(token.type != TYPE_SEMICOLON){
        err = exprAnal(&varType, 0);
        
        // ;
        if(token.type != TYPE_SEMICOLON){
            fprintf(stderr, "Expected \";\" on line %d!\n", token.rowNumber);
            makeError(ERR_SYN);
            return (ERR_SYN);
        }
        token = newToken(0);

        //Returns a value but is of type void
        if(returnType == TYPE_VOID && isGlobal == 0 && err == 0){
            fprintf(stderr, "Function is of type void, but returns a value on line %d!\n", token.rowNumber);
            makeError(ERR_EXPRES);
            return (ERR_EXPRES);
        }

        return err;
    } // ;
    else if(returnType != TYPE_VOID && isGlobal == 0){   //Must be ";" | checking return value
        fprintf(stderr, "No return value in non-void function on line %d!\n", token.rowNumber);
        makeError(ERR_EXPRES);
        return (ERR_EXPRES);
    }
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
    if (isValueType(token.type))
        return 4;

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

int exprUseRule(TokenType *typeArr)
{

    /*for (int i = 0; i < 3; i++)
    {
        printf("%d ",typeArr[i]);
    }
    printf("\n");
*/
    for (int rule = 0; rule < numOfExprRules; rule++)
    {
        int match = 1;
        for (int i = 0; i < 3; i++)
        {
            if (typeArr[i] != exprRules[rule][i])
            {
                match *= 0;
                break;
            }
        }
        if (match)
        {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief process expression
 *
 * @param varType address
 * @return ErrorType
 */
ErrorType exprAnal(int *isEmpty, int usePrevToken)
{
    Token tmpToken, endToken;
    ErrorType err = 0;
    int done = 0;
    *isEmpty = 0;
    Stack *stack = STACK_init();

    // init expression stack
    tmpToken.type = TYPE_STACKEMPTY;
    STACK_push(stack, tmpToken);

    if (usePrevToken)
    {
        tmpToken.type = TYPE_LESSPREC;
        STACK_push(stack, tmpToken);
        STACK_push(stack, prevToken);

        // nothing to proccess after prevToken
        if (!isOperatorType(token.type) && !isValueType(token.type) && !isBracket(token.type))
        {
            endToken = token;
            token.type = TYPE_STACKEMPTY;
            done = 1;
        }
    }
    else
    {
        // empty expression
        if (!isOperatorType(token.type) && !isValueType(token.type) && !isBracket(token.type))
        {
            STACK_dispose(stack);
            *isEmpty = 1;
            return err;
        }
    }

    while (1)
    {
        TokenType tokenTypeArr[3] = {TYPE_UNDEF, TYPE_UNDEF, TYPE_UNDEF};
        int stackPrecIndex = getPrecTableIndex(*STACK_top(stack));
        int tokenPrecIndex = getPrecTableIndex(token);

        if (STACK_top(stack)->type == TYPE_EXPR)
        {
            STACK_pop(stack);
            stackPrecIndex = getPrecTableIndex(*STACK_top(stack));
            tmpToken.type = TYPE_EXPR;
            STACK_push(stack, tmpToken);
        }

        if (stackPrecIndex == -1 || tokenPrecIndex == -1)
        {
            printf("INDEX ERR\n");
            STACK_dispose(stack);
            makeError(ERR_SYN);
            return ERR_SYN;
        }

        switch (precTable[stackPrecIndex][tokenPrecIndex])
        {
        case E: // =
            STACK_push(stack, token);
            token = newToken(0);
            break;

        case L: // <
            if (isOperatorType(token.type) && STACK_top(stack)->type == TYPE_EXPR)
            {
                STACK_pop(stack);
                tmpToken.type = TYPE_LESSPREC;
                STACK_push(stack, tmpToken);
                tmpToken.type = TYPE_EXPR;
                STACK_push(stack, tmpToken);
            }
            else
            {
                tmpToken.type = TYPE_LESSPREC;
                STACK_push(stack, tmpToken);
            }

            STACK_push(stack, token);
            token = newToken(0);
            break;

        case R: // >
            // pop beteween < and >
            for (int i = 0; STACK_top(stack)->type != TYPE_LESSPREC; i++)
            {
                tokenTypeArr[i] = STACK_top(stack)->type;
                STACK_pop(stack);
                if (i > 2)
                {
                    printf("ARRAY ERR\n");
                    STACK_dispose(stack);
                    makeError(ERR_SYN);
                    return ERR_SYN;
                }
            }

            // check expression rules
            if (exprUseRule(tokenTypeArr))
            {
                if(token.type == TYPE_STACKEMPTY){
                    token = endToken;
                }
                printf("RULE ERR\n");
                STACK_dispose(stack);
                makeError(ERR_SYN);
                return ERR_SYN;
            }

            STACK_pop(stack); // pop <

            tmpToken.type = TYPE_EXPR;
            STACK_push(stack, tmpToken);
            break;

        case N: // error
            STACK_dispose(stack);
            printf("EXPR ERR\n");
            makeError(ERR_SYN);
            return ERR_SYN;
            break;
        }

        // non-expression token loaded
        if (!isOperatorType(token.type) && !isValueType(token.type) && !isBracket(token.type) && !done)
        {
            endToken = token;
            token.type = TYPE_STACKEMPTY;
            done = 1;
        }

        // expression is correct
        if (done && STACK_top(stack)->type == TYPE_EXPR)
        {
            STACK_pop(stack);
            if (STACK_top(stack)->type == TYPE_STACKEMPTY)
            {
                token = endToken;
                break;
            }
            tmpToken.type = TYPE_EXPR;
            STACK_push(stack, tmpToken);
        }
    }

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

    // generateBuiltInFunc();
    //  <prog> => BEGIN DECLARE_ST <stat_list>
    err = ruleProg();

    freeST();
    // printf("Parser OK!\n");
    return err; // predelat na exit
}